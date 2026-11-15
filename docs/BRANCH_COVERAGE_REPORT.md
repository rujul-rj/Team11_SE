# Branch Coverage Report

**Repository:** Team11_SE  
**Commit tested:** 63b62e8d5a9f7e20f23687ce35d35aad7b8d8a08  
**Date:** 15 November 2026  
**Tool:** gcovr 8.8 / GCC coverage instrumentation  
**Workflow:** https://github.com/rujul-rj/Team11_SE/actions/runs/43693424990

## Result

- **Measured branch coverage excluding executable entry points:** 48.73% (385 / 790).
- **STP exit criterion:** at least 70%.
- **Status:** Not met.

## Per-file branch coverage

| Source file | Branch coverage |
|---|---:|
| Intersection.cpp | 75.0% |
| Road.cpp | 37.0% |
| Simulation.cpp | 64.3% |
| Vehicle.cpp | 36.4% |
| signal.cpp | 40.5% |
| signal.hpp | 100.0% |

## Required follow-up

Add branch tests for invalid identifiers and constructor boundaries, signal validation ranges, parser error paths, emergency queue ordering/clearing, unknown signal/road lookups, and error paths in simulation/network APIs. Re-run coverage after each test expansion. Do not state that the STP coverage exit criterion has been achieved until branch coverage reaches at least 70%.
