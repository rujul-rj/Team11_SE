# Final Release Checklist (excluding demo videos)

## Repository and integration
- [ ] Review and merge the final-evaluation-prep PR after CI passes.
- [ ] Confirm PR #42 recovery work is integrated; do not force-merge the diverged branch.
- [ ] Reconcile backlog issues with the final code and close only verified completed items.
- [ ] Confirm clean clone/build/test/run instructions work on a second machine.
- [ ] Record the final commit SHA and create a release tag after sign-off.

## Requirements and design documents
- [ ] Review approved SRS, SAD and Test Plan against the faculty checklist.
- [ ] Confirm IEEE formatting, functional/non-functional requirements, security objectives/requirements, architecture and UML diagrams, API design, error handling and traceability.
- [ ] Confirm Test Plan Section 5.1 is dedicated to security validation.
- [ ] Confirm 7–10 or more test cases cover functional and non-functional requirements.
- [ ] Document every approved deviation from SRS/SAD.

## QA evidence
- [ ] Update the original test cases for sprint changes.
- [ ] Attach successful final-commit Unit Test Report.
- [ ] Execute and sign off System Validation Report.
- [ ] Generate and attach Code Coverage Report.
- [ ] Generate and attach Branch Coverage Report.
- [ ] Measure the SRS performance target on the specified workload/environment.
- [ ] Record sanitizer and repeated-run results.

## Presentation and viva (non-video)
- [ ] Prepare architecture and component explanation.
- [ ] Prepare requirement-to-test traceability explanation.
- [ ] Prepare explanation of fixed-time, adaptive and emergency priority logic.
- [ ] Prepare known limitations and design trade-offs, especially the current road-capacity limitation in strategy comparison.
- [ ] Prepare each member's PR/code contribution and Agile planning explanation.
- [ ] Rehearse the live CLI demonstration.

## Excluded
Demo videos are intentionally not included in this checklist's completion scope.
