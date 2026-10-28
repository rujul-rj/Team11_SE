#include "tms/signals/signal.hpp"

#include <algorithm>
#include <cctype>
#include <istream>
#include <set>
#include <stdexcept>

namespace tms::signals {

const char* to_string(SignalState s) {
    switch (s) {
        case SignalState::RED: return "RED";
        case SignalState::GREEN: return "GREEN";
        case SignalState::YELLOW: return "YELLOW";
    }
    return "RED";
}
const char* to_string(Strategy s) {
    return s == Strategy::ADAPTIVE ? "adaptive" : "fixed";
}
const char* to_string(Stage s) {
    switch (s) {
        case Stage::GREEN: return "GREEN";
        case Stage::YELLOW: return "YELLOW";
        case Stage::ALL_RED: return "ALL_RED";
    }
    return "ALL_RED";
}

namespace {

bool valid_id(const std::string& s) {
    if (s.empty() || s.size() > static_cast<std::size_t>(Limits::kMaxIdLen)) return false;
    for (unsigned char c : s)
        if (!(std::isalnum(c) || c == '_' || c == '-')) return false;
    return true;
}

void check_range(std::vector<std::string>& errs, const std::string& what, int v, int lo, int hi) {
    if (v < lo || v > hi)
        errs.push_back(what + " = " + std::to_string(v) + " is out of range [" +
                       std::to_string(lo) + ", " + std::to_string(hi) + "]");
}

std::string join(const std::vector<std::string>& v) {
    std::string out;
    for (std::size_t i = 0; i < v.size(); ++i) out += (i ? "; " : "") + v[i];
    return out;
}

}  // namespace

// ----------------------------------------------------------------------------
// Validation (FR-002, SEC-001, SEC-002)
// ----------------------------------------------------------------------------
ValidationResult validate(const IntersectionSignalConfig& c) {
    ValidationResult r;
    auto& e = r.errors;

    if (!valid_id(c.intersection_id))
        e.push_back("intersection id '" + c.intersection_id +
                    "' must be 1-10 characters of letters, digits, '_' or '-'");
    const std::string who = "intersection '" + c.intersection_id + "': ";

    if (c.phases.size() < 2)
        e.push_back(who + "at least 2 phases are required (got " +
                    std::to_string(c.phases.size()) + ")");
    if (c.phases.size() > static_cast<std::size_t>(Limits::kMaxPhases))
        e.push_back(who + "at most " + std::to_string(Limits::kMaxPhases) + " phases are allowed");

    check_range(e, who + "all_red", c.all_red_s, Limits::kMinAllRed, Limits::kMaxAllRed);

    std::set<std::string> seen_roads, seen_names;
    for (const auto& p : c.phases) {
        const std::string pw = who + "phase '" + p.name + "': ";
        if (!valid_id(p.name)) e.push_back(pw + "phase name must be 1-10 characters of letters, digits, '_' or '-'");
        else if (!seen_names.insert(p.name).second) e.push_back(pw + "duplicate phase name");

        if (p.roads.empty()) e.push_back(pw + "must serve at least one road");
        for (const auto& road : p.roads) {
            if (!valid_id(road)) {
                e.push_back(pw + "invalid road id '" + road + "'");
            } else if (!seen_roads.insert(road).second) {
                e.push_back(pw + "road '" + road +
                            "' is assigned to more than one phase (would allow conflicting greens)");
            }
        }
        check_range(e, pw + "green", p.green_s, Limits::kMinGreen, Limits::kMaxGreen);
        check_range(e, pw + "yellow", p.yellow_s, Limits::kMinYellow, Limits::kMaxYellow);
    }

    if (c.strategy == Strategy::ADAPTIVE) {
        const auto& a = c.adaptive;
        check_range(e, who + "adaptive min_green", a.min_green_s, Limits::kMinGreen, Limits::kMaxGreen);
        check_range(e, who + "adaptive max_green", a.max_green_s, Limits::kMinGreen, Limits::kMaxGreen);
        if (a.max_green_s < a.min_green_s) e.push_back(who + "adaptive max_green must be >= min_green");
        check_range(e, who + "adaptive threshold", a.extension_threshold, 1, Limits::kMaxThreshold);
    }

    r.ok = e.empty();
    return r;
}

// ----------------------------------------------------------------------------
// SignalController
// ----------------------------------------------------------------------------
SignalController::SignalController(IntersectionSignalConfig cfg) : cfg_(std::move(cfg)) {
    const auto v = validate(cfg_);
    if (!v.ok) throw std::invalid_argument("invalid signal configuration: " + join(v.errors));
    for (std::size_t i = 0; i < cfg_.phases.size(); ++i)
        for (const auto& road : cfg_.phases[i].roads) {
            road_phase_[road] = i;
            queues_[road] = 0;
        }
}

void SignalController::tick(int seconds) {
    for (int i = 0; i < seconds; ++i) tick_one();
}

void SignalController::tick_one() {
    ++elapsed_;
    ++time_;
    switch (stage_) {
        case Stage::GREEN:
            on_green();
            break;
        case Stage::YELLOW:
            if (elapsed_ >= cfg_.phases[current_].yellow_s) enter_all_red();
            break;
        case Stage::ALL_RED:
            if (elapsed_ >= cfg_.all_red_s) {
                // An emergency that arrived mid-transition is served next.
                if (auto t = emergency_target()) next_ = *t;
                enter_green(next_);
            }
            break;
    }
}

void SignalController::on_green() {
    if (auto tgt = emergency_target()) {
        if (*tgt == current_) return;          // already green for the emergency: hold
        next_ = *tgt;                          // leave green the safe way (yellow -> all-red)
        enter_yellow();
        return;
    }
    bool end = false;
    if (cfg_.strategy == Strategy::FIXED_TIME) {
        end = elapsed_ >= cfg_.phases[current_].green_s;
    } else {
        const auto& a = cfg_.adaptive;
        const int q = phase_queue(current_);
        end = elapsed_ >= a.min_green_s && (q < a.extension_threshold || elapsed_ >= a.max_green_s);
    }
    if (end) {
        next_ = choose_next();
        enter_yellow();
    }
}

void SignalController::enter_yellow() { stage_ = Stage::YELLOW; elapsed_ = 0; }
void SignalController::enter_all_red() { stage_ = Stage::ALL_RED; elapsed_ = 0; }
void SignalController::enter_green(std::size_t phase) {
    current_ = phase;
    stage_ = Stage::GREEN;
    elapsed_ = 0;
    ++phase_changes_;
}

std::size_t SignalController::choose_next() const {
    const std::size_t n = cfg_.phases.size();
    std::size_t best = (current_ + 1) % n;
    if (cfg_.strategy == Strategy::FIXED_TIME) return best;
    int best_q = phase_queue(best);
    for (std::size_t k = 2; k < n; ++k) {
        const std::size_t idx = (current_ + k) % n;
        const int q = phase_queue(idx);
        if (q > best_q) { best = idx; best_q = q; }   // ties keep cyclic order
    }
    return best;
}

int SignalController::phase_queue(std::size_t phase) const {
    int sum = 0;
    for (const auto& road : cfg_.phases[phase].roads) {
        auto it = queues_.find(road);
        if (it != queues_.end()) sum += it->second;
    }
    return sum;
}

std::optional<std::size_t> SignalController::emergency_target() const {
    if (emergency_order_.empty()) return std::nullopt;
    return road_phase_.at(emergency_order_.front());
}

bool SignalController::set_queue_length(const std::string& road, int queue) {
    auto it = queues_.find(road);
    if (it == queues_.end() || queue < 0) return false;
    it->second = queue;
    return true;
}

void SignalController::set_queue_lengths(const std::map<std::string, int>& q) {
    for (const auto& [road, len] : q) set_queue_length(road, len);
}

bool SignalController::request_emergency(const std::string& road) {
    if (!road_phase_.count(road)) return false;
    if (++emergency_count_[road] == 1) emergency_order_.push_back(road);
    return true;
}

bool SignalController::clear_emergency(const std::string& road) {
    auto it = emergency_count_.find(road);
    if (it == emergency_count_.end()) return false;
    if (--it->second == 0) {
        emergency_count_.erase(it);
        emergency_order_.erase(std::remove(emergency_order_.begin(), emergency_order_.end(), road),
                               emergency_order_.end());
    }
    return true;
}

SignalState SignalController::state(const std::string& road) const {
    auto it = road_phase_.find(road);
    if (it == road_phase_.end()) return SignalState::RED;
    if (it->second != current_) return SignalState::RED;
    switch (stage_) {
        case Stage::GREEN: return SignalState::GREEN;
        case Stage::YELLOW: return SignalState::YELLOW;
        case Stage::ALL_RED: return SignalState::RED;
    }
    return SignalState::RED;
}

std::map<std::string, SignalState> SignalController::snapshot() const {
    std::map<std::string, SignalState> s;
    for (const auto& kv : road_phase_) s[kv.first] = state(kv.first);
    return s;
}

bool SignalController::invariants_hold() const {
    std::set<std::size_t> active;
    bool any_green = false, any_yellow = false;
    for (const auto& [road, st] : snapshot()) {
        if (st == SignalState::GREEN) { any_green = true; active.insert(road_phase_.at(road)); }
        if (st == SignalState::YELLOW) { any_yellow = true; active.insert(road_phase_.at(road)); }
    }
    if (active.size() > 1) return false;               // conflicting phases
    if (any_green && any_yellow) return false;         // mixed states
    if (stage_ == Stage::ALL_RED && (any_green || any_yellow)) return false;
    if (stage_ == Stage::GREEN && !any_green) return false;
    if (stage_ == Stage::YELLOW && !any_yellow) return false;
    return true;
}

// ----------------------------------------------------------------------------
// SignalManager
// ----------------------------------------------------------------------------
void SignalManager::log_invalid(const std::string& source, const std::vector<std::string>& errors) {
    for (const auto& msg : errors) {
        const std::string line = "INVALID_CONFIG [" + source + "]: " + msg;
        log_.push_back(line);
        if (sink_) sink_(line);
    }
}

ValidationResult SignalManager::configure(const IntersectionSignalConfig& cfg) {
    ValidationResult r = validate(cfg);
    if (r.ok && controllers_.count(cfg.intersection_id)) {
        r.ok = false;
        r.errors.push_back("intersection '" + cfg.intersection_id + "' is already configured");
    }
    if (!r.ok) {
        log_invalid("configure " + cfg.intersection_id, r.errors);
        return r;
    }
    controllers_[cfg.intersection_id] = std::make_unique<SignalController>(cfg);
    return r;
}

ValidationResult SignalManager::configure_from_stream(std::istream& in, const std::string& source) {
    ValidationResult r;
    ParseResult p = parse_signal_config(in);
    r.errors = p.errors;
    std::set<std::string> batch_ids;
    for (const auto& c : p.configs) {
        auto v = validate(c);
        r.errors.insert(r.errors.end(), v.errors.begin(), v.errors.end());
        if (v.ok && (controllers_.count(c.intersection_id) || !batch_ids.insert(c.intersection_id).second))
            r.errors.push_back("intersection '" + c.intersection_id + "' is already configured");
    }
    if (p.configs.empty() && p.errors.empty())
        r.errors.push_back("no intersections defined in " + source);
    r.ok = r.errors.empty();
    if (!r.ok) {
        log_invalid(source, r.errors);
        return r;
    }
    for (const auto& c : p.configs) controllers_[c.intersection_id] = std::make_unique<SignalController>(c);
    return r;
}

void SignalManager::clear() { controllers_.clear(); }

bool SignalManager::has(const std::string& id) const { return controllers_.count(id) > 0; }

SignalController* SignalManager::get(const std::string& id) {
    auto it = controllers_.find(id);
    return it == controllers_.end() ? nullptr : it->second.get();
}
const SignalController* SignalManager::get(const std::string& id) const {
    auto it = controllers_.find(id);
    return it == controllers_.end() ? nullptr : it->second.get();
}

void SignalManager::tick(int seconds) {
    for (auto& kv : controllers_) kv.second->tick(seconds);
}

void SignalManager::set_strategy_all(Strategy s) {
    for (auto& kv : controllers_) kv.second->set_strategy(s);
}

SignalState SignalManager::state(const std::string& i, const std::string& road) const {
    const auto* c = get(i);
    return c ? c->state(road) : SignalState::RED;
}

bool SignalManager::can_proceed(const std::string& i, const std::string& road) const {
    return state(i, road) == SignalState::GREEN;
}

bool SignalManager::emergency_arrived(const std::string& i, const std::string& road) {
    auto* c = get(i);
    return c && c->request_emergency(road);
}
bool SignalManager::emergency_passed(const std::string& i, const std::string& road) {
    auto* c = get(i);
    return c && c->clear_emergency(road);
}
bool SignalManager::update_queue(const std::string& i, const std::string& road, int q) {
    auto* c = get(i);
    return c && c->set_queue_length(road, q);
}

}  // namespace tms::signals
