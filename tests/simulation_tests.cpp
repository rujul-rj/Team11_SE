
#include "Simulation.h"
#include "tms/signals/signal.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace {

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
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

tms::signals::IntersectionSignalConfig cfg() {
    using namespace tms::signals;

    IntersectionSignalConfig config;
    config.intersection_id = "A";
    config.phases = {
        {"NS", {"AB"}, 5, 2},
        {"EW", {"AX"}, 7, 2}
    };
    config.all_red_s = 1;
    config.adaptive.min_green_s = 5;
    config.adaptive.max_green_s = 8;
    config.adaptive.extension_threshold = 1;

    return config;
}

// Core network and vehicle tests.

void testRoutes() {
    auto network = makeNetwork();

    require(network.isValidRoute({"A", "B", "C"}),
            "Connected route should be valid");
    require(!network.isValidRoute({"A", "C"}),
            "Missing road should make route invalid");
    require(!network.isValidRoute({"A", "Z"}),
            "Unknown intersection should make route invalid");
}

void testMovement() {
    Simulation simulation(makeNetwork());
    simulation.addVehicle(Vehicle("V1", {"A", "B", "C"}));

    simulation.step();
    require(simulation.totalArrived() == 0,
            "Vehicle should traverse one road per step");

    simulation.step();
    require(simulation.totalArrived() == 1,
            "Vehicle should arrive after the second road");
}

void testRedAndBlockedQueues() {
    Simulation simulation(makeNetwork());
    simulation.addVehicle(Vehicle("V1", {"A", "B", "C"}));
    simulation.setSignal("A", SignalState::Red);
    simulation.step();

    require(simulation.lastStepStats().queuedVehicles == 1,
            "Red signal should queue vehicle");
    require(simulation.vehicles().front().waitingTimeSeconds() == 1.0,
            "Waiting time should increase");

    Simulation blockedSimulation(makeNetwork());
    blockedSimulation.addVehicle(Vehicle("V2", {"A", "B"}));
    blockedSimulation.setRoadBlocked("A", "B", true);
    blockedSimulation.step();

    require(blockedSimulation.lastStepStats().queuedVehicles == 1,
            "Blocked road should queue vehicle");
}

