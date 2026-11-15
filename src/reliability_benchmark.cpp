#include "Simulation.h"

#include <cmath>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

namespace {
TrafficNetwork makeNetwork() {
    TrafficNetwork network;
    const std::vector<std::string> ids{"A", "B", "C", "D"};
    for (const auto& id : ids) network.addIntersection(Intersection(id));
    int number = 1;
    for (const auto& from : ids) {
        for (const auto& to : ids) {
            if (from != to) network.addRoad(Road("R" + std::to_string(number++), from, to, 3));
        }
    }
    return network;
}

std::vector<std::vector<std::string>> routes() {
    return {
        {"A", "B", "C", "D", "A", "B", "C", "D", "A"},
        {"A", "C", "B", "D", "A", "C", "B", "D", "A"},
        {"A", "D", "C", "B", "A", "D", "C", "B", "A"}
    };
}
}

int main() {
    try {
    constexpr std::size_t kRuns = 100;
    constexpr std::size_t kSteps = 300; // 10x a 30-step normal run
    const auto network = makeNetwork();
    const auto routeSet = routes();

    for (unsigned int seed = 1; seed <= kRuns; ++seed) {
        Simulation simulation(network, 1.0);
        simulation.configureVehicleGeneration(routeSet, TrafficDensity::High, 100, seed);
        if (seed % 4 == 0) simulation.setRoadCapacityReduction("A", "B", 0.5);
        if (seed % 5 == 0) simulation.setRoadBlocked("B", "C", true);
        simulation.run(kSteps);

        const auto summary = simulation.summary();
        if (summary.stepsExecuted != kSteps || summary.vehiclesAdded != 100 ||
            !std::isfinite(summary.totalWaitingTimeSeconds) ||
            !std::isfinite(summary.averageQueueLength) ||
            !std::isfinite(summary.throughputVehiclesPerSecond)) {
            std::cerr << "Reliability failure on seed " << seed << '\n';
            return 1;
        }
    }

    Simulation allBlocked(network, 1.0);
    allBlocked.configureVehicleGeneration(routeSet, TrafficDensity::High, 100, 2026);
    for (const auto& from : {std::string("A"), std::string("B"), std::string("C"), std::string("D")}) {
        for (const auto& to : {std::string("A"), std::string("B"), std::string("C"), std::string("D")}) {
            if (from != to) allBlocked.setRoadBlocked(from, to, true);
        }
    }
    allBlocked.run(kSteps);
    const auto blockedSummary = allBlocked.summary();
    if (blockedSummary.stepsExecuted != kSteps || blockedSummary.vehiclesAdded != 100 ||
        blockedSummary.activeVehicles != 100) {
        std::cerr << "All-roads-blocked stress scenario failed.\n";
        return 2;
    }

    std::cout << "Reliability PASS: " << kRuns << "/" << kRuns
              << " seeded runs completed " << kSteps
              << " steps each; all-roads-blocked stress scenario completed.\n";
    return 0;
    } catch (const std::exception& error) {
        std::cerr << "Reliability benchmark failed: " << error.what() << '\n';
        return 3;
    }
}
