# Unit Test Report

## 1. Scope

The C++17 test executable registered with CTest contains **21 test functions**. It covers network configuration, routes, road capacity, vehicle generation, movement/queues, signal control, emergency priority, end-of-run analytics and security audit logging.

## 2. Test environment

| Item | Value |
|---|---|
| Language | C++17 |
| Build system | CMake 3.16+ |
| Test executable | simulation_tests |
| Test runner | CTest |
| Test source | tests/simulation_tests.cpp |
| CI | GitHub Actions |
| Compiler | GCC 15.2.0 |
| OS | GitHub-hosted Ubuntu x86_64 |

## 3. Test groups

1. Network configuration parser — valid topology files, explicit/default capacities, malformed directives, duplicate/self roads, unknown endpoints and empty files.
2. Route and network validation — connected/disconnected routes, duplicate intersections/roads and unknown endpoints.
3. Vehicle movement and queues — progression, red-signal waiting, blocked-road waiting and recovery.
4. Road capacity reduction — default capacity, reduction factors, capacity-limited movement, restoration and invalid factor rejection.
5. Invalid routes and identifiers — duplicate vehicle IDs and unknown signal/road references.
6. Constructor boundaries — zero lanes, empty identifiers, zero and non-finite simulation step.
7. End-of-run summaries — empty, short and normal runs; division-by-zero protection and finite throughput.
8. Automatic generation interval and maximum — generated IDs, configured interval, maximum count and invalid generation routes/intervals.
9. Seed reproducibility — identical seeds produce identical generated route sequences.
10. Traffic-density levels — LOW, MEDIUM and HIGH generate increasing vehicle counts over the same duration.
11. Signal configuration and parser — valid configuration, invalid timings, malformed values and undeclared intersections.
12. Fixed-time and adaptive controllers — safe phase sequence, queue-sensitive extension and invariants.
13. Emergency controller and integrated vehicle priority — emergency clearance sequence and arrival at destination.
14. Persistent security audit logging — three invalid attempts, timestamps, exact line count, persistent sink and newline-injection rejection.

## 4. Execution

Commands used:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

**Result: PASS.** On 15 November 2026, commit a72050a2ad3c52c45bd4e3a7f838ff11369e6a09, CTest reported 1/1 test executable passed and 100% tests passed. The executable contains the 21 test functions described above. AddressSanitizer and UndefinedBehaviorSanitizer also passed.

Workflow: https://github.com/rujul-rj/Team11_SE/actions/runs/43694337930

## 5. Conclusion

The automated unit regression suite passes on the tested commit. This result does not imply that every STP system/usability case is complete; see the system validation and implementation-deviation reports.
