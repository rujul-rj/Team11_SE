#include "network_config_parser.h"

#include <climits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
bool parse_positive_int(const std::string& text, int& value) {
    try {
        std::size_t consumed = 0;
        const long parsed = std::stol(text, &consumed);
        if (consumed != text.size() || parsed <= 0 || parsed > INT_MAX) return false;
        value = static_cast<int>(parsed);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}
}

NetworkParseResult parse_network_config(std::istream& input) {
    NetworkParseResult result;
    std::string line;
    std::size_t lineNumber = 0;
    std::size_t intersectionCount = 0;
    std::size_t roadCount = 0;

    while (std::getline(input, line)) {
        ++lineNumber;
        std::istringstream row(line);
        std::vector<std::string> tokens;
        std::string token;
        while (row >> token) tokens.push_back(token);
        if (tokens.empty() || tokens.front().starts_with("#")) continue;

        try {
            if (tokens[0] == "intersection") {
                if (tokens.size() != 2) {
                    result.errors.push_back("line " + std::to_string(lineNumber) +
                                            ": expected 'intersection <id>'");
                    continue;
                }
                result.network.addIntersection(Intersection(tokens[1]));
                ++intersectionCount;
            } else if (tokens[0] == "road") {
                if (tokens.size() != 5 && tokens.size() != 6) {
                    result.errors.push_back("line " + std::to_string(lineNumber) +
                                            ": expected 'road <id> <from> <to> <lanes> [capacity]'");
                    continue;
                }
                int lanes = 0;
                if (!parse_positive_int(tokens[4], lanes)) {
                    result.errors.push_back("line " + std::to_string(lineNumber) +
                                            ": lanes must be a positive integer");
                    continue;
                }
                if (tokens.size() == 6) {
                    int capacity = 0;
                    if (!parse_positive_int(tokens[5], capacity)) {
                        result.errors.push_back("line " + std::to_string(lineNumber) +
                                                ": capacity must be a positive integer");
                        continue;
                    }
                    result.network.addRoad(Road(tokens[1], tokens[2], tokens[3], lanes, capacity));
                } else {
                    result.network.addRoad(Road(tokens[1], tokens[2], tokens[3], lanes));
                }
                ++roadCount;
            } else {
                result.errors.push_back("line " + std::to_string(lineNumber) +
                                        ": unknown directive '" + tokens[0] + "'");
            }
        } catch (const std::exception& error) {
            result.errors.push_back("line " + std::to_string(lineNumber) + ": " + error.what());
        }
    }

    if (intersectionCount == 0) result.errors.push_back("network configuration defines no intersections");
    if (roadCount == 0) result.errors.push_back("network configuration defines no roads");
    result.ok = result.errors.empty();
    return result;
}
