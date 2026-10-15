# Team 11 — Sprint Backlog
**Project:** Traffic Management System (Simulation)  
**Repository:** https://github.com/rujul-rj/Team11_SE  
**Technology stated in project materials:** C++17, CMake, GoogleTest  
**Planning window:** Sprint 1 — 19–23 Oct 2026; Sprint 2 — 26–30 Oct 2026

## Team
- Rohan Komperla — PES1UG24CS380
- Rujul Jain — PES1UG24CS388
- Shana Susan Varghese — PES1UG24CS427
- M Ajjaiah — PES1UG25CS823

> Story points and task splits below are planning proposals. The team should confirm them during sprint planning against the complete SRS/STP and actual implementation. No implementation files were visible in the repository at planning time, so CI build commands must be aligned to the source tree once it is committed.

## Workflow and board fields
**Columns:** Todo → In Progress → In Review → Done  
**Fields:** Sprint, Story Points, Requirement ID, Priority, Owner, Test Evidence  
**Priority:** P0 = essential for a working simulation; P1 = core feature; P2 = enhancement.

## Product backlog / acceptance criteria

| ID | User story / task | Priority | Points | Acceptance criteria |
|---|---|---:|---:|---|
| PB-01 / FR-001 | As a user, I can define roads, lanes and intersections so the network can be represented. | P0 | 5 | Valid network structures can be created and inspected; invalid connections are rejected with a clear error. |
| PB-02 / FR-002 | As a user, I can configure traffic signals at intersections. | P0 | 3 | Each configured signal has valid phases/states and can be queried during a simulation. |
| PB-03 / FR-003 | As a user, I can generate vehicles with starting locations and routes. | P0 | 5 | Vehicles are created only for valid routes and appear in the simulation state. |
| PB-04 / FR-004 | As a user, I can simulate vehicle movement and queues. | P0 | 8 | Vehicles advance according to network/signal constraints; queue state updates consistently. |
| PB-05 / FR-012 | As a user, I receive validation errors for an invalid road/intersection network. | P0 | 3 | Invalid input is detected before simulation; error identifies the invalid configuration. |
| PB-06 / NFR-003 | As a developer, I can maintain separate, testable components. | P1 | 3 | Core components have clear interfaces; unit tests can exercise them independently. |
| PB-07 / NFR-005 | As a user, I can build and run the simulation through a documented simple workflow. | P1 | 2 | README lists prerequisites and verified configure/build/test/run commands. |
| PB-08 / SEC-001 | As a user, I cannot start the simulation with malformed configuration. | P0 | 2 | Malformed configuration fails safely and returns a useful diagnostic. |
| PB-09 / SEC-002 | As a user, invalid or out-of-range values are rejected. | P0 | 2 | Boundary and invalid-value tests pass; no invalid values silently enter simulation state. |
| PB-10 / Engineering | Establish CMake build, GoogleTest test target, and CI baseline. | P0 | 5 | A clean checkout can configure/build/test in CI once source and test files exist; failures fail the workflow. |
| PB-11 / Demo | Sprint 1 working-product video (~1 minute). | P0 | 1 | Video demonstrates the implemented network, signals, vehicle creation/movement, and validation where implemented. |
| PB-12 / FR-005 | As a user, I can run a fixed-time signal strategy. | P1 | 3 | Signal phases follow configured durations over multiple cycles. |
| PB-13 / FR-006 | As a user, I can run an adaptive signal strategy. | P1 | 5 | Signal timing changes in response to the defined traffic/queue inputs; behaviour is testable and documented. |
| PB-14 / FR-007 | As a user, emergency vehicles can receive signal priority. | P1 | 5 | Emergency priority is triggered by a defined condition and does not leave signals in an invalid state. |
| PB-15 / FR-008 | As a user, I can represent a traffic incident. | P1 | 3 | Incident input affects the appropriate road/traffic state and can be removed or resolved as specified. |
| PB-16 / FR-009 | As a user, I can view simulation metrics. | P1 | 3 | Required metrics are calculated from simulation state and have documented units/meaning. |
| PB-17 / FR-010 | As a user, I can view a summary at the end of a run. | P1 | 2 | Summary reports the required final statistics and handles empty/short runs safely. |
| PB-18 / FR-011 | As a user, I can compare traffic-management strategies. | P1 | 5 | Strategies run under comparable inputs and outputs can be compared using the defined metrics. |
| PB-19 / NFR-001 | As a user, the simulation meets the performance target in the requirements. | P1 | 3 | A repeatable test/benchmark records workload, duration and result against the SRS target. |
| PB-20 / NFR-002 | As a user, the simulation behaves reliably across repeated runs. | P1 | 3 | Repeated valid runs complete without crashes or corrupted state; failures are reproducible. |
| PB-21 / NFR-004 | As a developer, components can be tested independently. | P1 | 2 | Unit tests cover core component interfaces without requiring the entire simulation. |
| PB-22 / SEC-003 | As a user, invalid inputs do not cause unsafe memory behaviour. | P0 | 3 | Boundary/invalid-input tests pass; build/test run under available sanitizers when feasible. |
| PB-23 / SEC-004 | As a user, invalid configuration produces useful diagnostics. | P1 | 2 | Error path is logged/reported without dumping secrets or terminating unexpectedly. |
| PB-24 / Release | Complete traceability, regression tests, README and final validation. | P0 | 3 | Requirement-to-test mapping is updated; clean build/tests pass; known limitations are recorded. |
| PB-25 / Demo | Sprint 2 demo (~2 minutes), documentation and development freeze. | P0 | 2 | Demo covers the finished feature set; documentation is current; freeze commit/tag is identified. |

