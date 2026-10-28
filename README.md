# Team 11 Traffic Management System

A C++17 **traffic simulation** for experimenting with road networks, vehicles, signal control and traffic incidents. This is not a controller for real-world traffic infrastructure.

## Prerequisites

- C++17 compiler (GCC 9+, Clang 10+, or a current Visual Studio C++ toolchain)
- CMake 3.16+
- Git

No third-party test framework is currently required; the repository uses a dependency-free C++ test executable registered with CTest.

## Clean build, test and run

Run these commands from the repository root.

### Linux / macOS / WSL (GCC or Clang)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/traffic_sim
./build/strategy_comparison
```

### Windows PowerShell (Visual Studio generator)

```powershell
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
.\build\Release\traffic_sim.exe
.\build\Release\strategy_comparison.exe
```

For MinGW or another single-configuration generator, executables are usually under `build\`. If CMake cannot find a compiler, install the C++ workload for Visual Studio or a GCC/Clang toolchain and reopen the terminal.

## What is implemented

- Directed roads with lane counts and intersection endpoints.
- Route validation before a vehicle enters simulation state.
- Vehicle movement, waiting-time accounting and queue behaviour at red signals or blocked roads.
- Road incidents represented by blocking and unblocking a road.
- Fixed-time/adaptive signal controllers, safe phase transitions, emergency priority and signal configuration parsing in `tms/signals/`.
- Per-step statistics and end-of-run summary statistics, including active/arrived vehicles, accumulated waiting time, average queue length and throughput.
- A deterministic `strategy_comparison` executable that runs fixed-time and adaptive signal configurations with the same network, vehicle demand, step duration and run length.
- Automated tests for route/configuration validation, boundaries, incidents, signal transitions and empty/short/normal summaries.

Signal configuration syntax and an example are in [config/signals_example.cfg](config/signals_example.cfg) and [tms/signals/README.md](tms/signals/README.md).

## Metrics definitions

- **Steps executed:** number of simulation updates completed.
- **Vehicles added / arrived / active:** accepted vehicles, vehicles that completed their route during the run, and vehicles not yet arrived.
- **Total waiting time (seconds):** sum of waiting-time counters for all vehicles at the end of the run.
- **Average waiting time (seconds per added vehicle):** total waiting time divided by vehicles added; zero when no vehicles were added.
- **Average queue length (vehicles per step):** total queued vehicle observations divided by executed steps; zero when no steps were executed.
- **Throughput (vehicles/second):** completed vehicles divided by simulated elapsed time (steps × configured step duration); zero for a zero-step run.

These are simulation-level indicators, not calibrated predictions of real traffic.

## Comparing strategies

Run `strategy_comparison` after building. The executable prints fixed-time and adaptive results side by side for the same deterministic four-intersection network, 10 vehicles, 1-second steps and 30-step run. Compare arrivals, active vehicles, total wait, average queue and throughput. The small workload demonstrates the comparison workflow; it is not evidence that one strategy is universally better or that city-scale performance targets are met.

## Reliability and performance evidence

Use a clean checkout and retain the output when reporting results:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

For a sanitizer build on GCC/Clang:

```bash
cmake -S . -B build-asan -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build build-asan --parallel
ctest --test-dir build-asan --output-on-failure
```

Record compiler/version, operating system, commit SHA, workload (intersections, roads, vehicles and steps), elapsed wall time, number of repeated runs, failures and sanitizer result. Do not report performance targets as met until the workload and target from the SRS have been checked and measured on the target environment.

## Demo checklist

1. Configure and build from a clean checkout.
2. Run CTest and show the passing test summary.
3. Run `traffic_sim` and explain active, queued, throughput and waiting-time output.
4. Demonstrate an invalid route or malformed signal configuration being rejected.
5. Block a road, show waiting, clear the incident and show movement resuming.
6. Show the end-of-run summary and run `strategy_comparison`; explain that the demand and run length are held constant.

## Known limitations

- The model is intentionally simplified; it does not model real-world driver behaviour, calibrated road capacity, pedestrian phases, emissions or stochastic arrivals.
- Performance and memory-safety claims require recorded runs from the target environment; this README does not imply those measurements have already been completed.
- The comparison uses a small deterministic demand; repeat runs and broader workloads are needed before making general claims about strategy quality.
- The demo uses a small network; results should not be generalized to city-scale traffic.

## Requirement-to-test traceability

| Requirement / issue | Coverage |
|---|---|
| FR-012 / #7, #37 | Invalid and disconnected routes, unknown intersection and road IDs |
| SEC-001 / #10, #29, #37 | Malformed configuration and parser diagnostics |
| SEC-002 / #11, #30, #37 | Invalid lane count, invalid timing, non-finite step duration and negative queue values |
| FR-008 / #17 | Blocked road queues vehicles; clearing the block allows movement |
| FR-009 / #18, #38 | Per-step statistics and cumulative summary |
| FR-010 / #19, #38 | Empty, short and normal run summary |
| FR-011 / #20, #38 | Fixed-time/adaptive comparison under identical deterministic demand |
| NFR-005 / #9, #28, #40 | Clean configure/build/test/run commands above |
| NFR-001/002, SEC-003/004 / #39 | Sanitizer and repeatability procedure above; attach measured results before claiming pass |
| PB-25 / #35 | Use the demo checklist; add the actual recording URL and freeze commit when available |

## Contributing

Create a feature branch, run the build and tests, and open a pull request against `main`. Do not commit generated build directories. Coordinate shared simulation interfaces with the core simulation and signal-control owners before changing them.
