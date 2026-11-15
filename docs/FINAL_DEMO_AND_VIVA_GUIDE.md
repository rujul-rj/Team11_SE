# Final Demo and Viva Guide

This guide is for the live application demonstration and oral evaluation. Demo videos are intentionally excluded.

## Suggested 8–10 minute live demo

| Time | Demonstration | What to explain |
|---|---|---|
| 0:00–0:45 | Problem and objective | A configurable C++ traffic simulation for comparing signal strategies and handling traffic conditions; it is not a real traffic controller |
| 0:45–1:45 | Architecture | Road, Intersection, Vehicle and TrafficNetwork domain model; Simulation orchestration; signal-control subsystem; configuration parser and command-line executables |
| 1:45–2:30 | Configuration | Open config/network_example.cfg and config/signals_example.cfg; explain validation and how malformed configuration is rejected |
| 2:30–4:30 | Main simulation | Run ./build/traffic_sim; explain LOW/MEDIUM/HIGH generation, seeded route selection, normal/emergency vehicle types, signal phases, capacity-reduction incident and summary metrics |
| 4:30–6:00 | Strategy comparison | Run ./build/strategy_comparison; show identical network and 22-vehicle demand under both strategies |
| 6:00–7:30 | QA evidence | Show CTest, static-analysis, sanitizer, performance and reliability workflow runs; explain line/branch coverage honestly |
| 7:30–8:30 | Limitations | Explain simplified per-road capacity, lack of role enforcement, missing separate IncidentManager/AnalyticsEngine classes and the outstanding usability walkthrough |
| 8:30–10:00 | Wrap-up and questions | Summarize requirements satisfied, measured evidence and future work |

## Exact comparison result to explain

For the current comparison scenario (20 vehicles approaching A→B, 2 approaching A→C, 30 one-second steps):

- Both fixed-time and adaptive completed all 22 vehicles.
- Fixed-time: total waiting time 216 vehicle-seconds; average queue 7.2 vehicles/step.
- Adaptive: total waiting time 156 vehicle-seconds; average queue 5.2 vehicles/step.
- Throughput: 0.733 vehicles/second for both.

Interpretation: adaptive control reduced waiting time and average queue for this unbalanced-demand test case. This does not prove adaptive control is universally superior; results depend on demand and the simplified capacity assumptions.

## QA evidence to show

- Unit regression: 28 test functions in one CTest executable; latest successful workflow run is linked from the Unit Test Report.
- Static analysis: cppcheck warning/performance/portability checks.
- Runtime safety: AddressSanitizer and UndefinedBehaviorSanitizer.
- Performance: TC-013.1 baseline, TC-013.2 adaptive and TC-013.3 incidents+emergency; each runs 1,000 vehicle-step updates 10 times.
- Reliability: 100 different seeds, 300 steps per run, plus all-roads-blocked stress scenario.
- Coverage: aggregate core line coverage is above 80%; aggregate branch coverage is 70.02%, meeting the STP 70% target; signal.cpp is 67.0% individually, so explain the aggregate-versus-per-module interpretation.

## Viva preparation

Be ready to explain:

1. Why the application uses C++17, CMake and a modular domain model.
2. How the network configuration parser rejects duplicate IDs, unknown endpoints, invalid lanes and capacities.
3. How signal phases transition safely through GREEN, YELLOW and ALL_RED.
4. How adaptive signal timing uses current-run queue measurements.
5. How an emergency vehicle requests priority and why it still waits for safe clearance.
6. How per-road capacity and a reduction factor affect queueing.
7. How the summary computes waiting time, average queue and throughput.
8. How seeded route generation makes runs reproducible.
9. What CTest, cppcheck, ASan/UBSan, gcovr and the performance/reliability executables each validate.
10. How branch coverage is measured; the aggregate core target is met, while signal.cpp is 67.0% individually.
11. Which SRS/SAD items remain deviations: role enforcement, separate incident/analytics modules, persistent log retention policy and first-time-user usability evidence.

## Team contribution evidence

Use GitHub PRs and the actual commits as the source of truth. The currently visible PR history shows:

- PR #41 — authored by prohan1287; merged to main; title: “Implement traffic simulation and signal control strategies”.
- PR #42 — authored by ajjaiahm; still open and based on feature/core-simulation; title: “Sprint recovery: validation, simulation summaries and strategy comparison”.
- PR #43 — authored by rujul-rj; final-evaluation-prep branch; integrates recovery work and QA documentation.

Before the viva, each member should identify their own exact files/functions and commit/PR evidence. The repository PR list does not show a separate PR for every team member; do not assign unverified code ownership to a member.

## Outstanding before final sign-off

- Confirm whether the faculty applies the 70% branch threshold to the aggregate core set or to each core file; signal.cpp needs 14 additional branch outcomes to reach 70% individually.
- Confirm the hosted runner is accepted as the designated reference machine or repeat the performance test on the team's desktop/laptop.
- Complete the three-first-time-user usability walkthrough.
- Integrate the SRS/SAD/STP addenda into the approved consolidated PDF.
- Complete repository-owner review and merge PR #43; only then reconcile/close superseded PR #42 and backlog issues.
