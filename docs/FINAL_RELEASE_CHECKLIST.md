# Final Release Checklist (excluding demo videos)

## Repository and integration
- [x] Recovery implementation from PR #42 has been carried onto the current mainline in the final-evaluation-prep branch and proposed in PR #43.
- [ ] Review and merge PR #43; it is intentionally left open for repository-owner review.
- [ ] Close PR #42 as superseded only after PR #43 is approved and merged.
- [x] Create docs/BACKLOG_RECONCILIATION.md with a per-issue ready-to-close / keep-open map for the 40 open backlog issues.
- [ ] After PR #43 is merged, close verified completed issues and update the GitHub Project board.
- [x] README now documents clean build, test, run, configuration, performance and reliability workflows.
- [ ] Verify a clean clone/build/run on a second machine.
- [ ] Record the final merged commit SHA and create a release tag after sign-off.

## Requirements and design documents
- [x] Added SRS security-objectives addendum and SAD security/API/error-handling addendum.
- [x] Added an implementation-deviation register covering role control, module boundaries, persistent logging and tooling differences.
- [x] Added a revised Section 5.1 security-validation addendum.
- [ ] Integrate the addenda into the approved consolidated SRS/SAD/STP PDF; the existing PDF has not been rewritten.
- [ ] Complete the faculty checklist audit of IEEE formatting, UML diagrams, API design, error-handling protocols and full traceability matrix.
- [ ] Verify the GitHub Project board and sprint assignments against the final code.
- [x] Updated the final-evaluation test matrix to 10 grouped cases covering FR/NFR/security requirements.
- [ ] Update the original STP test cases and obtain faculty/team approval for documented deviations.

## QA evidence
- [x] Unit Test Report updated; 28 test functions pass under CTest on the CI commit.
- [x] System Validation Report updated with main simulation, emergency, incident and strategy-comparison evidence.
- [x] Code Coverage Report generated; aggregate core line coverage 97.85%.
- [x] Branch Coverage Report generated; aggregate core branch coverage 70.02% (570/814), meeting the STP 70% target. signal.cpp is 67.0% individually; confirm whether the faculty expects a per-module threshold.
- [x] cppcheck 2.19.0 static-analysis job passes.
- [x] AddressSanitizer and UndefinedBehaviorSanitizer job passes.
- [x] TC-013.1, TC-013.2 and TC-013.3 performance workloads each pass 10 repetitions on the hosted x86_64 4-CPU / 15-GiB runner.
- [x] TC-014 reliability job passes 100 seeded 300-step runs plus the all-roads-blocked stress case.
- [ ] Confirm that the hosted runner is accepted as the reference machine, or repeat NFR-001 on the team's designated desktop/laptop.
- [x] Aggregate core branch coverage meets the 70% target (70.02%); signal.cpp is 67.0% individually, so confirm whether faculty expects the threshold per module.
- [x] Implement timestamped, newline-sanitized SEC-004 logging for signal and network configuration errors; automated tests verify the log sink and injection handling.
- [ ] Run the full three-entry/no-false-entry test on the designated reference environment and attach the actual log file as evidence.
- [ ] Record one operator walkthrough demonstrating a basic run in no more than three configuration steps, and capture the steps/results.

## Presentation and viva (non-video)
- [x] Documented the architecture, API boundaries, known deviations and capacity-model assumptions.
- [x] Draft an 11-slide final presentation deck has been prepared as a separate PPTX artifact; review it and add any verified team-contribution details before presenting.
- [ ] Prepare each member's actual PR/code contribution and Agile planning explanation.
- [ ] Rehearse the live CLI demonstration on the final merged commit.
- [ ] Prepare answers on why the capacity model is simplified and why the adaptive comparison result is scenario-specific.

- [x] Latest all-green CI run: https://github.com/rujul-rj/Team11_SE/actions/runs/43695727436 (all six jobs passed).

## Excluded
Demo videos are intentionally not included in this task's completion scope.
