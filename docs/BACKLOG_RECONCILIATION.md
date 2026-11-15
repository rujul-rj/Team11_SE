# GitHub Backlog Reconciliation — Final Evaluation

**Snapshot:** 15 November 2026  
**Repository:** rujul-rj/Team11_SE  
**Current main merge:** PR #43 merged as `0111898c3cf2b7437f27f976ef3818b13e88dabf`  
**Superseded PR:** #42 closed after confirming its recovery work is included in PR #43  
**Backlog state:** 30 issues closed; 10 remain open.

## Closed after PR #43 was merged

**Completed implementation/evidence issues:** #3, #14, #15, #16, #17, #18, #19, #20, #22, #23, #24, #25, #26, #29, #30, #32, #33, #34, #36, #37, #38, #39.

These cover the network model, signal configuration, generation/routes, movement/queues, fixed/adaptive control, emergency priority, incidents, metrics, summaries, strategy comparison, reliability, input/memory safety, diagnostic logging, traceability and final validation. Each issue was closed with the merged PR #43 commit and current CI evidence.

**Duplicate issues closed with a canonical reference:** #4 -> #23, #5 -> #24, #6 -> #25, #7 -> #26, #8 -> #27, #9 -> #28, #10 -> #29, #11 -> #30.

## Remaining open issues

| Issue | Backlog item | Why it remains open |
|---|---|---|
| #1 | Sprint 1 epic | Sprint 1 video evidence is still outstanding (#13/#40). |
| #2 | Sprint 2 epic | Sprint 2 video and formal freeze/tag remain outstanding (#35). |
| #12 | PB-10 CMake, GoogleTest and CI baseline | CMake and CI pass; the repository uses a custom test harness rather than the GoogleTest dependency named in the backlog. Resolve or approve the framework deviation. |
| #13 | PB-11 Sprint 1 working-product video | Demo video excluded from this task. |
| #21 | PB-19 performance validation | CI benchmark passes on a runner meeting the minimum compute spec; team must confirm the designated reference machine or rerun there. |
| #27 | PB-06 modular/testable components | Road, Intersection, Vehicle, TrafficNetwork and signal control are modular; separate IncidentManager and AnalyticsEngine classes are not present. |
| #28 | PB-07 simple build/run workflow (NFR-005) | README documents the three-step path; one operator walkthrough still needs to be recorded to verify a basic run in no more than three configuration steps. |
| #31 | PB-21 independent component testability | Core components have isolated tests, but mocks/stubs are not used; confirm whether the evaluator requires them. |
| #35 | PB-25 Sprint 2 demo/docs/freeze | Documentation is updated; video and formal release freeze/tag remain. |
| #40 | PB-30 Sprint 1 demo evidence/README | README setup guide is complete; video evidence remains excluded. |

## Duplicate issue families

The original and recovery backlog contained duplicate PB-02 through PB-09 issues. The older copies were closed with duplicate references:
- PB-02: #4 closed as duplicate of #23 (canonical issue completed).
- PB-03: #5 closed as duplicate of #24 (canonical issue completed).
- PB-04: #6 closed as duplicate of #25 (canonical issue completed).
- PB-05: #7 closed as duplicate of #26 (canonical issue completed).
- PB-06: #8 closed as duplicate of #27 (canonical issue remains open for modularity work).
- PB-07: #9 closed as duplicate of #28 (canonical issue remains open for operator walkthrough).
- PB-08: #10 closed as duplicate of #29 (canonical issue completed).
- PB-09: #11 closed as duplicate of #30 (canonical issue completed).

## Remaining closeout sequence

1. Record the one-operator NFR-005 walkthrough (basic run in <=3 configuration steps).
2. Confirm the designated performance reference machine.
3. Resolve or formally approve the GoogleTest/mocks-stubs and separate-module deviations.
4. Complete the SRS/SAD/STP PDF repository upload and confirm any per-module branch-coverage interpretation (aggregate core branch coverage is 70.02%; signal.cpp is 67.0% individually).
5. Record demo videos, identify the freeze commit/tag, and close the two sprint epics when their child evidence is complete.
6. Update the GitHub Project board to reflect the 10 remaining open issues and closed work.

The project board itself was not independently updated in this pass.
