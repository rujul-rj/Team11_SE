#include "Road.h"
#include <iostream>
#include <stdexcept>

int main() {
    try {
        Road road("R1", "A", "B", 2);

        if (road.lanes() != 2)
            throw std::runtime_error("Lane count incorrect");

        if (road.capacity() != 40)
            throw std::runtime_error("Capacity incorrect");

        road.setBlocked(true);
        if (!road.isBlocked())
            throw std::runtime_error("Blocking failed");

        road.setCapacityReductionFactor(0.5);
        if (road.effectiveCapacity() != 20)
            throw std::runtime_error("Effective capacity incorrect");

        std::cout << "Road component tests passed.\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Road test failed: " << e.what() << '\n';
        return 1;
    }
}
