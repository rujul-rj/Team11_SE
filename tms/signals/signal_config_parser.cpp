// Line-based signal configuration parser (SEC-001: all input checked before use).
//
//   intersection <ID> [strategy fixed|adaptive] [all_red <s>]
//   adaptive     <ID> [min_green <s>] [max_green <s>] [threshold <n>]
//   phase        <ID> <NAME> [green <s>] [yellow <s>] roads <R1> [R2 ...]
//   # comment
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <istream>
#include <map>
#include <sstream>

#include "tms/signals/signal.hpp"

namespace tms::signals {
namespace {

bool parse_int(const std::string& s, int& out) {
    if (s.empty()) return false;
    char* end = nullptr;
    errno = 0;
    const long v = std::strtol(s.c_str(), &end, 10);
    if (errno != 0 || *end != '\0' || v < INT_MIN || v > INT_MAX) return false;
    out = static_cast<int>(v);
    return true;
}

}  // namespace

ParseResult parse_signal_config(std::istream& in) {
    ParseResult res;
    std::map<std::string, std::size_t> index;
    std::string line;
    int lineno = 0;

    auto err = [&](const std::string& m) { res.errors.push_back("line " + std::to_string(lineno) + ": " + m); };
    auto find_cfg = [&](const std::string& id) -> IntersectionSignalConfig* {
        auto it = index.find(id);
        if (it == index.end()) { err("intersection '" + id + "' must be declared before use"); return nullptr; }
        return &res.configs[it->second];
    };

    while (std::getline(in, line)) {
        ++lineno;
        const auto hash = line.find('#');
        if (hash != std::string::npos) line.erase(hash);
        std::istringstream ss(line);
        std::vector<std::string> t;
        for (std::string w; ss >> w;) t.push_back(w);
        if (t.empty()) continue;

        const std::string& cmd = t[0];
        if (cmd == "intersection") {
            if (t.size() < 2) { err("intersection needs an id"); continue; }
            if (index.count(t[1])) { err("intersection '" + t[1] + "' declared twice"); continue; }
            IntersectionSignalConfig c;
            c.intersection_id = t[1];
            for (std::size_t i = 2; i < t.size(); i += 2) {
                if (i + 1 >= t.size()) { err("missing value for '" + t[i] + "'"); break; }
                if (t[i] == "strategy") {
                    if (t[i + 1] == "fixed" || t[i + 1] == "fixed_time") c.strategy = Strategy::FIXED_TIME;
                    else if (t[i + 1] == "adaptive") c.strategy = Strategy::ADAPTIVE;
                    else err("unknown strategy '" + t[i + 1] + "' (use fixed or adaptive)");
                } else if (t[i] == "all_red") {
                    if (!parse_int(t[i + 1], c.all_red_s)) err("all_red is not a valid integer: '" + t[i + 1] + "'");
                } else {
                    err("unknown key '" + t[i] + "'");
                }
            }
            index[c.intersection_id] = res.configs.size();
            res.configs.push_back(std::move(c));
        } else if (cmd == "adaptive") {
            if (t.size() < 2) { err("adaptive needs an intersection id"); continue; }
            auto* c = find_cfg(t[1]);
            if (!c) continue;
            for (std::size_t i = 2; i < t.size(); i += 2) {
                if (i + 1 >= t.size()) { err("missing value for '" + t[i] + "'"); break; }
                int* target = t[i] == "min_green" ? &c->adaptive.min_green_s
                            : t[i] == "max_green" ? &c->adaptive.max_green_s
                            : t[i] == "threshold" ? &c->adaptive.extension_threshold : nullptr;
                if (!target) { err("unknown key '" + t[i] + "'"); continue; }
                if (!parse_int(t[i + 1], *target)) err(t[i] + " is not a valid integer: '" + t[i + 1] + "'");
            }
        } else if (cmd == "phase") {
            if (t.size() < 3) { err("phase needs an intersection id and a phase name"); continue; }
            auto* c = find_cfg(t[1]);
            if (!c) continue;
            PhaseConfig p;
            p.name = t[2];
            bool have_roads = false;
            for (std::size_t i = 3; i < t.size();) {
                if (t[i] == "roads") {
                    have_roads = true;
                    for (++i; i < t.size(); ++i) p.roads.push_back(t[i]);
                    break;
                }
                if (i + 1 >= t.size()) { err("missing value for '" + t[i] + "'"); break; }
                int* target = t[i] == "green" ? &p.green_s : t[i] == "yellow" ? &p.yellow_s : nullptr;
                if (!target) err("unknown key '" + t[i] + "'");
                else if (!parse_int(t[i + 1], *target)) err(t[i] + " is not a valid integer: '" + t[i + 1] + "'");
                i += 2;
            }
            if (!have_roads) err("phase '" + p.name + "' has no 'roads' list");
            c->phases.push_back(std::move(p));
        } else {
            err("unknown directive '" + cmd + "'");
        }
    }
    return res;
}

}  // namespace tms::signals
