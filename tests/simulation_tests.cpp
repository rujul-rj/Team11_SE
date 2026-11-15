#include "Simulation.h"
#include "network_config_parser.h"
#include "tms/signals/signal.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <sstream>
#include <limits>
#include <cmath>
#include <stdexcept>

namespace {
void require(bool c, const char* m) { if (!c) throw std::runtime_error(m); }
TrafficNetwork makeNetwork() {
    TrafficNetwork n;
    n.addIntersection(Intersection("A")); n.addIntersection(Intersection("B")); n.addIntersection(Intersection("C"));
    n.addRoad(Road("AB", "A", "B", 2)); n.addRoad(Road("BC", "B", "C", 1));
    return n;
}
tms::signals::IntersectionSignalConfig cfg() {
    using namespace tms::signals;
    IntersectionSignalConfig c; c.intersection_id = "A";
    c.phases = {{"NS", {"AB"}, 5, 2}, {"EW", {"AX"}, 7, 2}};
    c.all_red_s = 1; c.adaptive.min_green_s = 5; c.adaptive.max_green_s = 8; c.adaptive.extension_threshold = 1;
    return c;
}
void testNetworkConfigParser() {
    std::istringstream valid(
        "# sample topology\n"
        "intersection A\n"
        "intersection B\n"
        "intersection C\n"
        "road R1 A B 2 40\n"
        "road R2 B C 1\n"
    );
    auto parsed = parse_network_config(valid);
    require(parsed.ok, "valid network configuration should parse");
    require(parsed.network.isValidRoute({"A", "B", "C"}),
            "parsed network should contain connected route");
    require(parsed.network.findRoad("A", "B")->capacity() == 40,
            "explicit road capacity should be retained");
    require(parsed.network.findRoad("B", "C")->capacity() == 20,
            "default road capacity should derive from lane count");

    std::istringstream invalid(
        "intersection A\n"
        "road R1 A Z 1\n"
        "road R2 A A 1\n"
        "road R3 A B 0\n"
        "unknown directive\n"
    );
    auto rejected = parse_network_config(invalid);
    require(!rejected.ok && rejected.errors.size() >= 4,
            "unknown endpoints, self roads, zero lanes and unknown directives should be rejected");

    std::istringstream empty;
    auto emptyResult = parse_network_config(empty);
    require(!emptyResult.ok && !emptyResult.errors.empty(),
            "empty network configuration should be rejected");
}
void testRoutesAndNetworkValidation() {
    auto n = makeNetwork();
    require(n.isValidRoute({"A", "B", "C"}), "connected route valid");
    require(!n.isValidRoute({}), "empty route invalid");
    require(!n.isValidRoute({"A", "C"}), "missing road invalid");
    require(!n.isValidRoute({"A", "Z"}), "unknown intersection invalid");
    bool threw = false;
    try { n.addIntersection(Intersection("A")); } catch (const std::invalid_argument&) { threw = true; }
    require(threw, "duplicate intersection rejected");
    threw = false;
    try { n.addRoad(Road("missing-endpoint", "A", "Z", 1)); } catch (const std::invalid_argument&) { threw = true; }
    require(threw, "road with unknown endpoint rejected");
    threw = false;
    try { n.addRoad(Road("duplicate-road", "A", "B", 1)); } catch (const std::invalid_argument&) { threw = true; }
    require(threw, "duplicate directed road rejected");
}
void testMovement() {
    Simulation s(makeNetwork()); s.addVehicle(Vehicle("V1", {"A", "B", "C"}));
    s.step(); require(s.totalArrived() == 0, "vehicle traverses one road per step");
    s.step(); require(s.totalArrived() == 1, "vehicle arrives after second road");
    require(s.stepsExecuted() == 2, "step counter increments");
}
void testRoadCapacityReduction() {
    Simulation simulation(makeNetwork());
    for (int i = 0; i < 25; ++i) {
        simulation.addVehicle(Vehicle("CAP-" + std::to_string(i + 1), {"A", "B"}));
    }
    simulation.setRoadCapacityReduction("A", "B", 0.5);
    require(simulation.network().findRoad("A", "B")->capacity() == 40,
            "Two-lane default road capacity should be 40");
    require(simulation.network().findRoad("A", "B")->effectiveCapacity() == 20,
            "Half capacity factor should reduce capacity from 40 to 20");
    simulation.step();
    require(simulation.totalArrived() == 20 &&
            simulation.lastStepStats().queuedVehicles == 5,
            "Only 20 vehicles should pass when capacity is reduced to 20");
    simulation.setRoadCapacityReduction("A", "B", 1.0);
    simulation.step();
    require(simulation.totalArrived() == 25,
            "Restoring capacity should release the remaining queue");

    bool threw = false;
    try { simulation.setRoadCapacityReduction("A", "B", -0.1); }
    catch (const std::invalid_argument&) { threw = true; }
    require(threw, "Negative capacity reduction factor should be rejected");
    threw = false;
    try { simulation.setRoadCapacityReduction("A", "B", 1.1); }
    catch (const std::invalid_argument&) { threw = true; }
    require(threw, "Capacity reduction factor above one should be rejected");
}
void testRedAndBlockedQueues() {
    Simulation s(makeNetwork()); s.addVehicle(Vehicle("V1", {"A", "B", "C"}));
    s.setSignal("A", SignalState::Red); s.step();
    require(s.lastStepStats().queuedVehicles == 1, "red queues vehicle");
    require(s.vehicles().front().waitingTimeSeconds() == 1.0, "waiting time increases");
    Simulation b(makeNetwork()); b.addVehicle(Vehicle("V2", {"A", "B"}));
    b.setRoadBlocked("A", "B", true); b.step(); require(b.lastStepStats().queuedVehicles == 1, "blocked road queues vehicle");
    b.setRoadBlocked("A", "B", false); b.step(); require(b.totalArrived() == 1, "clearing incident permits movement");
}
void testInvalidRouteAndIdentifiers() {
    Simulation s(makeNetwork());
    bool threw = false; try { s.addVehicle(Vehicle("V1", {"A", "C"})); } catch (const std::invalid_argument&) { threw = true; }
    require(threw, "invalid route rejected");
    threw = false; try { s.addVehicle(Vehicle("V1", {"A", "B"})); s.addVehicle(Vehicle("V1", {"A", "B"})); } catch (const std::invalid_argument&) { threw = true; }
    require(threw, "duplicate vehicle ID rejected");
    threw = false; try { s.setSignal("missing", SignalState::Green); } catch (const std::invalid_argument&) { threw = true; }
    require(threw, "unknown intersection signal rejected");
    threw = false; try { s.setRoadBlocked("A", "missing", true); } catch (const std::invalid_argument&) { threw = true; }
    require(threw, "unknown road incident rejected");
}
void testConstructorBoundaries() {
    bool threw = false; try { Road bad("R", "A", "B", 0); } catch (const std::invalid_argument&) { threw = true; }
    require(threw, "zero lanes rejected");
    threw = false; try { Intersection bad(""); } catch (const std::invalid_argument&) { threw = true; }
    require(threw, "empty intersection ID rejected");
    threw = false; try { Simulation bad(makeNetwork(), 0.0); } catch (const std::invalid_argument&) { threw = true; }
    require(threw, "zero step rejected");
    threw = false; try { Simulation bad(makeNetwork(), std::numeric_limits<double>::quiet_NaN()); } catch (const std::invalid_argument&) { threw = true; }
    require(threw, "NaN step rejected");
}
void testSummaryEmptyShortAndNormal() {
    Simulation empty(makeNetwork());
    auto report = empty.summary();
    require(report.stepsExecuted == 0 && report.vehiclesAdded == 0 && report.totalArrived == 0, "empty summary counts");
    require(report.averageWaitingTimeSeconds == 0.0 && report.averageQueueLength == 0.0 && report.throughputVehiclesPerSecond == 0.0, "empty summary avoids division by zero");
    empty.run(0); require(empty.stepsExecuted() == 0, "zero-step run is safe");

    Simulation shortRun(makeNetwork()); shortRun.addVehicle(Vehicle("V1", {"A", "B"})); shortRun.run(1);
    report = shortRun.summary();
    require(report.stepsExecuted == 1 && report.vehiclesAdded == 1 && report.activeVehicles == 0 && report.totalArrived == 1, "one-step route summary");
    require(std::isfinite(report.throughputVehiclesPerSecond), "summary metrics finite");

    Simulation normal(makeNetwork()); normal.addVehicle(Vehicle("V1", {"A", "B", "C"}));
    normal.addVehicle(Vehicle("V2", {"A", "B", "C"})); normal.run(2);
    report = normal.summary();
    require(report.stepsExecuted == 2 && report.vehiclesAdded == 2, "normal run counts");
    require(report.totalArrived == 2 && report.activeVehicles == 0, "normal run completion");
    require(report.throughputVehiclesPerSecond > 0.0, "normal throughput measured");
}
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

void testGenerationSeedReproducibility() {
    auto runWithSeed = [](unsigned int seed) {
        Simulation simulation(makeNetwork());
        simulation.configureVehicleGeneration(
            {{"A", "B", "C"}, {"A", "B"}},
            TrafficDensity::High,
            10,
            seed
        );
        simulation.run(10);
        std::vector<std::vector<std::string>> routes;
        for (const auto& vehicle : simulation.vehicles()) routes.push_back(vehicle.route());
        return routes;
    };
    require(runWithSeed(42) == runWithSeed(42),
            "The same seed should reproduce the same generated routes");
}
void testTrafficDensityLevels() {
    auto countFor = [](TrafficDensity density) {
        Simulation simulation(makeNetwork());
        simulation.configureVehicleGeneration({{"A", "B", "C"}}, density, 20);
        simulation.run(12);
        return simulation.vehicles().size();
    };
    const auto low = countFor(TrafficDensity::Low);
    const auto medium = countFor(TrafficDensity::Medium);
    const auto high = countFor(TrafficDensity::High);
    require(low < medium && medium < high,
            "Higher traffic density should generate more vehicles over the same duration");
}

void testConfigValidationAndParser() {
    using namespace tms::signals;
    auto c = cfg(); require(validate(c).ok, "valid config accepted");
    c.phases[0].green_s = 1; require(!validate(c).ok, "invalid green rejected");
    std::istringstream in("intersection A strategy fixed all_red 1\nphase A NS green 5 yellow 2 roads AB\nphase A EW green 7 yellow 2 roads AX\n");
    require(parse_signal_config(in).ok(), "valid config parses");
    std::istringstream bad("intersection A strategy unknown\n"); require(!parse_signal_config(bad).ok(), "bad strategy rejected");
    std::istringstream malformed("intersection A all_red nope\nphase A NS green 5 yellow 2 roads AB\n");
    require(!parse_signal_config(malformed).ok(), "malformed integer rejected");
    std::istringstream missing("phase A NS green 5 roads AB\n");
    require(!parse_signal_config(missing).ok(), "undeclared intersection rejected");
}
void testFixedTime() {
    using namespace tms::signals;
    SignalController c(cfg()); require(c.state("AB") == tms::signals::SignalState::GREEN, "initial phase green");
    c.tick(5); require(c.stage() == Stage::YELLOW, "green ends on schedule"); require(c.invariants_hold(), "yellow invariant");
    c.tick(2); require(c.stage() == Stage::ALL_RED, "yellow then all-red");
    require(c.state("AB") == tms::signals::SignalState::RED, "all-red stops approach");
    c.tick(1); require(c.state("AX") == tms::signals::SignalState::GREEN, "next phase after clearance");
    require(c.invariants_hold(), "transition invariant");
}
void testAdaptive() {
    using namespace tms::signals;
    auto x = cfg(); x.strategy = Strategy::ADAPTIVE; SignalController c(x);
    require(c.set_queue_length("AB", 4), "queue accepted"); c.tick(5);
    require(c.stage() == Stage::GREEN, "high queue extends green");
    require(c.set_queue_length("AB", 0), "queue clears"); c.tick(1);
    require(c.stage() == Stage::YELLOW, "adaptive green ends below threshold");
    require(!c.set_queue_length("AB", -1), "negative queue rejected");
}
void testEmergency() {
    using namespace tms::signals;
    SignalController c(cfg()); require(c.request_emergency("AX"), "emergency accepted");
    c.tick(1); require(c.stage() == Stage::YELLOW, "emergency uses yellow"); require(c.invariants_hold(), "safe yellow transition");
    c.tick(2); require(c.stage() == Stage::ALL_RED, "emergency uses all-red");
    c.tick(1); require(c.state("AX") == tms::signals::SignalState::GREEN, "emergency approach gets green");
    require(c.invariants_hold(), "safe emergency green"); require(c.clear_emergency("AX"), "emergency cleared");
}
void testEmergencyVehicleIntegration() {
    auto network = makeNetwork();
    network.addRoad(Road("AX", "A", "C", 1));
    Simulation simulation(network);
    require(simulation.configureSignal(cfg()).ok,
            "Signal controller should configure for emergency test");
    simulation.addVehicle(Vehicle("EV1", {"A", "C"}, VehicleType::Emergency));
    simulation.run(8);
    require(simulation.totalArrived() == 1,
            "Emergency vehicle should receive priority and reach its destination");
    require(simulation.vehicles().front().waitingTimeSeconds() >= 1.0,
            "Emergency vehicle should wait for safe yellow/all-red clearance");
}
void testPersistentInvalidConfigurationLogging() {
    using namespace tms::signals;
    SignalManager manager;
    std::ostringstream persisted;
    manager.set_log_sink([&persisted](const std::string& line) {
        persisted << line << '\n';
    });

    auto firstInvalid = cfg();
    firstInvalid.phases[0].green_s = 0;
    require(!manager.configure(firstInvalid).ok,
            "invalid green timing should be rejected");

    auto secondInvalid = cfg();
    secondInvalid.phases[0].yellow_s = 0;
    require(!manager.configure(secondInvalid).ok,
            "invalid yellow timing should be rejected");

    auto thirdInvalid = cfg();
    thirdInvalid.intersection_id = "BAD\nFORGED";
    require(!manager.configure(thirdInvalid).ok,
            "invalid intersection identifier should be rejected");

    require(manager.invalid_attempt_log().size() == 3,
            "exactly three invalid attempts should create exactly three log entries");
    const auto persistedText = persisted.str();
    std::size_t auditLines = 0;
    for (char ch : persistedText) if (ch == '\n') ++auditLines;
    require(auditLines == 3, "persistent sink should contain exactly three audit lines");
    require(persistedText.find("INVALID_CONFIG") != std::string::npos,
            "log sink should receive invalid configuration entries");
    require(persistedText.find("\nFORGED") == std::string::npos,
            "newline log injection should be neutralized");
    require(manager.invalid_attempt_log().front().find("T") != std::string::npos,
            "log entries should include a timestamp");
}
void testSignalValidationBranches() {
    using namespace tms::signals;
    auto invalid = [](const IntersectionSignalConfig& config) { return !validate(config).ok; };

    auto x = cfg(); x.intersection_id.clear(); require(invalid(x), "empty intersection ID should fail");
    x = cfg(); x.intersection_id = "ABCDEFGHIJK"; require(invalid(x), "overlong intersection ID should fail");
    x = cfg(); x.intersection_id = "bad id"; require(invalid(x), "invalid intersection characters should fail");
    x = cfg(); x.phases.resize(1); require(invalid(x), "one phase should fail");
    x = cfg(); x.phases.resize(9); require(invalid(x), "more than eight phases should fail");
    x = cfg(); x.all_red_s = 0; require(invalid(x), "all-red below range should fail");
    x = cfg(); x.all_red_s = 11; require(invalid(x), "all-red above range should fail");
    x = cfg(); x.phases[0].name = "bad name"; require(invalid(x), "invalid phase name should fail");
    x = cfg(); x.phases[1].name = x.phases[0].name; require(invalid(x), "duplicate phase name should fail");
    x = cfg(); x.phases[0].roads.clear(); require(invalid(x), "phase without roads should fail");
    x = cfg(); x.phases[0].roads = {"bad!"}; require(invalid(x), "invalid road identifier should fail");
    x = cfg(); x.phases[1].roads = {"AB"}; require(invalid(x), "road assigned to two phases should fail");
    x = cfg(); x.phases[0].green_s = 4; require(invalid(x), "green below range should fail");
    x = cfg(); x.phases[0].green_s = 121; require(invalid(x), "green above range should fail");
    x = cfg(); x.phases[0].yellow_s = 1; require(invalid(x), "yellow below range should fail");
    x = cfg(); x.phases[0].yellow_s = 11; require(invalid(x), "yellow above range should fail");
    x = cfg(); x.strategy = Strategy::ADAPTIVE; x.adaptive.min_green_s = 4; require(invalid(x), "adaptive minimum below range should fail");
    x = cfg(); x.strategy = Strategy::ADAPTIVE; x.adaptive.max_green_s = 121; require(invalid(x), "adaptive maximum above range should fail");
    x = cfg(); x.strategy = Strategy::ADAPTIVE; x.adaptive.min_green_s = 9; x.adaptive.max_green_s = 8; require(invalid(x), "adaptive max below min should fail");
    x = cfg(); x.strategy = Strategy::ADAPTIVE; x.adaptive.extension_threshold = 0; require(invalid(x), "zero adaptive threshold should fail");
    x = cfg(); x.strategy = Strategy::ADAPTIVE; x.adaptive.extension_threshold = 1001; require(invalid(x), "adaptive threshold above range should fail");

    require(std::string(to_string(tms::signals::SignalState::RED)) == "RED" &&
            std::string(to_string(tms::signals::SignalState::GREEN)) == "GREEN" &&
            std::string(to_string(tms::signals::SignalState::YELLOW)) == "YELLOW",
            "all signal state enum values should have stable text");
    require(std::string(to_string(Strategy::FIXED_TIME)) == "fixed" &&
            std::string(to_string(Strategy::ADAPTIVE)) == "adaptive",
            "all strategy enum values should have stable text");
    require(std::string(to_string(Stage::GREEN)) == "GREEN" &&
            std::string(to_string(Stage::YELLOW)) == "YELLOW" &&
            std::string(to_string(Stage::ALL_RED)) == "ALL_RED",
            "all controller stage enum values should have stable text");
    require(std::string(to_string(static_cast<tms::signals::SignalState>(99))) == "RED",
            "unknown signal state should fail safe to RED");
    require(std::string(to_string(static_cast<Strategy>(99))) == "fixed",
            "unknown strategy enum should use fixed fallback text");
    require(std::string(to_string(static_cast<Stage>(99))) == "ALL_RED",
            "unknown controller stage should fail safe to ALL_RED");
}

void testSignalControllerBranches() {
    using namespace tms::signals;
    IntersectionSignalConfig config;
    config.intersection_id = "A";
    config.strategy = Strategy::FIXED_TIME;
    config.phases = {
        {"P0", {"AB"}, 5, 2},
        {"P1", {"AC"}, 5, 2},
        {"P2", {"AD"}, 5, 2}
    };
    config.all_red_s = 1;
    SignalController controller(config);

    require(controller.invariants_hold(), "initial controller invariants should hold");
    require(controller.state("AB") == tms::signals::SignalState::GREEN &&
            controller.state("AC") == tms::signals::SignalState::RED,
            "initial phase state should be green only for its approach");
    require(controller.state("UNKNOWN") == tms::signals::SignalState::RED,
            "unknown road should be RED");
    require(controller.set_queue_length("UNKNOWN", 1) == false,
            "unknown queue road should be rejected");
    require(controller.set_queue_length("AB", -1) == false,
            "negative queue length should be rejected");
    require(controller.set_queue_length("AB", 4), "known queue should be updated");
    controller.set_queue_lengths({{"AC", 3}, {"AD", 8}, {"UNKNOWN", 4}, {"AB", -2}});
    require(controller.snapshot().size() == 3, "snapshot should include configured roads");
    require(!controller.request_emergency("UNKNOWN"), "unknown emergency road should be rejected");
    require(!controller.clear_emergency("UNKNOWN"), "unknown emergency clear should be rejected");

    require(controller.request_emergency("AC"), "first emergency request should be accepted");
    require(controller.request_emergency("AC"), "duplicate emergency request should increment count");
    require(controller.request_emergency("AD"), "second approach emergency should be queued");
    controller.tick(1);
    require(controller.stage() == Stage::YELLOW &&
            controller.state("AB") == tms::signals::SignalState::YELLOW &&
            controller.state("AC") == tms::signals::SignalState::RED,
            "emergency on another approach should trigger yellow only for the current phase");
    controller.tick(2);
    require(controller.stage() == Stage::ALL_RED &&
            controller.state("AB") == tms::signals::SignalState::RED &&
            controller.state("AC") == tms::signals::SignalState::RED,
            "yellow should transition to all-red for every approach");
    require(controller.request_emergency("AB"), "emergency arriving during clearance should be queued");
    controller.tick(1);
    require(controller.current_phase() == 1 && controller.stage() == Stage::GREEN &&
            controller.state("AC") == tms::signals::SignalState::GREEN,
            "first queued emergency should receive green after clearance");
    require(controller.clear_emergency("AC") && controller.emergency_active(),
            "first clear should decrement duplicate emergency count");
    require(controller.clear_emergency("AC"), "second clear should remove first approach request");
    controller.tick(1);
    require(controller.stage() == Stage::YELLOW, "next queued emergency should trigger a safe transition");
    controller.tick(3);
    require(controller.current_phase() == 2 && controller.stage() == Stage::GREEN &&
            controller.state("AD") == tms::signals::SignalState::GREEN,
            "second queued emergency should receive green in arrival order");
    require(controller.clear_emergency("AD"), "second approach request should clear");
    controller.tick(1);
    controller.tick(3);
    require(controller.current_phase() == 0 && controller.stage() == Stage::GREEN,
            "emergency arriving during clearance should be served next");
    require(controller.clear_emergency("AB"), "last emergency request should clear");
    require(!controller.clear_emergency("AB"), "clearing absent emergency should fail");
    require(!controller.emergency_active(), "emergency queue should be empty after all clears");
    require(controller.invariants_hold(), "controller invariants should hold after emergency sequence");

    const long long beforeNegativeTick = controller.time();
    controller.tick(-3);
    require(controller.time() == beforeNegativeTick, "negative tick should not advance time");

    config.strategy = Strategy::ADAPTIVE;
    config.adaptive.min_green_s = 5;
    config.adaptive.max_green_s = 5;
    config.adaptive.extension_threshold = 1;
    SignalController adaptive(config);
    adaptive.set_queue_lengths({{"AB", 0}, {"AC", 1}, {"AD", 9}});
    adaptive.tick(5);
    require(adaptive.stage() == Stage::YELLOW, "adaptive controller should end green below queue threshold");
    adaptive.tick(3);
    require(adaptive.current_phase() == 2, "adaptive controller should select the highest queued phase");
    require(adaptive.invariants_hold(), "adaptive controller invariants should hold");
}

void testSignalManagerFacadeBranches() {
    using namespace tms::signals;
    SignalManager manager;
    std::istringstream valid(
        "intersection A strategy fixed all_red 1\n"
        "phase A NS green 5 yellow 2 roads AB\n"
        "phase A EW green 5 yellow 2 roads AC\n"
        "intersection B strategy fixed all_red 1\n"
        "phase B NS green 5 yellow 2 roads BA\n"
        "phase B EW green 5 yellow 2 roads BC\n"
    );
    const auto configured = manager.configure_from_stream(valid, "valid-config");
    require(configured.ok && manager.has("A") && manager.has("B"),
            "valid multi-intersection stream should configure all controllers");
    require(manager.get("missing") == nullptr, "unknown mutable controller lookup should return null");
    const SignalManager& constManager = manager;
    require(constManager.get("missing") == nullptr, "unknown const controller lookup should return null");
    require(manager.state("missing", "AB") == tms::signals::SignalState::RED &&
            !manager.can_proceed("missing", "AB"),
            "unknown intersection should fail safe to RED");
    require(!manager.emergency_arrived("missing", "AB") &&
            !manager.emergency_passed("missing", "AB") &&
            !manager.update_queue("missing", "AB", 1),
            "unknown intersection operations should fail safely");
    require(manager.can_proceed("A", "AB"),
            "known current green approach should be permitted");
    require(!manager.emergency_arrived("A", "UNKNOWN"),
            "unknown emergency approach should be rejected");
    require(!manager.emergency_passed("A", "AB"),
            "clearing an absent emergency should fail");
    require(!manager.update_queue("A", "UNKNOWN", 2) &&
            manager.update_queue("A", "AB", 2),
            "queue update should reject unknown roads and accept known roads");
    manager.set_strategy_all(Strategy::ADAPTIVE);
    require(manager.get("A")->strategy() == Strategy::ADAPTIVE,
            "set_strategy_all should update all configured controllers");
    manager.set_strategy_all(Strategy::FIXED_TIME);
    require(manager.get("A")->strategy() == Strategy::FIXED_TIME,
            "set_strategy_all should also restore fixed-time strategy");

    SignalManager adaptiveManager;
    auto adaptiveConfig = cfg();
    adaptiveConfig.strategy = Strategy::ADAPTIVE;
    adaptiveConfig.adaptive.min_green_s = 5;
    adaptiveConfig.adaptive.max_green_s = 5;
    adaptiveConfig.adaptive.extension_threshold = 1;
    require(adaptiveManager.configure(adaptiveConfig).ok,
            "adaptive manager configuration should succeed");
    require(adaptiveManager.update_queue("A", "AB", 0),
            "adaptive manager should accept a current queue sample");
    adaptiveManager.tick(5);
    require(!adaptiveManager.can_proceed("A", "AB"),
            "approach should not proceed during adaptive yellow clearance");
    std::istringstream duplicateExisting(
        "intersection A strategy fixed all_red 1\n"
        "phase A NS green 5 yellow 2 roads AB\n"
        "phase A EW green 5 yellow 2 roads AC\n"
    );
    require(!manager.configure_from_stream(duplicateExisting, "existing-config").ok,
            "stream configuration should reject an already-configured intersection");

    require(!manager.configure(cfg()).ok,
            "configuring an already-configured intersection should fail");
    std::istringstream empty;
    require(!manager.configure_from_stream(empty, "empty-config").ok,
            "empty configuration stream should be rejected");
    std::istringstream duplicateBatch(
        "intersection C strategy fixed all_red 1\n"
        "phase C NS green 5 yellow 2 roads CA\n"
        "phase C EW green 5 yellow 2 roads CB\n"
        "intersection C strategy fixed all_red 1\n"
        "phase C NS green 5 yellow 2 roads CA\n"
        "phase C EW green 5 yellow 2 roads CB\n"
    );
    require(!manager.configure_from_stream(duplicateBatch, "duplicate-batch").ok &&
            !manager.has("C"),
            "duplicate batch IDs should reject the entire configuration atomically");
    require(!manager.invalid_attempt_log().empty(),
            "manager facade errors should be retained in the diagnostic log");
    manager.clear();
    require(!manager.has("A") && manager.get("A") == nullptr,
            "clear should remove configured controllers");
}

void testSignalParserErrorBranches() {
    using namespace tms::signals;
    std::istringstream malformed(
        "# comment and blank line\n"
        "intersection\n"
        "intersection A strategy\n"
        "intersection A strategy adaptive all_red nope\n"
        "intersection B strange value\n"
        "adaptive\n"
        "adaptive Z min_green 5\n"
        "adaptive A mystery 1 min_green bad threshold\n"
        "phase\n"
        "phase Z P green 5 roads R1\n"
        "phase A P green nope yellow 2\n"
        "phase A P unknown 2 roads AB\n"
        "phase A P green 5 yellow\n"
        "unknown directive\n"
    );
    const auto parsed = parse_signal_config(malformed);
    require(!parsed.ok() && parsed.errors.size() >= 10,
            "signal parser should report malformed directives, values and references");
}
void testRoadConstructorAndStateBranches() {
    auto throws = [](const auto& operation) {
        try { operation(); } catch (const std::invalid_argument&) { return true; }
        return false;
    };
    require(throws([] { Road("", "A", "B", 1); }), "empty road ID should fail");
    require(throws([] { Road("R", "", "B", 1); }), "empty source intersection should fail");
    require(throws([] { Road("R", "A", "", 1); }), "empty destination intersection should fail");
    require(throws([] { Road("R", "A", "A", 1); }), "self-connected road should fail");
    require(throws([] { Road("R", "A", "B", 0); }), "zero lanes should fail");
    require(throws([] { Road("R", "A", "B", std::numeric_limits<int>::max()); }),
            "unrepresentable default capacity should fail");
    require(throws([] { Road("R", "A", "B", 1, 0); }), "zero explicit capacity should fail");
    require(throws([] { Road("R", "A", "B", 0, 10); }), "zero lanes with explicit capacity should fail");

    Road road("R", "A", "B", 2, 40, true);
    require(road.isBlocked() && road.capacity() == 40 && road.lanes() == 2,
            "explicit road constructor should preserve capacity and blocked state");
    road.setBlocked(false);
    require(!road.isBlocked(), "road should unblock");
    road.setCapacityReductionFactor(0.0);
    require(road.effectiveCapacity() == 0, "zero capacity factor should stop discharge");
    road.setCapacityReductionFactor(1.0);
    require(road.effectiveCapacity() == 40 && road.capacityReductionFactor() == 1.0,
            "full capacity factor should restore base capacity");
    require(throws([&] { road.setCapacityReductionFactor(std::numeric_limits<double>::quiet_NaN()); }),
            "non-finite capacity factor should fail");
}

void testVehicleBoundaryBranches() {
    auto throws = [](const auto& operation) {
        try { operation(); } catch (const std::invalid_argument&) { return true; }
        return false;
    };
    require(throws([] { Vehicle("", {"A", "B"}); }), "empty vehicle ID should fail");
    require(throws([] { Vehicle("V", {}); }), "empty vehicle route should fail");
    require(throws([] { Vehicle("V", {"A", ""}); }), "empty route stop should fail");

    Vehicle normal("N", {"A", "B"});
    require(!normal.isEmergency() && normal.type() == VehicleType::Normal,
            "normal vehicle type should be preserved");
    require(normal.currentIntersection() == "A" && normal.nextIntersection() &&
            *normal.nextIntersection() == "B",
            "vehicle should expose current and next intersection");
    normal.waitOneStep(0.0);
    normal.waitOneStep(0.5);
    require(normal.waitingTimeSeconds() == 0.5, "zero and fractional wait durations should be handled");
    normal.moveToNextIntersection();
    require(normal.hasArrived() && normal.nextIntersection() == nullptr &&
            normal.currentIntersection() == "B",
            "vehicle should expose arrived state and no next intersection");
    normal.moveToNextIntersection();
    require(normal.currentIntersection() == "B", "moving an arrived vehicle should be a no-op");
    require(throws([&] { normal.waitOneStep(-0.1); }),
            "negative waiting duration should fail");

    Vehicle emergency("E", {"A", "B"}, VehicleType::Emergency);
    require(emergency.isEmergency(), "emergency vehicle type should be recognized");
}

void testSimulationBoundaryAndFacadeBranches() {
    auto network = makeNetwork();
    Simulation simulation(network);
    auto throws = [](const auto& operation) {
        try { operation(); } catch (const std::invalid_argument&) { return true; }
        return false;
    };
    require(throws([&] { simulation.addVehicle(Vehicle("BAD", {"A", "Z"})); }),
            "invalid vehicle route should be rejected");
    simulation.addVehicle(Vehicle("DUP", {"A", "B"}));
    require(throws([&] { simulation.addVehicle(Vehicle("DUP", {"A", "C"})); }),
            "duplicate vehicle ID should be rejected");
    require(throws([&] { simulation.configureVehicleGeneration({}, 1, 1); }),
            "empty generation route set with positive maximum should fail");
    require(throws([&] { simulation.configureVehicleGeneration({{"A", "Z"}}, 1, 1); }),
            "invalid generation route should fail");

    Simulation emptyGenerator(network);
    emptyGenerator.configureVehicleGeneration({}, 1, 0);
    emptyGenerator.step();
    require(emptyGenerator.vehicles().empty(),
            "empty zero-maximum generator should safely produce no vehicles");

    Simulation collision(network);
    collision.addVehicle(Vehicle("AUTO-1", {"A", "B"}));
    collision.configureVehicleGeneration({{"A", "B"}}, 1, 1, 42);
    collision.step();
    require(collision.vehicles().size() == 2 &&
            collision.vehicles()[1].id() == "AUTO-2",
            "automatic ID generation should skip a colliding manual ID");

    Simulation fractional(network, 0.5);
    fractional.addVehicle(Vehicle("F", {"A", "B"}));
    fractional.step();
    require(fractional.stepsExecuted() == 1,
            "fractional step should execute without advancing whole signal seconds");
    fractional.step();
    require(fractional.stepsExecuted() == 2,
            "fractional remainder should advance the signal clock on the next step");

    require(throws([&] { simulation.setSignal("UNKNOWN", SignalState::Green); }),
            "unknown signal intersection should throw");
    require(throws([&] { simulation.setRoadBlocked("A", "Z", true); }),
            "unknown road block request should throw");
    require(throws([&] { simulation.setRoadCapacityReduction("A", "Z", 0.5); }),
            "unknown road capacity request should throw");

    auto unknownSignal = cfg();
    unknownSignal.intersection_id = "Z";
    require(!simulation.configureSignal(unknownSignal).ok,
            "signal config for unknown intersection should fail");

    TrafficNetwork signalNetwork;
    signalNetwork.addIntersection(Intersection("A"));
    signalNetwork.addIntersection(Intersection("B"));
    signalNetwork.addIntersection(Intersection("C"));
    signalNetwork.addRoad(Road("AB", "A", "B", 1));
    signalNetwork.addRoad(Road("AC", "A", "C", 1));
    signalNetwork.addRoad(Road("BC", "B", "C", 1));
    Simulation signalSimulation(signalNetwork);

    auto wrongOutgoing = cfg();
    wrongOutgoing.phases[1].roads = {"BC"};
    require(!signalSimulation.configureSignal(wrongOutgoing).ok,
            "signal phase referencing a non-outgoing road should fail");
    auto validSignal = cfg();
    validSignal.phases[1].roads = {"AC"};
    require(signalSimulation.configureSignal(validSignal).ok,
            "valid outgoing road signal config should succeed");
    require(!signalSimulation.configureSignal(validSignal).ok,
            "duplicate signal controller config should fail");
    require(signalSimulation.emergencyArrived("A", "AB"),
            "simulation should forward a valid emergency arrival");
    require(signalSimulation.emergencyPassed("A", "AB"),
            "simulation should forward a valid emergency passage");
    require(!signalSimulation.emergencyArrived("UNKNOWN", "AB") &&
            !signalSimulation.emergencyPassed("UNKNOWN", "AB"),
            "simulation emergency wrappers should fail safely for unknown intersection");
    signalSimulation.step(); // configured controller with no queue exercises zero-queue refresh
    const Simulation& constSimulation = signalSimulation;
    (void)constSimulation.signalManager();
}
void testIntegrationAndInvalidLog() {
    using namespace tms::signals;
    auto n = makeNetwork(); n.addRoad(Road("AX", "A", "C", 1)); Simulation s(n);
    require(s.configureSignal(cfg()).ok, "signal installs for valid outgoing roads");
    s.addVehicle(Vehicle("V1", {"A", "B"})); s.step(); require(s.totalArrived() == 1, "green controller permits movement");
    SignalManager m; auto invalid = cfg(); invalid.phases[0].yellow_s = 0;
    require(!m.configure(invalid).ok, "invalid config rejected"); require(!m.has("A"), "invalid config not applied");
    require(!m.invalid_attempt_log().empty(), "invalid config logged");
    auto unknown = cfg(); unknown.intersection_id = "UNKNOWN";
    require(!s.configureSignal(unknown).ok, "unknown network intersection rejected");
}
}
int main() {
    try {
        testNetworkConfigParser(); testRoutesAndNetworkValidation(); testMovement(); testRoadCapacityReduction(); testRedAndBlockedQueues(); testInvalidRouteAndIdentifiers();
        testAutomaticGenerationInterval(); testAutomaticGenerationMaximum();
        testAutomaticGenerationRejectsInvalidRoute(); testAutomaticGenerationRejectsZeroInterval();
        testGenerationSeedReproducibility(); testTrafficDensityLevels();
        testConstructorBoundaries(); testSummaryEmptyShortAndNormal(); testConfigValidationAndParser();
        testFixedTime(); testAdaptive(); testEmergency(); testEmergencyVehicleIntegration(); testPersistentInvalidConfigurationLogging(); testSignalValidationBranches(); testSignalControllerBranches();
        testSignalManagerFacadeBranches(); testSignalParserErrorBranches(); testRoadConstructorAndStateBranches();
        testVehicleBoundaryBranches(); testSimulationBoundaryAndFacadeBranches(); testIntegrationAndInvalidLog();
        std::cout << "All simulation, generation, summary, and signal tests passed.\n"; return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "TEST FAILURE: " << e.what() << '\n'; return EXIT_FAILURE;
    }
}
