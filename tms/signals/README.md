# Traffic signal module

Implements signal configuration (FR-002), fixed-time strategy (FR-005), adaptive strategy (FR-006), emergency priority (FR-007), and validation/logging.

## Configuration format
```text
intersection <ID> [strategy fixed|adaptive] [all_red <seconds>]
adaptive <ID> [min_green <seconds>] [max_green <seconds>] [threshold <vehicles>]
phase <ID> <PHASE_NAME> [green <seconds>] [yellow <seconds>] roads <ROAD_ID> [ROAD_ID ...]
```
Declare an intersection before its phase/adaptive lines. Every intersection requires at least two phases. IDs must match outgoing road IDs; one road cannot belong to multiple phases. See `config/signals_example.cfg`.

## Behavior and safety
- Fixed-time phases follow GREEN -> YELLOW -> ALL_RED -> GREEN.
- Adaptive mode extends green while queue length meets the threshold, within min/max green limits, and selects the next phase with the largest queue (cyclic tie-break).
- Emergency requests are served in arrival order and use normal yellow/all-red clearance; call `emergencyPassed(intersectionId, roadId)` after passage.
- Unknown roads fail safe to RED. Invalid configurations are rejected and logged by `SignalManager::invalid_attempt_log()`.
- Configured controllers are integrated with `Simulation`; only GREEN permits movement at those intersections. Unconfigured intersections retain legacy RED/GREEN behavior.
- Simulation refreshes configured approach queues and advances signal time in whole seconds, preserving fractional step time.

## API
Call `Simulation::configureSignal(config)` after defining the network. Parser-driven configuration is available through `parse_signal_config(stream)`; validate parsed configs and configure each with the simulation API. Emergency events are exposed as `emergencyArrived` and `emergencyPassed`.

## Tests
`simulation_tests` covers parser errors, configuration ranges, fixed-time timing, adaptive extension, emergency clearance invariants, simulation integration, and invalid-config logging.
