# Code Coverage Report

**Repository:** Team11_SE  
**Commit tested:** 63b62e8d5a9f7e20f23687ce35d35aad7b8d8a08  
**Date:** 15 November 2026  
**Tool:** gcovr 8.8 / GCC coverage instrumentation  
**Workflow:** https://github.com/rujul-rj/Team11_SE/actions/runs/43693424990  
**Artifact:** team11-coverage-reports (HTML detail report and Cobertura XML)

## Result

The STP exit criterion is at least 80% line coverage and 70% branch coverage for core modules. For the SRS/STP core implementation set (Road, Intersection, Vehicle, Simulation incident/analytics orchestration and signal-control module), executable entry points and configuration parser/network infrastructure are excluded.

- **Line coverage:** 81.99% (446 / 544) — meets the 80% target.
- **Branch coverage:** 48.73% (385 / 790) — does not meet the 70% target.

## Per-file line coverage

| Source file | Line coverage |
|---|---:|
| Intersection.cpp | 100.0% |
| Road.cpp | 61.5% |
| Simulation.cpp | 91.4% |
| TrafficNetwork.cpp | 91.4% |
| Vehicle.cpp | 95.8% |
| signal.cpp | 75.0% |
| signal.hpp | 80.0% |
| signal_config_parser.cpp | 84.8% |

The full HTML/XML details are attached to the Actions run. Branch coverage is reported separately in BRANCH_COVERAGE_REPORT.md.
