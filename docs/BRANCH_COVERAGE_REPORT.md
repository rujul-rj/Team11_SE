# Branch Coverage Report

**Repository:** Team11_SE  
**Commit tested:** bc2a643ff1067350b0a949dd8233ba3db12a677f  
**Date:** 15 November 2026  
**Tool:** gcovr 8.8 / GCC coverage instrumentation  
**Workflow:** https://github.com/rujul-rj/Team11_SE/actions/runs/43695727436

## Result

- **Measured branch coverage for the SRS/STP core implementation set:** 70.02% (570 / 814).
- **STP exit criterion:** at least 70%.
- **Aggregate status:** PASS.

## Per-file branch coverage

| Source file | Branch coverage |
|---|---:|
| Intersection.cpp | 75.0% |
| Road.cpp | 83.3% |
| Simulation.cpp | 71.3% |
| Vehicle.cpp | 81.8% |
| signal.cpp | 67.0% |
| signal.hpp | 100.0% |

The aggregate threshold is met. The signal.cpp module is 67.0%; if the faculty expects every core file individually to meet 70%, add tests for at least 14 more signal.cpp branch outcomes.

## Branch-focused tests added

The suite now exercises signal validation boundaries, enum string mappings, fixed/adaptive state transitions, current-approach and queued emergency requests, manager facade error paths, malformed parser directives, road constructor boundaries, vehicle arrival/wait boundaries and simulation configuration/generation edge cases.

## Evidence

The coverage HTML report and Cobertura XML are available in the `team11-coverage-reports` artifact for workflow run 43695727436.
