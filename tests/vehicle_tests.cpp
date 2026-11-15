#include "Vehicle.h"
#include <iostream>
#include <stdexcept>

int main() {
    try {
        Vehicle vehicle("V1", {"A", "B", "C"});

        if (vehicle.currentIntersection() != "A")
            throw std::runtime_error("Initial position incorrect");

        vehicle.moveToNextIntersection();

        if (vehicle.currentIntersection() != "B")
            throw std::runtime_error("Vehicle movement failed");

        vehicle.waitOneStep(2.0);

        if (vehicle.waitingTimeSeconds() != 2.0)
            throw std::runtime_error("Waiting time incorrect");

        vehicle.moveToNextIntersection();

        if (!vehicle.hasArrived())
            throw std::runtime_error("Arrival detection failed");

        std::cout << "Vehicle component tests passed.\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Vehicle test failed: " << e.what() << '\n';
        return 1;
    }
}
