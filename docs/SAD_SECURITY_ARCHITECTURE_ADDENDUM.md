# SAD Addendum — Security Architecture, API and Error Handling

## Security architecture

The application is a single-process C++17 simulation. Security controls are applied at configuration and module boundaries:

1. **Configuration boundary:** parse_signal_config() parses line-oriented signal configuration; validate() checks identifiers, phase count, timing ranges, adaptive parameters and duplicate road assignments before configuration is installed.
2. **Network boundary:** parse_network_config() and TrafficNetwork::addIntersection(), addRoad() and isValidRoute() reject malformed directives, duplicate IDs, unknown endpoints, invalid lanes/capacities and invalid route references.
3. **Simulation boundary:** Simulation rejects non-finite or non-positive time-step values, duplicate vehicle IDs, invalid routes and unknown road/intersection references.
4. **Signal safety:** SignalController enforces GREEN → YELLOW → ALL_RED transitions; unknown roads fail safe to RED; emergency requests use the normal clearance sequence.
5. **Diagnostics:** SignalManager records invalid configuration attempts in invalid_attempt_log(). The current log is in-memory, not a persistent audit file.
6. **Runtime verification:** CI runs AddressSanitizer and UndefinedBehaviorSanitizer against the automated test suite.

## Architectural pattern

The implementation follows a modular, layered simulation design: domain model (Road, Intersection, Vehicle, TrafficNetwork), simulation orchestration (Simulation), signal-control subsystem (tms::signals) and command-line executables. This is a modular layered design rather than a distributed or client-server architecture.

## API summary

| API | Responsibility | Error behaviour |
|---|---|---|
| TrafficNetwork::addIntersection() | Register unique intersection | Throws std::invalid_argument on duplicate ID |
| TrafficNetwork::addRoad() | Register road with known endpoints | Throws std::invalid_argument on invalid endpoints or duplicate directed road |
| TrafficNetwork::isValidRoute() | Validate connected route | Returns false for empty, unknown or disconnected route |
| Simulation::addVehicle() | Add a vehicle with valid route and unique ID | Throws std::invalid_argument |
| Simulation::configureSignal() | Validate and install signal controller against network roads | Returns ValidationResult with errors |
| SignalManager::configure_from_stream() | Parse and atomically apply signal configuration | Returns validation result and records invalid attempts |
| Simulation::step() / run() | Advance logical simulation time | Updates step statistics and summary |

## Known architecture deviations

The SRS/SAD conceptual model names separate IncidentManager and AnalyticsEngine components. The current code keeps incident blocking and analytics aggregation within Simulation; these are not yet independent classes. Network topology is currently assembled through the C++ API in the sample application rather than loaded from a user-editable network configuration file. These deviations are documented for the final evaluation and should not be described as implemented modules.
