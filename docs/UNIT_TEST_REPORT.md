# Unit Test Report

## 1. Scope

This report inventories the dependency-free C++ test executable registered with CTest. The suite contains 21 test functions covering network and route validation, vehicle movement, queue behaviour, traffic-density generation, signal configuration, fixed-time and adaptive control, emergency controller behaviour, integrated emergency-vehicle priority, incident recovery and simulation summaries.

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

1. Network configuration parser — valid topology files, explicit/default capacities, malformed directives, duplicate/self roads, unknown endpoints and empty files.
2. Route and network validation — connected/disconnected routes, duplicate intersections/roads and unknown endpoints.
3. Vehicle movement and queues — progression, red-signal waiting, blocked-road waiting and recovery.
4. Invalid routes and identifiers — duplicate vehicle IDs and unknown signal/road references.
5. Constructor boundaries — zero lanes, empty identifiers, zero and non-finite simulation step.
6. End-of-run summaries — empty, short and normal runs; division-by-zero protection and finite throughput.
7. Automatic generation interval and maximum — generated IDs, configured interval, maximum count and invalid generation routes/intervals.
8. Road capacity reduction — default lane capacity, factor range validation, capacity-limited movement and restoration.
9. Traffic-density levels — LOW, MEDIUM and HIGH generate increasing vehicle counts over the same duration.
10. Signal configuration and parser — valid configuration, invalid timings, malformed values and undeclared intersections.
11. Fixed-time and adaptive controllers — safe phase sequence, queue-sensitive extension and invariants.
12. Emergency controller and integrated vehicle priority — emergency clearance sequence and arrival at destination.
13. Integration and invalid-configuration logging — network/controller integration and rejected configuration diagnostics.

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
