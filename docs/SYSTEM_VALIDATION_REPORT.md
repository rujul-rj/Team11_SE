# System Validation Report

## 1. Objective

Validate the integrated traffic simulation as a runnable C++17 application against the documented core workflows. This report is an execution record; do not mark a scenario passed until it has been run on the final release commit.

## 2. Environment record

- Commit SHA: `bc2a643ff1067350b0a949dd8233ba3db12a677f`
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

- Main simulation: 12 steps, 8 vehicles added, 3 arrived and 5 active; total waiting time 44.00 vehicle-seconds; average waiting time 5.50 seconds/vehicle; average queue length 3.67 vehicles/step; throughput 0.25 vehicles/second.
- Strategy comparison (same network, 22 vehicles: 20 on AB and 2 on AC, 30 one-second steps): both strategies completed all 22 vehicles; fixed-time total wait 216.000 vehicle-seconds and average queue 7.200; adaptive total wait 156.000 vehicle-seconds and average queue 5.200; both throughput 0.733 vehicles/second. Adaptive performs better on this tested demand, but this is scenario-specific.
- CTest: 1/1 test executable passed; 100% tests passed. The executable contains 17 test functions.
- Repeated-run check: both executables completed 10 consecutive runs.
- ASan/UBSan: build and tests passed.
- NFR-001 performance: 1,000 vehicle-step updates, 10 runs per scenario. TC-013.1 baseline: min 0.165 / avg 0.168 / max 0.194 ms; TC-013.2 adaptive: min 0.175 / avg 0.179 / max 0.194 ms; TC-013.3 incidents+emergency: min 0.139 / avg 0.141 / max 0.147 ms. Runner: x86_64 Ubuntu VM, 4 CPUs, 15 GiB RAM, AMD EPYC 7763 64-Core Processor, GCC 15.2.0, CMake 4.5.0. This meets the STP compute baseline but is not the designated team desktop/laptop.
- Workflow: [run 43695727436](https://github.com/rujul-rj/Team11_SE/actions/runs/43693133717).

## 6. Limitations to disclose

The comparison executable now applies a simplified per-road capacity limit (default 20 vehicles per lane per simulation step). The sample scenario shows lower waiting time and queue under adaptive control, but the capacity model is not calibrated to real traffic and the result should not be generalized to all demand patterns. The model is a simulation prototype, not a controller for real traffic infrastructure.

## 7. Sign-off

Complete the result column with PASS/FAIL, add output or screenshot references for each scenario, record defects and retest outcomes, and obtain team review before submission.
