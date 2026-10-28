// Traffic signal module - Team 11 Traffic Management System (Simulation)
// Covers: FR-002 (signal config), FR-005 (fixed-time), FR-006 (adaptive),
//         FR-007 (emergency priority), SEC-001/002/004 (validation + logging).
#pragma once

#include <deque>
#include <functional>
#include <iosfwd>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace tms::signals {

enum class SignalState { RED, GREEN, YELLOW };       // SRS Appendix B: signal_state
enum class Strategy { FIXED_TIME, ADAPTIVE };
enum class Stage { GREEN, YELLOW, ALL_RED };         // internal phase machine

const char* to_string(SignalState s);
const char* to_string(Strategy s);
const char* to_string(Stage s);

// Safe ranges (seconds). Anything outside is rejected (SEC-002).
struct Limits {
    static constexpr int kMinGreen = 5,   kMaxGreen = 120;
    static constexpr int kMinYellow = 2,  kMaxYellow = 10;
    static constexpr int kMinAllRed = 1,  kMaxAllRed = 10;
    static constexpr int kMaxIdLen = 10;      // SRS Appendix B: ids are 10 chars
    static constexpr int kMaxPhases = 8;
    static constexpr int kMaxThreshold = 1000;
};

// One phase = a set of approach roads that get GREEN together.
struct PhaseConfig {
    std::string name;                 // e.g. "NS"
    std::vector<std::string> roads;   // road ids served by this phase
    int green_s = 20;                 // fixed-time green duration
    int yellow_s = 3;
};

struct AdaptiveParams {
    int min_green_s = 8;
    int max_green_s = 40;
    int extension_threshold = 1;      // keep green while phase queue >= this
};

struct IntersectionSignalConfig {
    std::string intersection_id;
    Strategy strategy = Strategy::FIXED_TIME;
    std::vector<PhaseConfig> phases;
    int all_red_s = 1;                // clearance between phases
    AdaptiveParams adaptive;
};

struct ValidationResult {
    bool ok = true;
    std::vector<std::string> errors;
};

// Pure validation: no side effects.
ValidationResult validate(const IntersectionSignalConfig& cfg);

// ---------------------------------------------------------------------------
// One controller per intersection. Time advances in whole seconds via tick().
// Invariants (checked by invariants_hold()):
//   * only roads of ONE phase are ever GREEN/YELLOW at a time
//   * every GREEN is followed by YELLOW, then ALL_RED, before another GREEN
// ---------------------------------------------------------------------------
class SignalController {
public:
    explicit SignalController(IntersectionSignalConfig cfg);  // throws std::invalid_argument

    void tick(int seconds = 1);

    // Adaptive input: only data measured in the CURRENT run (business rule).
    bool set_queue_length(const std::string& road, int queue);   // false if unknown/negative
    void set_queue_lengths(const std::map<std::string, int>& q); // ignores bad entries

    // Emergency priority (FR-007). Counted per road, served in arrival order.
    bool request_emergency(const std::string& road);
    bool clear_emergency(const std::string& road);
    bool emergency_active() const { return !emergency_order_.empty(); }

    void set_strategy(Strategy s) { cfg_.strategy = s; }
    Strategy strategy() const { return cfg_.strategy; }

    SignalState state(const std::string& road) const;   // unknown road -> RED (fail safe)
    std::map<std::string, SignalState> snapshot() const;
    bool invariants_hold() const;

    const std::string& id() const { return cfg_.intersection_id; }
    const IntersectionSignalConfig& config() const { return cfg_; }
    std::size_t current_phase() const { return current_; }
    Stage stage() const { return stage_; }
    int elapsed_in_stage() const { return elapsed_; }
    long long time() const { return time_; }
    long phase_changes() const { return phase_changes_; }

private:
    void tick_one();
    void on_green();
    void enter_yellow();
    void enter_all_red();
    void enter_green(std::size_t phase);
    std::size_t choose_next() const;
    int phase_queue(std::size_t phase) const;
    std::optional<std::size_t> emergency_target() const;

    IntersectionSignalConfig cfg_;
    std::map<std::string, std::size_t> road_phase_;
    std::map<std::string, int> queues_;
    std::deque<std::string> emergency_order_;
    std::map<std::string, int> emergency_count_;
    Stage stage_ = Stage::GREEN;
    std::size_t current_ = 0, next_ = 0;
    int elapsed_ = 0;
    long long time_ = 0;
    long phase_changes_ = 0;
};

// ---------------------------------------------------------------------------
// Config-file parsing (SEC-001). Format: see config/signals_example.cfg
// ---------------------------------------------------------------------------
struct ParseResult {
    std::vector<IntersectionSignalConfig> configs;
    std::vector<std::string> errors;      // "line N: message"
    bool ok() const { return errors.empty(); }
};
ParseResult parse_signal_config(std::istream& in);

// ---------------------------------------------------------------------------
// Facade used by the simulation (Rohan's vehicle movement calls this).
// ---------------------------------------------------------------------------
class SignalManager {
public:
    using LogSink = std::function<void(const std::string&)>;

    ValidationResult configure(const IntersectionSignalConfig& cfg);
    // All-or-nothing: if anything is invalid nothing is applied.
    ValidationResult configure_from_stream(std::istream& in, const std::string& source = "config");
    void clear();

    bool has(const std::string& intersection) const;
    SignalController* get(const std::string& intersection);
    const SignalController* get(const std::string& intersection) const;

    void tick(int seconds = 1);
    void set_strategy_all(Strategy s);

    // Vehicle-movement hook: may this vehicle enter the intersection now?
    // Only GREEN allows entry; emergency priority works by serving that road's
    // GREEN (never by forcing an invalid state). Unknown ids -> false.
    bool can_proceed(const std::string& intersection, const std::string& road) const;
    SignalState state(const std::string& intersection, const std::string& road) const;

    bool emergency_arrived(const std::string& intersection, const std::string& road);
    bool emergency_passed(const std::string& intersection, const std::string& road);
    bool update_queue(const std::string& intersection, const std::string& road, int queue);

    // SEC-004: invalid attempts are logged here (and to the sink, if set).
    const std::vector<std::string>& invalid_attempt_log() const { return log_; }
    void set_log_sink(LogSink s) { sink_ = std::move(s); }

private:
    void log_invalid(const std::string& source, const std::vector<std::string>& errors);
    std::map<std::string, std::unique_ptr<SignalController>> controllers_;
    std::vector<std::string> log_;
    LogSink sink_;
};

}  // namespace tms::signals
