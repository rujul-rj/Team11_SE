#include "Simulation.h"
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace {
void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
TrafficNetwork makeNetwork() {
    TrafficNetwork network;
    network.addIntersection(Intersection("A"));
    network.addIntersection(Intersection("B"));
    network.addIntersection(Intersection("C"));
    network.addRoad(Road("AB", "A", "B", 2));
    network.addRoad(Road("BC", "B", "C", 1));
    return network;
}
void testValidRoute() {
    auto network = makeNetwork();
    require(network.isValidRoute({"A", "B", "C"}), "Connected route should be valid");
    require(!network.isValidRoute({"A", "C"}), "Missing road should make route invalid");
    require(!network.isValidRoute({"A", "Z"}), "Unknown intersection should make route invalid");
}
void testMovementAndArrival() {
    Simulation sim(makeNetwork());
    sim.addVehicle(Vehicle("V1", {"A", "B", "C"}));
    sim.step();
    require(sim.totalArrived() == 0, "Vehicle should not arrive after first road");
    sim.step();
    require(sim.totalArrived() == 1, "Vehicle should arrive after second road");
}
void testRedSignalQueuesVehicle() {
    Simulation sim(makeNetwork());
    sim.addVehicle(Vehicle("V1", {"A", "B", "C"}));
    sim.setSignal("A", SignalState::Red);
    sim.step();
    require(sim.lastStepStats().queuedVehicles == 1, "Red signal should queue vehicle");
    require(sim.vehicles().front().waitingTimeSeconds() == 1.0, "Waiting time should increase");
}
void testBlockedRoadQueuesVehicle() {
    Simulation sim(makeNetwork());
    sim.addVehicle(Vehicle("V1", {"A", "B"}));
    sim.setRoadBlocked("A", "B", true);
    sim.step();
    require(sim.lastStepStats().queuedVehicles == 1, "Blocked road should queue vehicle");
}
void testRejectInvalidVehicleRoute() {
    Simulation sim(makeNetwork());
    bool threw = false;
    try { sim.addVehicle(Vehicle("V1", {"A", "C"})); }
    catch (const std::invalid_argument&) { threw = true; }
    require(threw, "Invalid route should be rejected");
}
}

int main() {
    try {
        testValidRoute();
        testMovementAndArrival();
        testRedSignalQueuesVehicle();
        testBlockedRoadQueuesVehicle();
        testRejectInvalidVehicleRoute();
        std::cout << "All simulation tests passed.\n";
        return EXIT_SUCCESS;
    } catch (const std::exception& ex) {
        std::cerr << "TEST FAILURE: " << ex.what() << '\n';
        return EXIT_FAILURE;
    }
}