void testInvalidRoute() {
    Simulation simulation(makeNetwork());
    bool threw = false;

    try {
        simulation.addVehicle(Vehicle("V1", {"A", "C"}));
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    require(threw, "Invalid vehicle route should be rejected");
}

// Automatic vehicle generation tests.

void testAutomaticGenerationInterval() {
    Simulation simulation(makeNetwork());

    simulation.configureVehicleGeneration({{"A", "B", "C"}}, 2, 3);

    require(simulation.vehicles().empty(),
            "No vehicles should exist before the first step");

    simulation.step();
    require(simulation.vehicles().empty(),
            "Vehicle should not generate before the interval");

    simulation.step();
    require(simulation.vehicles().size() == 1,
            "One vehicle should generate after two steps");

    require(simulation.vehicles().front().id() == "AUTO-1",
            "Generated vehicle should have an automatic ID");
}

void testAutomaticGenerationMaximum() {
    Simulation simulation(makeNetwork());

    simulation.configureVehicleGeneration({{"A", "B", "C"}}, 1, 2);
    simulation.run(5);

    require(simulation.vehicles().size() == 2,
            "Generator should respect its maximum of two vehicles");
}

void testAutomaticGenerationRejectsInvalidRoute() {
    Simulation simulation(makeNetwork());
    bool threw = false;

    try {
        simulation.configureVehicleGeneration({{"A", "C"}}, 1, 3);
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    require(threw, "Generator should reject an invalid route");
}

void testAutomaticGenerationRejectsZeroInterval() {
    Simulation simulation(makeNetwork());
    bool threw = false;

    try {
        simulation.configureVehicleGeneration({{"A", "B"}}, 0, 3);
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    require(threw, "Generator should reject a zero interval");
}

// Teammate's signal-controller tests.

void testConfigValidationAndParser() {
    using namespace tms::signals;

    auto config = cfg();
    require(validate(config).ok, "Valid signal config should be accepted");

    config.phases[0].green_s = 1;
    require(!validate(config).ok, "Invalid green duration should be rejected");

    std::istringstream input(
        "intersection A strategy fixed all_red 1\n"
        "phase A NS green 5 yellow 2 roads AB\n"
        "phase A EW green 7 yellow 2 roads AX\n"
    );
    require(parse_signal_config(input).ok(),
            "Valid signal config should parse");

    std::istringstream badInput("intersection A strategy unknown\n");
    require(!parse_signal_config(badInput).ok(),
            "Unknown strategy should be rejected");
}

void testFixedTime() {
    using namespace tms::signals;

    SignalController controller(cfg());

    require(controller.state("AB") == SignalState::GREEN,
            "Initial phase should be green");

    controller.tick(5);
    require(controller.stage() == Stage::YELLOW,
            "Green should end on schedule");

    require(controller.invariants_hold(),
            "Yellow transition should preserve invariants");

    controller.tick(2);
    require(controller.stage() == Stage::ALL_RED,
            "Yellow should be followed by all-red");

    require(controller.state("AB") == SignalState::RED,
            "All-red should stop the approach");

    controller.tick(1);
    require(controller.state("AX") == SignalState::GREEN,
            "Next phase should start after clearance");

    require(controller.invariants_hold(),
            "Phase transition should preserve invariants");
}

void testAdaptive() {
    using namespace tms::signals;

    auto config = cfg();
    config.strategy = Strategy::ADAPTIVE;

    SignalController controller(config);

    require(controller.set_queue_length("AB", 4),
            "Queue length should be accepted");

    controller.tick(5);
    require(controller.stage() == Stage::GREEN,
            "High queue should extend green");

    require(controller.set_queue_length("AB", 0),
            "Queue should be allowed to clear");

    controller.tick(1);
    require(controller.stage() == Stage::YELLOW,
            "Green should end when the extension condition is no longer met");
}

void testEmergency() {
    using namespace tms::signals;

    SignalController controller(cfg());

    require(controller.request_emergency("AX"),
            "Emergency request should be accepted");

    controller.tick(1);
    require(controller.stage() == Stage::YELLOW,
            "Emergency transition should use yellow");

    require(controller.invariants_hold(),
            "Yellow transition should preserve invariants");

    controller.tick(2);
    require(controller.stage() == Stage::ALL_RED,
            "Emergency transition should use all-red");

    controller.tick(1);
    require(controller.state("AX") == SignalState::GREEN,
            "Emergency approach should receive green");

    require(controller.invariants_hold(),
            "Emergency green should preserve invariants");

    require(controller.clear_emergency("AX"),
            "Emergency should be clearable");
}

void testIntegrationAndInvalidLog() {
    using namespace tms::signals;

    auto network = makeNetwork();
    network.addRoad(Road("AX", "A", "C", 1));

    Simulation simulation(network);

    require(simulation.configureSignal(cfg()).ok,
            "Signal should install for valid outgoing roads");

    simulation.addVehicle(Vehicle("V1", {"A", "B"}));
    simulation.step();

    require(simulation.totalArrived() == 1,
            "Green controller should permit movement");

    SignalManager manager;
    auto invalidConfig = cfg();
    invalidConfig.phases[0].yellow_s = 0;

    require(!manager.configure(invalidConfig).ok,
            "Invalid signal config should be rejected");

    require(!manager.has("A"),
            "Invalid config should not be applied");

    require(!manager.invalid_attempt_log().empty(),
            "Invalid configuration attempt should be logged");
}

} // namespace

int main() {
    try {
        testRoutes();
        testMovement();
        testRedAndBlockedQueues();
        testInvalidRoute();

        testAutomaticGenerationInterval();
        testAutomaticGenerationMaximum();
        testAutomaticGenerationRejectsInvalidRoute();
        testAutomaticGenerationRejectsZeroInterval();

        testConfigValidationAndParser();
        testFixedTime();
        testAdaptive();
        testEmergency();
        testIntegrationAndInvalidLog();

        std::cout << "All simulation, generation, and signal tests passed.\n";
        return EXIT_SUCCESS;
    } catch (const std::exception& ex) {
        std::cerr << "TEST FAILURE: " << ex.what() << '\n';
        return EXIT_FAILURE;
    }
}
