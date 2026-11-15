# Code Coverage Report

**Repository:** Team11_SE  
**Commit tested:** 269166cb1472f7c585eb0846686e85a1cf074740  
**Date:** 15 November 2026  
**Tool:** gcovr 8.8 / GCC coverage instrumentation  
**Workflow:** https://github.com/rujul-rj/Team11_SE/actions/runs/43693880691  
**Artifact:** team11-coverage-reports (HTML detail report and Cobertura XML)

## Result

The STP exit criterion is at least 80% line coverage and 70% branch coverage for core modules. For the SRS/STP core implementation set (Road, Intersection, Vehicle, Simulation incident/analytics orchestration and signal-control module), executable entry points and configuration parser/network infrastructure are excluded.

- **Aggregate core line coverage:** 83.70% (457 / 546) — above 80%; however Road.cpp (61.5%) and signal.cpp (75.0%) are below 80% individually.
- **Branch coverage:** 49.87% (397 / 796) — does not meet the 70% target.

## Per-file line coverage

| Source file | Line coverage |
|---|---:|
| Intersection.cpp | 100.0% |
| Road.cpp | 84.6% |
| Simulation.cpp | 91.5% |
| Vehicle.cpp | 95.8% |
| signal.cpp | 75.0% |
| signal.hpp | 80.0% |

The full HTML/XML details are attached to the Actions run. Branch coverage is reported separately in BRANCH_COVERAGE_REPORT.md.
