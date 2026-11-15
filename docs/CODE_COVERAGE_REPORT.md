# Code Coverage Report

**Repository:** Team11_SE  
**Commit tested:** a72050a2ad3c52c45bd4e3a7f838ff11369e6a09  
**Date:** 15 November 2026  
**Tool:** gcovr 8.8 / GCC coverage instrumentation  
**Workflow:** https://github.com/rujul-rj/Team11_SE/actions/runs/43694337930  
**Artifact:** team11-coverage-reports (HTML detail report and Cobertura XML)

## Result

The STP exit criterion is at least 80% line coverage and 70% branch coverage for core modules. For the SRS/STP core implementation set (Road, Intersection, Vehicle, Simulation incident/analytics orchestration and signal-control module), executable entry points and configuration parser/network infrastructure are excluded.

- **Aggregate core line coverage:** 84.32% (468 / 555) — above 80%; however signal.cpp (76.6%) are below 80% individually.
- **Branch coverage:** 51.23% (417 / 814) — does not meet the 70% target.

## Per-file line coverage

| Source file | Line coverage |
|---|---:|
| Intersection.cpp | 100.0% |
| Road.cpp | 84.6% |
| Simulation.cpp | 91.5% |
| Vehicle.cpp | 95.8% |
| signal.cpp | 76.6% |
| signal.hpp | 83.3% |

The full HTML/XML details are attached to the Actions run. Branch coverage is reported separately in BRANCH_COVERAGE_REPORT.md.
