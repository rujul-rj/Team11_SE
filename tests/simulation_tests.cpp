#include "Simulation.h"
#include "tms/signals/signal.hpp"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <sstream>
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
void testRoutes() {
    auto n = makeNetwork();
    require(n.isValidRoute({"A", "B", "C"}), "connected route valid");
    require(!n.isValidRoute({}), "empty route invalid");
    require(!n.isValidRoute({"A", "C"}), "missing road invalid");
    require(!n.isValidRoute({"A", "Z"}), "unknown intersection invalid");
}
void testMovement() {
    Simulation s(makeNetwork()); s.addVehicle(Vehicle("V1", {"A", "B", "C"}));
    s.step(); require(s.totalArrived() == 0, "vehicle traverses one road per step");
    s.step(); require(s.totalArrived() == 1, "vehicle arrives after second road");
    require(s.stepsExecuted() == 2, "step counter increments");
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
        testRoutes(); testMovement(); testRedAndBlockedQueues(); testInvalidRouteAndIdentifiers();
        testConstructorBoundaries(); testSummaryEmptyShortAndNormal(); testConfigValidationAndParser();
        testFixedTime(); testAdaptive(); testEmergency(); testIntegrationAndInvalidLog();
        std::cout << "All simulation and signal tests passed.\n"; return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "TEST FAILURE: " << e.what() << '\n'; return EXIT_FAILURE;
    }
}
