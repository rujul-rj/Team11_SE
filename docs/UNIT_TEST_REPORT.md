# Unit Test Report

## 1. Scope

The C++17 test executable registered with CTest contains **28 test functions**. It covers network configuration, routes, road capacity, vehicle generation, movement/queues, signal validation/control, emergency priority, end-of-run analytics, simulation boundaries and security audit logging.

## 2. Test environment

| Item | Value |
|---|---|
| Language | C++17 |
| Build system | CMake 3.16+ |
| Test executable | simulation_tests |
| Test runner | CTest |
| Test source | tests/simulation_tests.cpp |
| Compiler | GCC 15.2.0 |
| OS | GitHub-hosted Ubuntu x86_64 |
| Workflow | https://github.com/rujul-rj/Team11_SE/actions/runs/43695727436 |

## 3. Test groups

1. Network configuration parser — valid topology files, explicit/default capacities, malformed directives, duplicate/self roads, unknown endpoints and empty files.
2. Route and network validation — connected/disconnected routes, duplicate intersections/roads and unknown endpoints.
3. Vehicle movement and queues — progression, red-signal waiting, blocked-road waiting and recovery.
4. Road capacity reduction — default capacity, reduction factors, capacity-limited movement, restoration and invalid factor rejection.
5. Road constructor/state boundaries — empty IDs/endpoints, self roads, lane/capacity bounds, blocked state and non-finite factors.
6. Vehicle boundaries — invalid IDs/routes, arrival state, next intersection, zero/fractional/negative waiting durations and emergency type.
7. Simulation boundaries/facade — duplicate IDs, invalid generation routes, empty generators, ID collisions, fractional steps, signal config, road operations and emergency wrappers.
8. Invalid routes and identifiers — duplicate vehicle IDs and unknown signal/road references.
9. Constructor boundaries — zero lanes, empty identifiers, zero and non-finite simulation step.
10. End-of-run summaries — empty, short and normal runs; division-by-zero protection and finite throughput.
11. Automatic generation interval and maximum — generated IDs, configured interval, maximum count and invalid generation routes/intervals.
12. Seed reproducibility — identical seeds produce identical generated route sequences.
13. Traffic-density levels — LOW, MEDIUM and HIGH generate increasing vehicle counts over the same duration.
14. Signal configuration/parser — valid configuration, malformed directives, invalid values, duplicate declarations and undeclared intersections.
15. Signal validation branches — ID length/character rules, phase count, all-red, duplicate names/roads, timing boundaries, adaptive parameters and enum formatting.
16. Signal controller branches — green/yellow/all-red states, fixed/adaptive selection, current-approach emergency, queued emergencies, clearance order and invariants.
17. SignalManager facade — valid/invalid/duplicate configuration, empty manager, unknown lookups, strategy changes, emergency operations and queue updates.
18. Persistent security audit logging — three invalid attempts, timestamps, exact line count, persistent sink and newline-injection rejection.

## 4. Execution

Commands used:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

**Result: PASS.** On 15 November 2026, commit bc2a643ff1067350b0a949dd8233ba3db12a677f, CTest reported 1/1 test executable passed and 100% tests passed. The executable contains the 28 test functions described above. AddressSanitizer and UndefinedBehaviorSanitizer also passed.

Workflow: https://github.com/rujul-rj/Team11_SE/actions/runs/43695727436

## 5. Conclusion

The automated unit regression suite passes on the tested commit. The aggregate core line and branch coverage criteria are met. Manual three-first-time-user usability validation remains outstanding.
