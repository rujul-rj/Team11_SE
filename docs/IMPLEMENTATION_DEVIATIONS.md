# Implementation Deviations and Known Limitations

This document records differences between the approved SRS/SAD/STP baseline and the current repository implementation. Use it during the final demo and viva; do not claim these items are implemented until they are closed.

| SRS / STP item | Current implementation | Status / explanation |
|---|---|---|
| FR-001 network setup | Network topology is loaded from config/network_example.cfg; signal phases from config/signals_example.cfg | Implemented for file-based setup; no interactive GUI editor |
| FR-003 density-based generation | LOW / MEDIUM / HIGH modes map to spawn intervals; route selection is reproducible from a configurable seed; generated IDs and routes are validated | Implemented on the final-prep branch; verify latest CI |
| FR-007 emergency priority | Vehicles have NORMAL / EMERGENCY type; simulation requests priority at configured signals and clears it after passage | Implemented on the final-prep branch; verify latest CI |
| FR-008 incident capacity | Roads have base capacity and a per-road reduction factor; incidents can reduce capacity or block/unblock the road | Implemented for a single active reduction factor and full block; multiple overlapping incident objects and incident history remain |
| FR-009 / FR-010 analytics | Simulation aggregates waiting time, queue length, throughput and end-of-run summary | Implemented; summary values are included in CI evidence |
| FR-011 strategy comparison | Same capacity-constrained network and demand are run under fixed and adaptive strategies | Implemented; tested unbalanced-demand scenario produced lower waiting time and queue under adaptive; results are scenario-specific |
| Strategy comparison limitation | Capacity is a simplified per-road per-step limit (default 20 vehicles per lane per step); the model is not calibrated to real traffic | Document the assumption; comparison results are scenario-specific, not a universal claim that adaptive always wins |
| BR-03 role restriction | No user-role/authentication mechanism exists for Administrator vs Operator/Analyst | Not implemented; document as a permitted deviation or implement before claiming full SRS compliance |
| SEC-004 logging | Invalid signal configurations are retained in an in-memory vector | Partial; persistent timestamped audit logging remains |
| NFR-001 performance | CI benchmark runs TC-013.1, TC-013.2 and TC-013.3 with 1,000 vehicle-step updates and 10 repetitions each | CI runner meets the x86-64 / 4 CPU / 8 GB compute baseline; confirm designated reference-machine identity |
| NFR-002 reliability | 100 seeded runs of 300 steps plus all-roads-blocked stress scenario are executed in CI | Verify latest reliability job and retain evidence |\n| NFR-003 / NFR-004 architecture | Simulation currently owns incident handling and analytics aggregation; separate IncidentManager and AnalyticsEngine classes are not present | Deviation from conceptual SAD components |
| Test framework | Tests use a lightweight custom require() harness registered with CTest, not GoogleTest/GoogleMock as named in the SRS/STP | Tooling deviation; disclose or migrate |
| CI/CD | GitHub Actions builds, tests, runs sanitizers and generates coverage artifacts; the STP mentions Jenkins | Tooling deviation; GitHub Actions is the active CI system |
| Coverage exit criteria | Line coverage excluding executable entry points is above 80%; branch coverage is below the STP's 70% target | Not fully satisfied; further branch tests required |
| Usability walkthrough | CLI instructions exist; three first-time users have not been observed completing the workflow | Manual validation remains |

## Final-evaluation rule

Only mark a deviation as resolved after implementation, regression tests and evidence have all been updated. If it remains open, explain why it is acceptable for this simulation scope and identify its impact.
