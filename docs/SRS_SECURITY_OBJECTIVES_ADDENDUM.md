# SRS Addendum — Security Objectives

The current SRS lists four security requirements (SEC-001 to SEC-004) but does not explicitly state security objectives. Add the following subsection to the SRS Security Requirements section to satisfy the faculty checklist.

## Security objectives
1. **SO-01 — Configuration integrity:** Ensure that only complete, well-formed and in-range network and signal configuration is accepted, so invalid input cannot corrupt simulation state or produce unsafe behaviour.
2. **SO-02 — Runtime safety and auditability:** Protect core simulation operations from unsafe memory access and preserve useful diagnostics when invalid configuration is rejected.

## Requirement mapping

| Objective | Supporting SRS requirements |
|---|---|
| SO-01 — Configuration integrity | SEC-001: validate configuration inputs; SEC-002: reject values outside safe ranges |
| SO-02 — Runtime safety and auditability | SEC-003: avoid unsafe memory operations; SEC-004: log invalid configuration attempts |

This addendum clarifies the intent of the existing security requirements; it does not introduce external authentication, network security or real traffic-control capabilities, which remain outside the documented standalone simulation scope.
