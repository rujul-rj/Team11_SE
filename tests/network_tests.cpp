#include "TrafficNetwork.h"
#include <iostream>
#include <stdexcept>

int main() {
    try {
        TrafficNetwork network;

        network.addIntersection(Intersection("A"));
        network.addIntersection(Intersection("B"));
        network.addIntersection(Intersection("C"));

        network.addRoad(Road("R1", "A", "B", 2));
        network.addRoad(Road("R2", "B", "C", 1));

        if (!network.hasIntersection("A"))
            throw std::runtime_error("Intersection lookup failed");

        if (!network.hasRoad("A", "B"))
            throw std::runtime_error("Road lookup failed");

        if (!network.isValidRoute({"A", "B", "C"}))
            throw std::runtime_error("Valid route rejected");

        if (network.isValidRoute({"A", "C"}))
            throw std::runtime_error("Invalid route accepted");

        std::cout << "Network component tests passed.\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Network test failed: " << e.what() << '\n';
        return 1;
    }
}
