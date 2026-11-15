#include "Intersection.h"
#include <iostream>
#include <stdexcept>

int main() {
    try {
        Intersection intersection("A");

        if (intersection.id() != "A")
            throw std::runtime_error("Intersection ID incorrect");

        intersection.setSignalState(SignalState::Red);

        if (intersection.signalState() != SignalState::Red)
            throw std::runtime_error("Red signal state failed");

        intersection.setSignalState(SignalState::Green);

        if (intersection.signalState() != SignalState::Green)
            throw std::runtime_error("Green signal state failed");

        std::cout << "Intersection component tests passed.\n";
        return 0;
    }
    catch (const std::exception& e) {
        std::cerr << "Intersection test failed: " << e.what() << '\n';
        return 1;
    }
}