## Sprint 1 — 19–23 October 2026
**Sprint goal:** Deliver a runnable simulation foundation: network model, signals, vehicle generation/movement, validation, and automated build/test baseline.

- PB-01 / FR-001 — Road/lane/intersection model
- PB-02 / FR-002 — Signal configuration
- PB-03 / FR-003 — Vehicle generation/routes
- PB-04 / FR-004 — Movement and queues
- PB-05 / FR-012 — Invalid network validation
- PB-06 / NFR-003 — Modular components
- PB-07 / NFR-005 — Build/run documentation
- PB-08 / SEC-001 — Configuration validation
- PB-09 / SEC-002 — Range validation
- PB-10 — CMake + GoogleTest + CI baseline
- PB-11 — One-minute working-product video

**Sprint 1 review checklist:** clean checkout/build; run unit tests; demonstrate the implemented flow; show one invalid-input case; list unfinished acceptance criteria honestly.

## Sprint 2 — 26–30 October 2026
**Sprint goal:** Complete signal strategies, emergency and incident behaviour, metrics, reliability/security validation, comparison and final demonstration.

- PB-12 / FR-005 — Fixed-time strategy
- PB-13 / FR-006 — Adaptive strategy
- PB-14 / FR-007 — Emergency priority
- PB-15 / FR-008 — Incident handling
- PB-16 / FR-009 — Metrics
- PB-17 / FR-010 — End-of-run summary
- PB-18 / FR-011 — Strategy comparison
- PB-19 / NFR-001 — Performance validation
- PB-20 / NFR-002 — Reliability validation
- PB-21 / NFR-004 — Independent testability
- PB-22 / SEC-003 — Memory/input safety
- PB-23 / SEC-004 — Diagnostic logging
- PB-24 — Traceability, regression, documentation
- PB-25 — Two-minute demo and freeze

## Definition of Ready
- Story references an SRS requirement or a necessary engineering deliverable.
- Acceptance criteria and test evidence are stated.
- Dependencies are identified and the story is small enough to finish within the sprint.
- Owner and estimate are agreed by the team.

## Definition of Done
- Implementation is committed and reviewed.
- Acceptance criteria are demonstrated.
- Relevant GoogleTest tests pass locally and in CI.
- No known regression is introduced; invalid input is handled safely.
- Documentation and requirement-to-test traceability are updated.
- Demo evidence is captured where the sprint deliverable requires it.

## Suggested meeting cadence
- **Planning:** agree sprint goal, confirm estimates, owners, dependencies and capacity.
- **Daily check-in:** yesterday / today / blockers; update board status.
- **Review:** demonstrate working software against acceptance criteria.
- **Retrospective:** record one thing to keep, one to improve, and an action owner.

## Requirement-to-test traceability starter
Create/maintain a test mapping for FR-001–FR-012, NFR-001–NFR-005 and SEC-001–SEC-004. For every requirement, record the corresponding test ID, test type (unit/integration/system), expected result, actual result, and evidence link. Exact test IDs should be taken from the team's STP and reconciled with the implemented test suite rather than invented.
