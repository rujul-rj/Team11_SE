# Code Coverage Report

**Repository:** Team11_SE  
**Commit tested:** bc2a643ff1067350b0a949dd8233ba3db12a677f  
**Date:** 15 November 2026  
**Tool:** gcovr 8.8 / GCC coverage instrumentation  
**Workflow:** https://github.com/rujul-rj/Team11_SE/actions/runs/43695727436  
**Artifact:** team11-coverage-reports (HTML detail report and Cobertura XML)

## Result

The STP exit criterion is at least 80% line coverage and 70% branch coverage for the core implementation set. The measured core set is Road, Intersection, Vehicle, Simulation (incident/analytics orchestration) and the signal-control module. Executable entry points and configuration parser/network infrastructure are reported separately.

- **Aggregate core line coverage:** 97.85% (547 / 559) — PASS.
- **Aggregate core branch coverage:** 70.02% (570 / 814) — PASS, narrowly above the 70% threshold.
- **All-source line coverage:** 68% in gcovr's full report because CLI and benchmark entry points are intentionally not unit-test targets.

## Per-file core coverage

| Source file | Line coverage | Branch coverage |
|---|---:|---:|
| Intersection.cpp | 100.0% | 75.0% |
| Road.cpp | 100.0% | 83.3% |
| Simulation.cpp | 98.2% | 71.3% |
| Vehicle.cpp | 100.0% | 81.8% |
| signal.cpp | 96.9% | 67.0% |
| signal.hpp | 100.0% | 100.0% |

The aggregate branch threshold is met. The signal.cpp file is individually at 67.0%; if the evaluator interprets the 70% requirement per module rather than across the core set, 14 additional signal.cpp branch outcomes need coverage.

## Reproduction

```bash
cmake -S . -B build-coverage -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="--coverage" \
  -DCMAKE_EXE_LINKER_FLAGS="--coverage"
cmake --build build-coverage --parallel
ctest --test-dir build-coverage --output-on-failure
gcovr --root . --filter 'src/.*\\.cpp' --filter 'tms/signals/.*\\.cpp' \
  --xml-pretty --output coverage.xml --html-details coverage.html
```

Download the workflow artifact for line-by-line details.
