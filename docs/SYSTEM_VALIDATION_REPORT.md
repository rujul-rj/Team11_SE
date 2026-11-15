# System Validation Report

## 1. Objective

Validate the integrated traffic simulation as a runnable C++17 application against the documented core workflows. This report is an execution record; do not mark a scenario passed until it has been run on the final release commit.

## 2. Environment record

- Commit SHA: `4ca98fb5e4858d58ecad7e955f6d4ed8be7be5b8`
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
| SV-05 | Emergency vehicle request | Safe transition grants priority to the emergency approach | PASS (automated regression) |
| SV-06 | Block and clear a road incident | Vehicle waits during blockage and resumes after clearance | PASS (automated regression) |
| SV-07 | Invalid route/configuration | Invalid input is rejected and diagnostics are available | PASS (automated regression) |
| SV-08 | Strategy comparison | Fixed-time and adaptive runs use identical topology, demand and duration | PASS (CI; identical metrics observed) |
| SV-09 | Repeated execution | Both executables complete 10 consecutive runs | PASS (CI) |
| SV-10 | Sanitizer execution | AddressSanitizer/UndefinedBehaviorSanitizer report no findings | PASS (CI) |

## 5. Measured CI outputs

- Main simulation: 4 steps, 2 vehicles added, 2 arrived, 0 active; total waiting time 2.00 vehicle-seconds; average waiting time 1.00 seconds/vehicle; average queue length 0.50 vehicles/step; throughput 0.50 vehicles/second.
- Strategy comparison (same four-intersection network, 10 vehicles, 30 one-second steps): fixed-time and adaptive each reported 10 arrived, 0 active, 40.000 vehicle-seconds total wait, 1.333 average queue and 0.333 vehicles/second throughput.
- CTest: 1/1 test executable passed; 100% tests passed.
- Repeated-run check: both executables completed 10 consecutive runs.
- ASan/UBSan: build and tests passed.
- Workflow: [run 43692422652](https://github.com/rujul-rj/Team11_SE/actions/runs/43692422652).

## 6. Limitations to disclose

The comparison executable currently uses a simplified movement model that does not constrain vehicle discharge by road capacity. Its sample fixed-time and adaptive metrics may therefore be identical. This is not evidence that the strategies are equivalent or that one is superior. The model is a simulation prototype, not a controller for real traffic infrastructure.

## 7. Sign-off

Complete the result column with PASS/FAIL, add output or screenshot references for each scenario, record defects and retest outcomes, and obtain team review before submission.
