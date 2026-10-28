#include "Simulation.h"
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

        Simulation simulation(network, 1.0);
        simulation.addVehicle(Vehicle("V1", {"A", "B", "C"}));
        simulation.addVehicle(Vehicle("V2", {"A", "B", "C"}));
        simulation.setSignal("B", SignalState::Red);

        std::cout << "Traffic Management System Simulation\n";
        std::cout << "Scenario: A -> B -> C, two vehicles, 1-second steps\n";
        for (int tick = 1; tick <= 4; ++tick) {
            if (tick == 3) simulation.setSignal("B", SignalState::Green);
            simulation.step();
            const auto& stats = simulation.lastStepStats();
            std::cout << "Step " << tick
                      << " | active=" << stats.activeVehicles
                      << " | queued=" << stats.queuedVehicles
                      << " | arrived this step=" << stats.throughput
                      << " | total arrived=" << simulation.totalArrived()
                      << " | accumulated wait(s)=" << stats.totalWaitingTimeSeconds
                      << '\n';
        }

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
