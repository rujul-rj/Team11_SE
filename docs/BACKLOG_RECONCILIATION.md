# GitHub Backlog Reconciliation — Final Evaluation

**Snapshot:** 15 November 2026  
**Repository:** rujul-rj/Team11_SE  
**Open backlog:** 40 issues; open PRs #42 and #43  
**Integration dependency:** PR #43 is mergeable and all six CI jobs pass, but it remains open for repository-owner review. Do not close implementation issues until PR #43 is merged.

## Ready to close after PR #43 is merged and owner verifies the result

| Issue | Backlog item | Current evidence |
|---|---|---|
| #3 | PB-01 road/lane/intersection model | Network model plus validated network configuration parser |
| #4, #23 | PB-02 signal configuration | config/signals_example.cfg and parser/controller tests |
| #5, #24 | PB-03 vehicle generation/routes | LOW/MEDIUM/HIGH generation, bounds, route validation and seeded route selection |
| #6, #25 | PB-04 movement/queues | Movement, red-signal, blocked-road and capacity queue tests |
| #7, #26 | PB-05 invalid network validation | Network parser rejects malformed directives, unknown endpoints, duplicate/self roads and invalid lanes/capacities |
| #9 | PB-07 simple build/run workflow | README has clean configure/build/test/run instructions |
| #10, #29 | PB-08 malformed configuration validation | Network and signal parser negative tests |
| #11, #30 | PB-09 range/invalid-value validation | Timing, capacity factor, route and constructor boundary tests |
| #14 | PB-12 fixed-time signal strategy | Fixed-time controller tests and signal config file |
| #15 | PB-13 adaptive signal strategy | Queue-responsive controller tests and comparison executable |
| #16 | PB-14 emergency priority | Integrated emergency-vehicle signal-priority regression |
| #17 | PB-15 traffic incident handling | Per-road capacity reduction and block/unblock behaviour |
| #18 | PB-16 simulation metrics | Waiting time, queue length and throughput aggregation |
| #19 | PB-17 end-of-run summary | Empty, short and normal summary tests |
| #20 | PB-18 strategy comparison | Same capacity-constrained demand; measured fixed/adaptive comparison |
| #22 | PB-20 repeated-run reliability | 100 seeded 300-step runs plus all-roads-blocked stress |
| #32 | PB-22 memory/input safety | cppcheck, ASan/UBSan and invalid-input tests |
| #33 | PB-23 diagnostic logging | Timestamped, newline-sanitized persistent configuration audit log |
| #36 | PB-26 core simulation integration | Runnable main simulation and strategy comparison |
| #37 | PB-27 configuration/boundary recovery | Network parser, signal parser, capacity and constructor boundary tests |
| #38 | PB-28 metrics/summary/comparison | Reports and comparison executable updated |
| #39 | PB-29 reliability/performance/memory safety | Three performance scenarios, 100-run reliability, cppcheck and sanitizers |

## Keep open until the remaining acceptance criteria are completed

| Issue | Backlog item | Remaining work |
|---|---|---|
| #1 | Sprint 1 epic | Close only after child issues are reconciled and video evidence policy is agreed |
| #2 | Sprint 2 epic | Close only after child issues and final release items are reconciled |
| #8, #27 | PB-06 modular/testable components | Separate IncidentManager and AnalyticsEngine classes are not present; custom harness is used instead of independent mocks/stubs |
| #12 | PB-10 CMake/GoogleTest/CI | CMake and CI are implemented; GoogleTest/GoogleMock has not been adopted |
| #13 | PB-11 Sprint 1 video | Demo video excluded from this task |
| #21 | PB-19 performance validation | CI runner meets x86_64 / 4 CPU / 15 GB compute baseline; confirm acceptance or rerun on designated team desktop/laptop |
| #28 | PB-07 simple build/run workflow | README is complete; three-first-time-user usability walkthrough is outstanding |
| #31 | PB-21 independent component testability | Add isolated component tests/mocks and improve branch coverage |
| #34 | PB-24 traceability/regression/final validation | Branch coverage is 51.23% vs 70% target; integrate SRS/SAD/STP addenda into consolidated PDF |
| #35 | PB-25 Sprint 2 demo/docs/freeze | Documentation is updated; video and release freeze/tag remain |
| #40 | PB-30 Sprint 1 demo evidence/README | README setup guide is complete; video evidence excluded |

## Duplicate issue families

The original and recovery backlog contains duplicates for PB-02 through PB-09:
- PB-02: #4 and #23
- PB-03: #5 and #24
- PB-04: #6 and #25
- PB-05: #7 and #26
- PB-06: #8 and #27
- PB-07: #9 and #28
- PB-08: #10 and #29
- PB-09: #11 and #30

When closing, retain one canonical issue per backlog item and close the duplicate with a reference to the canonical issue. Do not bulk-close both copies without recording the relationship.

## Safe closeout sequence

1. Repository owner reviews and merges PR #43.
2. Close PR #42 as superseded by PR #43 after confirming the merged tree contains its recovery work.
3. Close only the “ready to close” issues above, linking each to PR #43 and the relevant report/test evidence.
4. Keep partial and video-dependent issues open.
5. Update the GitHub Project board and sprint epics to match the final issue states.
6. Create a release tag only after the branch-coverage decision, reference-machine decision and consolidated PDF are signed off.
