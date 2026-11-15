# Software Test Plan — Revised Section 5.1: Security Validation

This addendum corrects the current STP structure. The faculty checklist requires Section 5.1 to be dedicated to security validation. In the consolidated STP currently in the repository, Section 5.1 is titled “Test Case Pass/Fail”; replace that section with this material in the final consolidated PDF.

## 5.1 Security Validation

### 5.1.1 Security objectives
- **SO-01 — Input integrity:** Prevent malformed, incomplete or out-of-range configuration data from entering simulation state.
- **SO-02 — Runtime safety and accountability:** Prevent unsafe memory behaviour in core modules and retain diagnostic records when invalid configuration is rejected.

### 5.1.2 Security requirements and traceability

| Requirement | Validation focus | Test ID |
|---|---|---|
| SEC-001 — Validate all configuration file inputs before use | Malformed, truncated, missing-field and invalid-enum inputs are rejected before application | SEC-TC-001 |
| SEC-002 — Reject values outside defined safe ranges | Negative, zero where prohibited, NaN/Infinity and out-of-range values are rejected | SEC-TC-002 |
| SEC-003 — Avoid unsafe memory operations in core modules | Static analysis, compiler warnings, AddressSanitizer and UndefinedBehaviorSanitizer | SEC-TC-003 |
| SEC-004 — Log invalid configuration attempts | Invalid attempts produce a diagnostic record containing source and reason | SEC-TC-004 |

### 5.1.3 Security test cases

| ID | Input / procedure | Expected result | Evidence / current status |
|---|---|---|---|
| SEC-TC-001 | Submit malformed signal configuration: unknown strategy, malformed integer, undeclared intersection, invalid phase or road | Entire invalid configuration is rejected; no partial configuration is applied | Automated parser/validation tests pass in CI |
| SEC-TC-002 | Submit invalid signal timings, negative queue values, zero/non-finite simulation step and invalid vehicle routes | Invalid values are rejected and valid state remains unchanged | Automated boundary tests pass in CI; density is enum-constrained |
| SEC-TC-003 | Run warning-enabled build and AddressSanitizer/UndefinedBehaviorSanitizer tests; run cppcheck or equivalent | No memory-safety findings and no unresolved critical static-analysis findings | PASS: cppcheck 2.19.0 and ASan/UBSan CI |
| SEC-TC-004 | Submit invalid signal configuration and inspect SignalManager invalid_attempt_log() | Rejection reason is recorded for review | Automated in-memory logging test passes; persistent timestamped log is not yet implemented |

### 5.1.4 Entry and exit criteria

Entry: final code builds, configuration parser and test data are available, and the test suite passes.

Exit: all four security test cases have recorded results; SEC-001/002 negative tests pass; SEC-003 has static-analysis and sanitizer evidence; SEC-004 logging evidence is retained; no unresolved critical security defect remains.

### 5.1.5 Limitations

The current invalid-attempt log is in-memory and does not yet provide a persistent timestamped audit file. Static-analysis results must be attached separately; sanitizer success alone does not satisfy the complete SEC-003 validation.
