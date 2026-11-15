# System Validation Report

## 1. Objective

Validate the integrated traffic simulation as a runnable C++17 application against the documented core workflows. This report is an execution record; do not mark a scenario passed until it has been run on the final release commit.

## 2. Environment record

- Commit SHA: **to be recorded**
- Operating system / version: **to be recorded**
- Compiler / version: **to be recorded**
- CMake version: **to be recorded**
- Build configuration: **Debug or Release — record actual value**
- Tester / date: **to be recorded**

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
| SV-01 | Clean configure and build | All targets compile without errors | NOT RUN |
| SV-02 | Execute the main simulation | Vehicle movement, queue and end-of-run summary are printed | NOT RUN |
| SV-03 | Configure fixed-time signal | Controller follows green, yellow and all-red sequence | NOT RUN |
| SV-04 | Configure adaptive signal | Queue information affects green extension within configured bounds | NOT RUN |
| SV-05 | Emergency vehicle request | Safe transition grants priority to the emergency approach | NOT RUN |
| SV-06 | Block and clear a road incident | Vehicle waits during blockage and resumes after clearance | NOT RUN |
| SV-07 | Invalid route/configuration | Invalid input is rejected and diagnostics are available | NOT RUN |
| SV-08 | Strategy comparison | Fixed-time and adaptive runs use identical topology, demand and duration | NOT RUN |
| SV-09 | Repeated execution | Both executables complete 10 consecutive runs | NOT RUN |
| SV-10 | Sanitizer execution | AddressSanitizer/UndefinedBehaviorSanitizer report no findings | NOT RUN |

## 5. Limitations to disclose

The comparison executable currently uses a simplified movement model that does not constrain vehicle discharge by road capacity. Its sample fixed-time and adaptive metrics may therefore be identical. This is not evidence that the strategies are equivalent or that one is superior. The model is a simulation prototype, not a controller for real traffic infrastructure.

## 6. Sign-off

Complete the result column with PASS/FAIL, add output or screenshot references for each scenario, record defects and retest outcomes, and obtain team review before submission.
