
#include "Simulation.h"

#include <exception>
#include <iomanip>
#include <iostream>

int main() {
    try {
        TrafficNetwork network;

        network.addIntersection(Intersection("A"));
        network.addIntersection(Intersection("B"));
        network.addIntersection(Intersection("C"));

        network.addRoad(Road("R1", "A", "B", 2));
        network.addRoad(Road("R2", "B", "C", 1));
        network.addRoad(Road("R3", "A", "C", 1));

        Simulation simulation(network, 1.0);

        tms::signals::IntersectionSignalConfig signalConfig;
        signalConfig.intersection_id = "A";
        signalConfig.phases = {
            {"AB", {"R1"}, 5, 2},
            {"AC", {"R3"}, 5, 2}
        };
        signalConfig.all_red_s = 1;
        const auto signalResult = simulation.configureSignal(signalConfig);
        if (!signalResult.ok) {
            throw std::runtime_error("Signal configuration failed.");
        }

        // Add two initial vehicles manually.
        simulation.addVehicle(Vehicle("V1", {"A", "B", "C"}));
        simulation.addVehicle(Vehicle("V2", {"A", "B", "C"}));
        simulation.addVehicle(Vehicle("EV1", {"A", "C"}, VehicleType::Emergency));

        // Automatically generate up to 5 more vehicles,
        // one every 2 simulation steps.
        simulation.configureVehicleGeneration(
            {{"A", "B", "C"}},
            TrafficDensity::Medium,
            5
        );

        // Demonstrate vehicles waiting at a red signal at B.
        simulation.setSignal("B", SignalState::Red);

        std::cout << "Traffic Management System Simulation\n";
        std::cout << "Scenario: A -> B -> C\n";
        std::cout << "Initial vehicles: 2 normal + 1 emergency\n";
        std::cout << "Automatic generation: MEDIUM density, max 5\n\n";

        for (int tick = 1; tick <= 12; ++tick) {
            // Release vehicles waiting at B from step 4 onward.
            if (tick == 4) {
                simulation.setSignal("B", SignalState::Green);
            }

            simulation.step();

            const auto& stats = simulation.lastStepStats();

            std::cout << "Step " << tick
                      << " | vehicles=" << simulation.vehicles().size()
                      << " | active=" << stats.activeVehicles
                      << " | queued=" << stats.queuedVehicles
                      << " | arrived this step=" << stats.throughput
                      << " | total arrived=" << simulation.totalArrived()
                      << " | accumulated wait(s)="
                      << stats.totalWaitingTimeSeconds
                      << '\n';
        }

        std::cout << "\nSimulation completed.\n";
        const auto summary = simulation.summary();
        std::cout << "\nEnd-of-run summary\n"
                  << "Steps executed: " << summary.stepsExecuted << '\n'
                  << "Vehicles added: " << summary.vehiclesAdded << '\n'
                  << "Vehicles arrived: " << summary.totalArrived << '\n'
                  << "Active vehicles: " << summary.activeVehicles << '\n'
                  << std::fixed << std::setprecision(2)
                  << "Total waiting time (vehicle-seconds): " << summary.totalWaitingTimeSeconds << '\n'
                  << "Average waiting time (seconds/vehicle): " << summary.averageWaitingTimeSeconds << '\n'
                  << "Average queue length (vehicles/step): " << summary.averageQueueLength << '\n'
                  << "Throughput (vehicles/second): " << summary.throughputVehiclesPerSecond << '\n';
        return 0;

    } catch (const std::exception& ex) {
        std::cerr << "Simulation error: " << ex.what() << '\n';
        return 1;
    }
}
