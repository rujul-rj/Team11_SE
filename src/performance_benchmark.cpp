#include "Simulation.h"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <stdexcept>
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
            if (from != to) network.addRoad(Road("R" + std::to_string(roadNumber++), from, to, 3));
        }
    }
    return network;
}

std::vector<std::string> makeLongRoute(const std::vector<std::string>& cycle) {
    std::vector<std::string> route;
    route.reserve(21);
    for (int i = 0; i <= 20; ++i) route.push_back(cycle[static_cast<std::size_t>(i) % cycle.size()]);
    return route;
}

void configureSignals(Simulation& simulation, tms::signals::Strategy strategy) {
    using namespace tms::signals;
    IntersectionSignalConfig config;
    config.intersection_id = "A";
    config.strategy = strategy;
    config.phases = {
        {"AB", {"R1"}, 5, 2},
        {"AC", {"R2"}, 5, 2}
    };
    config.all_red_s = 1;
    config.adaptive.min_green_s = 5;
    config.adaptive.max_green_s = 10;
    config.adaptive.extension_threshold = 1;
    const auto result = simulation.configureSignal(config);
    if (!result.ok) throw std::runtime_error("Benchmark signal configuration failed.");
}

bool runScenario(const std::string& name, bool adaptive, bool incidentAndEmergency) {
    constexpr std::size_t kVehicles = 100;
    constexpr std::size_t kSteps = 10;
    constexpr std::size_t kRuns = 10;
    constexpr double kLimitMilliseconds = 2000.0;

    const auto network = makeBenchmarkNetwork();
    const auto normalRoute = makeLongRoute({"A", "B", "C", "D"});
    const auto emergencyRoute = makeLongRoute({"A", "C", "B", "D"});
    std::vector<double> timings;
    timings.reserve(kRuns);

    for (std::size_t run = 1; run <= kRuns; ++run) {
        Simulation simulation(network, 1.0);
        if (adaptive || incidentAndEmergency) {
            configureSignals(simulation, tms::signals::Strategy::ADAPTIVE);
        }
        if (incidentAndEmergency) simulation.setRoadBlocked("A", "B", true);

        for (std::size_t vehicle = 0; vehicle < kVehicles; ++vehicle) {
            const bool emergency = incidentAndEmergency && vehicle >= 90;
            const auto& route = emergency ? emergencyRoute : normalRoute;
            const auto type = emergency ? VehicleType::Emergency : VehicleType::Normal;
            simulation.addVehicle(Vehicle("B" + std::to_string(vehicle + 1), route, type));
        }

        const auto start = std::chrono::steady_clock::now();
        simulation.run(kSteps);
        const auto stop = std::chrono::steady_clock::now();
        const double elapsed = std::chrono::duration<double, std::milli>(stop - start).count();

        const auto summary = simulation.summary();
        if (summary.stepsExecuted != kSteps || summary.vehiclesAdded != kVehicles ||
            summary.activeVehicles != kVehicles) {
            std::cerr << name << ": benchmark workload did not complete 1,000 vehicle-step updates.\n";
            return false;
        }

        timings.push_back(elapsed);
        std::cout << name << " run " << run << ": " << std::fixed << std::setprecision(3)
                  << elapsed << " ms for " << (kVehicles * kSteps)
                  << " vehicle-step updates\n";
        if (elapsed > kLimitMilliseconds) {
            std::cerr << name << ": NFR-001 threshold exceeded: " << elapsed
                      << " ms > " << kLimitMilliseconds << " ms\n";
            return false;
        }
    }

    const auto [minimum, maximum] = std::minmax_element(timings.begin(), timings.end());
    double total = 0.0;
    for (double timing : timings) total += timing;
    std::cout << name << " summary: min=" << *minimum << " ms, average="
              << (total / timings.size()) << " ms, max=" << *maximum
              << " ms; target <= " << kLimitMilliseconds << " ms per run.\n";
    return true;
}
}

int main() {
    try {
        const bool baseline = runScenario("TC-013.1 baseline", false, false);
        const bool adaptive = runScenario("TC-013.2 adaptive", true, false);
        const bool incidentEmergency = runScenario("TC-013.3 incidents+emergency", true, true);
        return baseline && adaptive && incidentEmergency ? 0 : 1;
    } catch (const std::exception& error) {
        std::cerr << "Performance benchmark failed: " << error.what() << '\\n';
        return 2;
    }
}
