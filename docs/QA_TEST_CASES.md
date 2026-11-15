# Updated Test Case Matrix — Team 11 Traffic Management System

This is the concise final-evaluation matrix (10 grouped cases) derived from the approved SRS and STP. The detailed STP retains the TC-001 to TC-024 identifiers; this matrix groups the most important functional, non-functional and security checks for execution evidence.

| Case | SRS traceability | Scenario / test data | Expected result | Current evidence |
|---|---|---|---|---|
| TC-001 — Network and route validation | FR-001, FR-012, SEC-001 | Build a valid network; attempt duplicate intersections/roads, unknown endpoints and disconnected routes | Valid topology is accepted; invalid topology/routes are rejected descriptively | PASS in automated regression; network is assembled through C++ API, not an operator-editable network file |
| TC-002 — Signal configuration and fixed-time control | FR-002, FR-005, SEC-001/002 | Parse valid and malformed signal files; advance fixed-time phases through GREEN, YELLOW and ALL_RED | Valid config installs; invalid config is rejected; phase transitions preserve safety invariants | PASS in automated regression |
| TC-003 — Density-based traffic generation | FR-003 | Run LOW, MEDIUM and HIGH density for the same duration and compare generated counts; test maximum count and invalid route/interval | Higher density generates more vehicles; IDs/routes are valid; configured maximum is respected | PASS in automated regression on final-prep branch |
| TC-004 — Vehicle movement and queues | FR-004, FR-009, BR-05 | Move vehicles along connected routes; stop them at RED and blocked roads; release after signal/incident clears | Vehicles advance one road per step; waiting time increases only while stopped; normal vehicles do not cross RED | PASS in automated regression |
| TC-005 — Adaptive signal strategy | FR-006, BR-04 | Provide high and low queue measurements; verify minimum/maximum green bounds and current-run queue use | Adaptive timing responds to queue data and preserves safe phase transitions | PASS in controller regression; strategy comparison metrics remain identical in current simplified workload |
| TC-006 — Emergency vehicle priority | FR-007, BR-01 | Introduce an EMERGENCY vehicle approaching a non-green configured approach | Controller uses yellow/all-red clearance, grants the emergency approach green and resumes normal operation after passage | PASS in integrated emergency-vehicle regression on final-prep branch |
| TC-007 — Traffic incident | FR-008, BR-02 | Block an existing road, verify vehicles queue, clear the incident; attempt unknown road | Existing road can be blocked/unblocked; unknown road is rejected | PASS for block/unblock; fractional capacity reduction is not implemented |
| TC-008 — Metrics and end-of-run summary | FR-009, FR-010 | Run empty, short and normal scenarios; compare summary fields with expected counts | Summary reports vehicles, waiting time, queue length and throughput; empty-run averages remain finite | PASS in automated regression |
| TC-009 — Fixed/adaptive strategy comparison | FR-011 | Execute the same network, demand and duration under both strategies | Both runs complete and metrics are displayed side by side | PASS in CI; sample metrics are identical because road discharge is not capacity-constrained |
| TC-010 — Non-functional and security validation | NFR-001 to NFR-005, SEC-001 to SEC-004 | 1,000 vehicle-step performance runs; repeated runs; sanitizers/static analysis; negative configuration tests; first-time-user walkthrough | Performance ≤2.0 seconds per run on reference machine; valid runs do not crash; safe inputs rejected; logs retained; basic run in ≤3 steps | All three CI performance scenarios, cppcheck 2.19.0 and ASan/UBSan pass; reference-machine sign-off, persistent logging and 3-user usability walkthrough remain |

## Final execution record

- Final branch: final-evaluation-prep
- Latest validated commit at time of report: b0cd2d7c2e15beffa5dd71534432734ab897ce9f
- Automated build, CTest, sanitizer, repeat-run and baseline performance jobs: [GitHub Actions run 43692880777](https://github.com/rujul-rj/Team11_SE/actions/runs/43692880777)
- The current PR continues to evolve; rerun the full matrix against the final merged commit and update the commit SHA before submission.
