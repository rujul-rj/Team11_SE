#pragma once

#include "TrafficNetwork.h"

#include <istream>
#include <string>
#include <vector>

struct NetworkParseResult {
    TrafficNetwork network;
    std::vector<std::string> errors;
    bool ok{false};
};

NetworkParseResult parse_network_config(std::istream& input);
