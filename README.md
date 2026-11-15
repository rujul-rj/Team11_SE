# Team 11 Traffic Management System

A C++17 single-process traffic simulation for experimenting with road networks, vehicle generation, signal control, emergency priority and road incidents. It does not control real traffic infrastructure or use live traffic data.

## Prerequisites

- C++17 compiler (GCC 9+, Clang 10+ or Visual Studio C++ toolchain)
- CMake 3.16+
- Git

The project uses a dependency-free C++ test harness registered with CTest.

## Clean build, test and run

Run from the repository root because the sample executable reads configuration files from config/.

### Linux / macOS / WSL

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/traffic_sim
./build/strategy_comparison
```

### Windows PowerShell

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
.\build\Release\traffic_sim.exe
.\build\Release\strategy_comparison.exe
```

## Configuration

The sample application loads both files before starting:

- config/network_example.cfg — intersections and directed roads.
- config/signals_example.cfg — signal strategy, phases, green/yellow timings and road assignments.

Network file format:

```text
intersection A
intersection B
intersection C
road R1 A B 2 40
road R2 B C 1 4
road R3 A C 1 20
```

Each road line is: road, road ID, source intersection, destination intersection, lane count, optional capacity. If capacity is omitted, the default is 20 vehicles per lane per simulation step. Malformed directives, duplicate IDs, unknown endpoints, invalid lanes and invalid capacities are rejected with line-specific errors.

Signal syntax and additional examples are documented in config/signals_example.cfg and tms/signals/README.md.

## Implemented capabilities

- Validated network configuration from a file, including roads, lanes, intersections and per-road capacity.
- LOW / MEDIUM / HIGH traffic-density generation with bounded vehicle counts and reproducible seeded route selection.
- Vehicle movement, route validation, queueing at red signals, blocked roads and capacity-limited roads.
- Fixed-time and adaptive signal control with safe GREEN → YELLOW → ALL_RED transitions.
- NORMAL / EMERGENCY vehicle types; emergency requests receive priority through the safe signal-clearance sequence.
- Road incidents that reduce a road's capacity by a factor in [0, 1] or block/unblock it.
- Per-step queue, waiting-time and throughput metrics plus an end-of-run summary.
- A deterministic fixed-time vs adaptive comparison on the same capacity-constrained demand.

## Executables

| Executable | Purpose |
|---|---|
| traffic_sim | Main sample scenario: loads network/signal files, generates traffic, demonstrates emergency priority and a capacity-reduction incident, then prints summary statistics |
| strategy_comparison | Runs the same 22-vehicle demand under fixed-time and adaptive strategies and prints side-by-side metrics |
| performance_benchmark | Runs TC-013.1 baseline, TC-013.2 adaptive and TC-013.3 incidents+emergency workloads; 1,000 vehicle-step updates per run, 10 repetitions each |
| reliability_benchmark | Runs 100 seeded 300-step scenarios and an all-roads-blocked stress case |

To run the Release performance and reliability checks locally:

```bash
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --parallel
./build-release/performance_benchmark
./build-release/reliability_benchmark
```

## Validation evidence

GitHub Actions runs:
- CMake build and CTest suite.
- Main simulation and strategy-comparison executables.
- Ten-repeat performance checks for baseline, adaptive and incident/emergency workloads.
- 100 seeded reliability runs, each 300 steps, plus an all-roads-blocked stress scenario.
- AddressSanitizer and UndefinedBehaviorSanitizer.
- cppcheck static analysis.
- gcovr line and branch coverage reports as a workflow artifact.

The current coverage report records aggregate core line coverage above 80%, but branch coverage remains below the STP target of 70%. See docs/CODE_COVERAGE_REPORT.md and docs/BRANCH_COVERAGE_REPORT.md.

## Metrics definitions

- Steps executed: simulation updates completed.
- Vehicles added / arrived / active: accepted vehicles, vehicles that completed their route, and vehicles not yet arrived.
- Total waiting time: sum of waiting-time counters for all vehicles.
- Average waiting time: total waiting time divided by vehicles added; zero when no vehicles were added.
- Average queue length: queued vehicle observations divided by executed steps.
- Throughput: completed vehicles divided by simulated elapsed time (steps × step duration).

These are simulation indicators, not calibrated predictions of real traffic.

## Known limitations and deviations

- Capacity is a simplified per-road, per-step model; it is not calibrated to real traffic flow or driver behaviour.
- Only one capacity-reduction factor is stored per road; overlapping incident objects and incident history are not modelled.
- The sample comparison is one demand pattern; its result does not prove adaptive control is universally better.
- There is no Administrator/Operator/Analyst role-enforcement mechanism.
- Invalid configuration attempts are retained in an in-memory diagnostic log; persistent timestamped audit logging is not implemented.
- Incident handling and analytics aggregation remain inside Simulation rather than separate IncidentManager and AnalyticsEngine classes.
- Tests use a custom require() harness rather than GoogleTest/GoogleMock.
- The CLI has not yet been validated by three first-time users; usability evidence remains outstanding.

See docs/IMPLEMENTATION_DEVIATIONS.md for the full SRS/SAD/STP deviation register.

## Final demo checklist (videos excluded from this repository task)

1. Show the validated network and signal configuration files.
2. Run CTest and explain the test report and coverage results.
3. Run traffic_sim; explain density generation, emergency priority, incident capacity reduction and end-of-run metrics.
4. Run strategy_comparison; explain that the same demand is used for both strategies and discuss the measured trade-off.
5. Run performance_benchmark and reliability_benchmark; show the CI evidence.
6. Explain known deviations and why the simulation is not a real traffic controller.
7. Record the required demo videos separately and attach their real URLs to the release evidence.

## Requirement-to-test traceability

| Requirement | Implementation / evidence |
|---|---|
| FR-001 / FR-012 | config/network_example.cfg and network_config_parser; positive and negative parser tests |
| FR-002 / FR-005 | config/signals_example.cfg and fixed-time controller tests |
| FR-003 | seeded LOW / MEDIUM / HIGH vehicle generation tests |
| FR-004 / BR-05 | vehicle movement, capacity queue and red-signal tests |
| FR-006 | adaptive signal controller and strategy comparison |
| FR-007 / BR-01 | emergency vehicle type and integrated signal-priority regression |
| FR-008 / BR-02 | per-road capacity reduction, block/unblock and invalid-road tests |
| FR-009 / FR-010 | per-step statistics and end-of-run summary tests |
| FR-011 | fixed/adaptive same-demand comparison executable |
| NFR-001 | three 1,000 vehicle-step performance scenarios, 10 repetitions each |
| NFR-002 | 100 seeded 300-step runs and all-roads-blocked stress scenario |
| SEC-001 / SEC-002 | network and signal parser negative/boundary tests |
| SEC-003 | cppcheck plus ASan/UBSan CI |
| SEC-004 | invalid signal configuration diagnostic log; persistent audit file remains a deviation |

## Contributing

Create a feature branch, run the build and tests, and open a pull request against main. Do not commit generated build directories. Coordinate shared simulation interfaces with the core simulation and signal-control owners before changing them.
