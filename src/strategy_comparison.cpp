#include "Simulation.h"
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
TrafficNetwork makeNetwork() {
    TrafficNetwork network;
    for (const char* id : {"A", "B", "C", "D"}) network.addIntersection(Intersection(id));
    network.addRoad(Road("AB", "A", "B", 1));
    network.addRoad(Road("AC", "A", "C", 1));
    network.addRoad(Road("BD", "B", "D", 1));
    network.addRoad(Road("CD", "C", "D", 1));
    return network;
}

tms::signals::IntersectionSignalConfig signalConfig(tms::signals::Strategy strategy) {
    using namespace tms::signals;
    IntersectionSignalConfig config;
    config.intersection_id = "A";
    config.strategy = strategy;
    config.all_red_s = 1;
    config.adaptive.min_green_s = 5;
    config.adaptive.max_green_s = 8;
    config.adaptive.extension_threshold = 1;
    config.phases = {{"AB-phase", {"AB"}, 5, 2}, {"AC-phase", {"AC"}, 5, 2}};
    return config;
}

Simulation runScenario(tms::signals::Strategy strategy) {
    Simulation simulation(makeNetwork(), 1.0);
    const auto configured = simulation.configureSignal(signalConfig(strategy));
    if (!configured.ok) {
        std::string message = "Unable to configure comparison scenario";
        if (!configured.errors.empty()) message += ": " + configured.errors.front();
        throw std::runtime_error(message);
    }
    // Same deterministic demand for both strategies.
    for (int i = 0; i < 10; ++i) {
        const std::string id = "V" + std::to_string(i + 1);
        simulation.addVehicle(Vehicle(id, i % 2 == 0
            ? std::vector<std::string>{"A", "B", "D"}
            : std::vector<std::string>{"A", "C", "D"}));
    }
    simulation.run(30);
    return simulation;
}
}

int main() {
    try {
        const auto fixed = runScenario(tms::signals::Strategy::FIXED_TIME);
        const auto adaptive = runScenario(tms::signals::Strategy::ADAPTIVE);
        const auto a = fixed.summary();
        const auto b = adaptive.summary();

        std::cout << "Strategy comparison (same network, 10 vehicles, 30 one-second steps)\n";
        std::cout << std::left << std::setw(30) << "Metric"
                  << std::right << std::setw(16) << "Fixed-time"
                  << std::setw(16) << "Adaptive" << '\n';
        std::cout << std::left << std::setw(30) << "Vehicles arrived"
                  << std::right << std::setw(16) << a.totalArrived
                  << std::setw(16) << b.totalArrived << '\n';
        std::cout << std::left << std::setw(30) << "Active at end"
                  << std::right << std::setw(16) << a.activeVehicles
                  << std::setw(16) << b.activeVehicles << '\n';
        std::cout << std::fixed << std::setprecision(3);
        std::cout << std::left << std::setw(30) << "Total wait (vehicle-seconds)"
                  << std::right << std::setw(16) << a.totalWaitingTimeSeconds
                  << std::setw(16) << b.totalWaitingTimeSeconds << '\n';
        std::cout << std::left << std::setw(30) << "Average queue (vehicles/step)"
                  << std::right << std::setw(16) << a.averageQueueLength
                  << std::setw(16) << b.averageQueueLength << '\n';
        std::cout << std::left << std::setw(30) << "Throughput (vehicles/second)"
                  << std::right << std::setw(16) << a.throughputVehiclesPerSecond
                  << std::setw(16) << b.throughputVehiclesPerSecond << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Strategy comparison error: " << error.what() << '\n';
        return 1;
    }
}
