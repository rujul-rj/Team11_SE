# Code Coverage Report

**Repository:** Team11_SE  
**Commit tested:** b0cd2d7c2e15beffa5dd71534432734ab897ce9f  
**Date:** 15 November 2026  
**Tool:** gcovr 8.8 / GCC coverage instrumentation  
**Workflow:** https://github.com/rujul-rj/Team11_SE/actions/runs/43692880777  
**Artifact:** team11-coverage-reports (HTML detail report and Cobertura XML)

## Result

The STP exit criterion is at least 80% line coverage and 70% branch coverage for core modules. For a representative source-code measure, the figures below exclude executable entry points (main.cpp, strategy_comparison.cpp and performance_benchmark.cpp), which are not unit-test targets.

- **Line coverage:** 83.71% (524 / 626) — meets the 80% target.
- **Branch coverage:** 47.71% (499 / 1,046) — does not meet the 70% target.

## Per-file line coverage

| Source file | Line coverage |
|---|---:|
| Intersection.cpp | 100.0% |
| Road.cpp | 82.4% |
| Simulation.cpp | 90.5% |
| TrafficNetwork.cpp | 91.4% |
| Vehicle.cpp | 95.8% |
| signal.cpp | 75.0% |
| signal.hpp | 80.0% |
| signal_config_parser.cpp | 84.8% |

The full HTML/XML details are attached to the Actions run. Branch coverage is reported separately in BRANCH_COVERAGE_REPORT.md.
