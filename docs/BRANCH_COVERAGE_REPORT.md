# Branch Coverage Report

**Repository:** Team11_SE  
**Commit tested:** 269166cb1472f7c585eb0846686e85a1cf074740  
**Date:** 15 November 2026  
**Tool:** gcovr 8.8 / GCC coverage instrumentation  
**Workflow:** https://github.com/rujul-rj/Team11_SE/actions/runs/43693880691

## Result

- **Measured branch coverage excluding executable entry points:** 49.87% (397 / 796).
- **STP exit criterion:** at least 70%.
- **Status:** Not met.

## Per-file branch coverage

| Source file | Branch coverage |
|---|---:|
| Intersection.cpp | 75.0% |
| Road.cpp | 53.7% |
| Simulation.cpp | 64.0% |
| Vehicle.cpp | 36.4% |
| signal.cpp | 40.5% |
| signal.hpp | 100.0% |

## Required follow-up

Add branch tests for invalid identifiers and constructor boundaries, signal validation ranges, parser error paths, emergency queue ordering/clearing, unknown signal/road lookups, and error paths in simulation/network APIs. Re-run coverage after each test expansion. Do not state that the STP coverage exit criterion has been achieved until branch coverage reaches at least 70%.
