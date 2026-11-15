# Implementation Deviations and Known Limitations

This document records differences between the approved SRS/SAD/STP baseline and the current repository implementation. Use it during the final demo and viva; do not claim these items are implemented until they are closed.

| SRS / STP item | Current implementation | Status / explanation |
|---|---|---|
| FR-001 network setup | Network objects are created through the C++ API in the sample executable; no user-editable network topology file/parser is provided | Partial; configuration UI/file workflow remains |
| FR-003 density-based generation | LOW / MEDIUM / HIGH generation modes map to different spawn intervals; generated IDs and routes are validated | Implemented on the final-prep branch; verify latest CI |
| FR-007 emergency priority | Vehicles have NORMAL / EMERGENCY type; simulation requests priority at configured signals and clears it after passage | Implemented on the final-prep branch; verify latest CI |
| FR-008 incident capacity | Roads have base capacity and a per-road reduction factor; incidents can reduce capacity or block/unblock the road | Implemented for a single active reduction factor and full block; multiple overlapping incident objects and incident history remain |
| FR-009 / FR-010 analytics | Simulation aggregates waiting time, queue length, throughput and end-of-run summary | Implemented; summary values are included in CI evidence |
| FR-011 strategy comparison | Same deterministic network/demand/duration is run under fixed and adaptive strategies | Implemented; sample outputs currently produce identical metrics |
| Strategy comparison limitation | Road discharge is not constrained by per-road capacity, so changing signal strategy does not yet create different throughput outcomes in the sample comparison | Known model limitation; do not claim adaptive outperforms fixed-time |
| BR-03 role restriction | No user-role/authentication mechanism exists for Administrator vs Operator/Analyst | Not implemented; document as a permitted deviation or implement before claiming full SRS compliance |
| SEC-004 logging | Invalid signal configurations are retained in an in-memory vector | Partial; persistent timestamped audit logging remains |
| NFR-001 performance | CI benchmark runs 1,000 vehicle-step updates 10 times and records timing | CI proxy passes; run on the approved reference machine before sign-off |
| NFR-003 / NFR-004 architecture | Simulation currently owns incident handling and analytics aggregation; separate IncidentManager and AnalyticsEngine classes are not present | Deviation from conceptual SAD components |
| Test framework | Tests use a lightweight custom require() harness registered with CTest, not GoogleTest/GoogleMock as named in the SRS/STP | Tooling deviation; disclose or migrate |
| CI/CD | GitHub Actions builds, tests, runs sanitizers and generates coverage artifacts; the STP mentions Jenkins | Tooling deviation; GitHub Actions is the active CI system |
| Coverage exit criteria | Line coverage excluding executable entry points is above 80%; branch coverage is below the STP's 70% target | Not fully satisfied; further branch tests required |
| Usability walkthrough | CLI instructions exist; three first-time users have not been observed completing the workflow | Manual validation remains |

## Final-evaluation rule

Only mark a deviation as resolved after implementation, regression tests and evidence have all been updated. If it remains open, explain why it is acceptable for this simulation scope and identify its impact.
