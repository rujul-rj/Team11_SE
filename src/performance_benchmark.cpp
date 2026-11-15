#include "Simulation.h"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace {
TrafficNetwork makeBenchmarkNetwork() {
    TrafficNetwork network;
    const std::vector<std::string> ids{"A", "B", "C", "D"};
    for (const auto& id : ids) network.addIntersection(Intersection(id));
    int roadNumber = 1;
    for (const auto& from : ids) {
        for (const auto& to : ids) {
            if (from != to) {
                network.addRoad(Road("R" + std::to_string(roadNumber++), from, to, 3));
            }
        }
    }
    return network;
}

std::vector<std::string> makeLongRoute() {
    std::vector<std::string> route;
    const std::vector<std::string> cycle{"A", "B", "C", "D"};
    route.reserve(21);
    for (int i = 0; i <= 20; ++i) route.push_back(cycle[static_cast<std::size_t>(i) % cycle.size()]);
    return route;
}
}

int main() {
    constexpr std::size_t kVehicles = 100;
    constexpr std::size_t kSteps = 10;
    constexpr std::size_t kRuns = 10;
    constexpr double kLimitMilliseconds = 2000.0;

    const auto network = makeBenchmarkNetwork();
    const auto route = makeLongRoute();
    std::vector<double> timings;
    timings.reserve(kRuns);

    for (std::size_t run = 1; run <= kRuns; ++run) {
        Simulation simulation(network, 1.0);
        for (std::size_t vehicle = 0; vehicle < kVehicles; ++vehicle) {
            simulation.addVehicle(Vehicle("B" + std::to_string(vehicle + 1), route));
        }

        const auto start = std::chrono::steady_clock::now();
        simulation.run(kSteps);
        const auto stop = std::chrono::steady_clock::now();
        const double elapsed = std::chrono::duration<double, std::milli>(stop - start).count();

        const auto summary = simulation.summary();
        if (summary.stepsExecuted != kSteps || summary.vehiclesAdded != kVehicles ||
            summary.activeVehicles != kVehicles) {
            std::cerr << "Benchmark workload did not complete 1,000 vehicle-step updates.\n";
            return 2;
        }

        timings.push_back(elapsed);
        std::cout << "Run " << run << ": " << std::fixed << std::setprecision(3)
                  << elapsed << " ms for " << (kVehicles * kSteps)
                  << " vehicle-step updates\n";
        if (elapsed > kLimitMilliseconds) {
            std::cerr << "NFR-001 threshold exceeded: " << elapsed
                      << " ms > " << kLimitMilliseconds << " ms\n";
            return 1;
        }
    }

    const auto [minimum, maximum] = std::minmax_element(timings.begin(), timings.end());
    double total = 0.0;
    for (double timing : timings) total += timing;
    std::cout << "Summary: min=" << *minimum << " ms, average="
              << (total / timings.size()) << " ms, max=" << *maximum
              << " ms; target <= " << kLimitMilliseconds << " ms per run.\n";
    return 0;
}
