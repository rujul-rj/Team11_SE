
#include "Simulation.h"

#include <exception>
#include <iostream>

int main() {
    try {
        TrafficNetwork network;

        network.addIntersection(Intersection("A"));
        network.addIntersection(Intersection("B"));
        network.addIntersection(Intersection("C"));

        network.addRoad(Road("R1", "A", "B", 2));
        network.addRoad(Road("R2", "B", "C", 1));

        Simulation simulation(network, 1.0);

        // Add two initial vehicles manually.
        simulation.addVehicle(Vehicle("V1", {"A", "B", "C"}));
        simulation.addVehicle(Vehicle("V2", {"A", "B", "C"}));

        // Automatically generate up to 5 more vehicles,
        // one every 2 simulation steps.
        simulation.configureVehicleGeneration(
            {{"A", "B", "C"}},
            2,
            5
        );

        // Demonstrate vehicles waiting at a red signal at B.
        simulation.setSignal("B", SignalState::Red);

        std::cout << "Traffic Management System Simulation\n";
        std::cout << "Scenario: A -> B -> C\n";
        std::cout << "Initial vehicles: 2\n";
        std::cout << "Automatic generation: every 2 steps, max 5\n\n";

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
        return 0;

    } catch (const std::exception& ex) {
        std::cerr << "Simulation error: " << ex.what() << '\n';
        return 1;
    }
}
