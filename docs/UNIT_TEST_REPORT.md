# Unit Test Report

## 1. Scope

This report inventories the dependency-free C++ test executable registered with CTest. The suite exercises network and route validation, vehicle movement, queue behaviour, signal configuration, fixed-time and adaptive control, emergency priority, incident recovery and simulation summaries.

## 2. Test environment

| Item | Value |
|---|---|
| Language | C++17 |
| Build system | CMake 3.16+ |
| Test executable | `simulation_tests` |
| Test runner | CTest |
| Test source | `tests/simulation_tests.cpp` |
| CI | GitHub Actions — C++ CMake CI |

Record the actual compiler, version, runner OS, commit SHA and Actions run URL from the final successful execution.

## 3. Test groups in the suite

1. Route and network validation — valid/invalid routes, duplicate intersections/roads and unknown endpoints.
2. Vehicle movement — progression over connected roads and arrival accounting.
3. Red-signal and blocked-road queues — waiting-time accounting and incident recovery.
4. Invalid route and identifiers — duplicate vehicle IDs and unknown signal/road references.
5. Constructor boundaries — zero lanes, empty identifiers, zero and non-finite simulation step.
6. Empty, short and normal summaries — counts, division-by-zero protection and finite throughput.
7. Signal configuration and parser — valid configuration, invalid timings, malformed values and undeclared intersections.
8. Fixed-time controller — green/yellow/all-red sequence and invariants.
9. Adaptive controller — queue-sensitive green extension and invalid queue rejection.
10. Emergency priority — safe transition and emergency clearance.
11. Integration and invalid-configuration logging — network/controller integration and rejected configuration diagnostics.

## 4. Execution

Run from the repository root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

**Final execution result:** PASS on Ubuntu GitHub Actions, 15 November 2026, commit `4ca98fb5e4858d58ecad7e955f6d4ed8be7be5b8`. CTest reported `1/1` test executable passed and `100% tests passed`. The test executable runs the 11 groups listed above. AddressSanitizer/UndefinedBehaviorSanitizer build and test job also passed. Workflow: [run 43692422652](https://github.com/rujul-rj/Team11_SE/actions/runs/43692422652).

## 5. Conclusion

The checked-in test source defines coverage for the 11 groups listed above. Submission status remains conditional on a successful run against the final release commit and retaining its output as evidence.
