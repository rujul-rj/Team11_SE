# Final Release Checklist (excluding demo videos)

## Repository and integration
- [x] Recovery implementation and final QA changes merged into main through PR #43.
- [x] PR #43 reviewed against the final QA evidence and merged into main.
- [x] PR #42 closed as superseded after verifying its recovery work is included in PR #43.
- [x] Update docs/BACKLOG_RECONCILIATION.md; all 9 issues from the 15 Nov snapshot reconciled and closed for non-video scope.
- [x] Close 30 completed/duplicate issues after PR #43 merge.
- [ ] Review/update the GitHub Project board after issue closeout (board state is not available through this repository workflow).
- [x] README now documents clean build, test, run, configuration, performance and reliability workflows.
- [ ] CI clean-checkout configure/build/test/run passed in workflow #43704321250; separate human second-machine walkthrough remains unrecorded.
- [x] Record final merge commit: 0111898c3cf2b7437f27f976ef3818b13e88dabf.
- [ ] Freeze point recorded at commit aa14d7b7cdb7e16c36fdb3089ffa565d4c61d9fb; a Git tag was not created because the available GitHub connection does not expose tag creation.

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
- [ ] Repeatable NFR-001 benchmark passed 10 repetitions per workload on the hosted runner; team designation of a reference machine is not asserted.
- [x] Aggregate core branch coverage meets the 70% target (70.02%); signal.cpp is 67.0% individually, so confirm whether faculty expects the threshold per module.
- [x] Implement timestamped, newline-sanitized SEC-004 logging for signal and network configuration errors; automated tests verify the log sink and injection handling.
- [ ] Run the full three-entry/no-false-entry test on the designated reference environment and attach the actual log file as evidence.
- [ ] README documents the exact build/test/run workflow and CI verifies the commands; a separately recorded human operator walkthrough is not claimed.

## Presentation and viva (non-video)
- [x] Documented the architecture, API boundaries, known deviations and capacity-model assumptions.
- [x] Draft an 11-slide final presentation deck has been prepared as a separate PPTX artifact; review it and add any verified team-contribution details before presenting.
- [ ] Prepare each member's actual PR/code contribution and Agile planning explanation.
- [ ] Rehearse the live CLI demonstration on the final merged commit.
- [ ] Prepare answers on why the capacity model is simplified and why the adaptive comparison result is scenario-specific.

- [x] Latest all-green CI run: https://github.com/rujul-rj/Team11_SE/actions/runs/43695727436 (all six jobs passed).

## Excluded / not claimed
Demo videos are intentionally not included in this task's completion scope. The hosted CI runner is not asserted to be the team's designated reference machine; a separate human operator walkthrough and Git tag are not claimed.
