# QA Test Case Matrix — Team 11 Traffic Management System

**Build:** C++17 / CMake  
**Execution status:** Test definitions reviewed against the current automated test suite. Execute on the final merged commit and record the run URL before submission.

| ID | Requirement | Test procedure | Expected result | Automated coverage |
|---|---|---|---|---|
| TC-01 | FR-001 / FR-003 | Create a network with connected intersections and roads; add a vehicle with a connected route | Network accepts valid topology and route | Yes |
| TC-02 | FR-012 | Attempt empty, disconnected and unknown-intersection routes | Invalid routes are rejected before entering simulation state | Yes |
| TC-03 | SEC-001 / SEC-002 | Add duplicate intersection/road/vehicle IDs, zero lanes, empty IDs and unknown endpoints | Invalid identifiers and boundary values are rejected | Yes |
| TC-04 | SEC-001 | Parse valid signal configuration, then malformed strategy, non-integer timing and undeclared intersection | Valid configuration parses; malformed configuration is rejected with diagnostics | Yes |
| TC-05 | FR-005 | Advance fixed-time controller through green, yellow and all-red phases | Phase transitions follow configured timing and maintain invariants | Yes |
| TC-06 | FR-006 | Apply high and low queue lengths to adaptive controller and advance time | High queue extends green within bounds; low queue permits phase transition | Yes |
| TC-07 | FR-007 | Request emergency priority for an approach and advance controller | Safe yellow/all-red transition occurs before emergency approach receives green | Yes |
| TC-08 | FR-008 | Block a road while a vehicle approaches, then unblock it | Vehicle queues while blocked and resumes movement after incident clearance | Yes |
| TC-09 | FR-009 / FR-010 | Check empty, one-step and normal simulation summaries | Counts and metrics are finite; zero-denominator cases return zero | Yes |
| TC-10 | NFR-001 / NFR-002 / SEC-003 | Run both executables repeatedly; run tests with sanitizers and coverage instrumentation | No crashes or sanitizer findings; coverage report records line and branch data | CI configured; final run pending |

## Execution record

For each final run, record:
- Commit SHA and branch
- Date/time and operating system
- Test command and complete output
- Pass/fail count
- GitHub Actions run URL
- Defect ID and retest result for every failure

Do not mark a case passed solely because an automated test exists; attach the final commit's successful test run.
