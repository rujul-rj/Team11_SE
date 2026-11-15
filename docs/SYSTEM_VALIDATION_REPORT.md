# System Validation Report

## 1. Objective

Validate the integrated traffic simulation as a runnable C++17 application against the documented core workflows. This report is an execution record; do not mark a scenario passed until it has been run on the final release commit.

## 2. Environment record

- Commit SHA: `b0cd2d7c2e15beffa5dd71534432734ab897ce9f`
- Operating system / version: GitHub-hosted Ubuntu runner
- Compiler / version: GCC 15.2.0 (coverage build)
- CMake version: 4.5.0 (CI)
- Build configuration: Debug
- Tester / date: GitHub Actions automation / 15 November 2026

## 3. Build and execution

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/traffic_sim
./build/strategy_comparison
```

## 4. System scenarios

| ID | Scenario | Expected outcome | Result |
|---|---|---|---|
| SV-01 | Clean configure and build | All targets compile without errors | PASS (CI) |
| SV-02 | Execute the main simulation | Vehicle movement, queue and end-of-run summary are printed | PASS (CI) |
| SV-03 | Configure fixed-time signal | Controller follows green, yellow and all-red sequence | PASS (automated regression) |
| SV-04 | Configure adaptive signal | Queue information affects green extension within configured bounds | PASS (automated regression) |
| SV-05 | Emergency vehicle request | Safe transition grants priority to the emergency approach | PASS (integrated emergency vehicle regression) |
| SV-06 | Block and clear a road incident | Vehicle waits during blockage and resumes after clearance | PASS (automated regression) |
| SV-07 | Invalid route/configuration | Invalid input is rejected and diagnostics are available | PASS (automated regression) |
| SV-08 | Strategy comparison | Fixed-time and adaptive runs use identical topology, demand and duration | PASS (CI; identical metrics observed) |
| SV-09 | Repeated execution | Both executables complete 10 consecutive runs | PASS (CI) |
| SV-10 | Sanitizer execution | AddressSanitizer/UndefinedBehaviorSanitizer report no findings | PASS (CI) |

## 5. Measured CI outputs

- Main simulation: 12 steps, 8 vehicles added, 3 arrived and 5 active; total waiting time 43.00 vehicle-seconds; average waiting time 5.38 seconds/vehicle; average queue length 3.58 vehicles/step; throughput 0.25 vehicles/second.
- Strategy comparison (same network, 10 vehicles, 30 one-second steps): fixed-time and adaptive each reported 10 arrived, 40.000 vehicle-seconds total wait, 1.333 average queue and 0.333 vehicles/second throughput. Metrics are identical in this simplified model; do not claim a strategy winner.
- CTest: 1/1 test executable passed; 100% tests passed. The executable contains 17 test functions.
- Repeated-run check: both executables completed 10 consecutive runs.
- ASan/UBSan: build and tests passed.
- NFR-001 CI proxy: 1,000 vehicle-step updates per run, 10 runs; min 0.110 ms, average 0.113 ms, max 0.124 ms on the GitHub-hosted Ubuntu runner. This does not replace the approved reference-machine test.
- Workflow: [run 43692880777](https://github.com/rujul-rj/Team11_SE/actions/runs/43692880777).

## 6. Limitations to disclose

The comparison executable currently uses a simplified movement model that does not constrain vehicle discharge by road capacity. Its sample fixed-time and adaptive metrics may therefore be identical. This is not evidence that the strategies are equivalent or that one is superior. The model is a simulation prototype, not a controller for real traffic infrastructure.

## 7. Sign-off

Complete the result column with PASS/FAIL, add output or screenshot references for each scenario, record defects and retest outcomes, and obtain team review before submission.
