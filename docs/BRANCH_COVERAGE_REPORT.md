# Branch Coverage Report

**Repository:** Team11_SE  
**Commit tested:** b0cd2d7c2e15beffa5dd71534432734ab897ce9f  
**Date:** 15 November 2026  
**Tool:** gcovr 8.8 / GCC coverage instrumentation  
**Workflow:** https://github.com/rujul-rj/Team11_SE/actions/runs/43692880777

## Result

- **Measured branch coverage excluding executable entry points:** 47.71% (499 / 1,046).
- **STP exit criterion:** at least 70%.
- **Status:** Not met.

## Per-file branch coverage

| Source file | Branch coverage |
|---|---:|
| Intersection.cpp | 75.0% |
| Road.cpp | 44.4% |
| Simulation.cpp | 66.5% |
| TrafficNetwork.cpp | 80.4% |
| Vehicle.cpp | 36.4% |
| signal.cpp | 40.5% |
| signal.hpp | 100.0% |
| signal_config_parser.cpp | 35.1% |

## Required follow-up

Add branch tests for invalid identifiers and constructor boundaries, signal validation ranges, parser error paths, emergency queue ordering/clearing, unknown signal/road lookups, and error paths in simulation/network APIs. Re-run coverage after each test expansion. Do not state that the STP coverage exit criterion has been achieved until branch coverage reaches at least 70%.
