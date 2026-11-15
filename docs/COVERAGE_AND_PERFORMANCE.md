# Code Coverage, Branch Coverage and Performance Evidence

## Coverage generation

The GitHub Actions workflow includes a coverage job that:
1. Configures a GCC coverage-instrumented build.
2. Builds all targets and runs CTest.
3. Uses gcovr to generate line and branch coverage outputs.
4. Uploads the generated files as the workflow artifact `team11-coverage-reports`.

The artifact contains:
- `coverage-summary.txt` — text summary
- `index.html` and detail pages — browsable HTML report
- `coverage.xml` — machine-readable report

Download the artifact from the successful Actions run and place the extracted reports under `docs/coverage/` in the final release branch if the evaluator requires the generated files to be committed in the repository.

## Local reproduction

Install gcovr, then run:

```bash
python3 -m pip install gcovr
cmake -S . -B build-coverage -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_FLAGS="--coverage -O0 -g" \
  -DCMAKE_EXE_LINKER_FLAGS="--coverage"
cmake --build build-coverage --parallel
ctest --test-dir build-coverage --output-on-failure
mkdir -p docs/coverage
gcovr --root . --filter 'src/.*' --filter 'tms/.*' \
  --txt docs/coverage/coverage-summary.txt \
  --html-details docs/coverage/index.html \
  --xml docs/coverage/coverage.xml --xml-pretty
```

## Performance validation

The approved SRS NFR-001 target is **at least 1,000 vehicle-movement steps within 2.0 seconds**. The STP defines one vehicle's movement update in one simulation time step as one vehicle-step, and TC-013.1 requires 10 repeated runs with every run at or below 2.0 seconds. TC-013.2 repeats this for adaptive mode; TC-013.3 repeats it with incidents and emergency vehicles. The STP reference machine is an x86-64 desktop/laptop with at least 4 cores and 8 GB RAM; record the actual CPU, memory, OS and compiler. Performance tests must use a Release build. The CI benchmark is a reproducible proxy; it does not replace the reference-machine result. Measure a defined workload on the intended environment and record:
- number of intersections, roads and vehicles
- simulation steps and configured step duration
- compiler, optimization level, CPU and OS
- elapsed wall-clock time and repeated-run statistics
- target from the SRS and measured result

The CI performance executable runs 100 vehicles for 10 steps (1,000 vehicle-step updates) over a four-intersection / twelve-road network, repeating the workload 10 times. Its timing is recorded in the Actions log. The reference-machine TC-013.1/TC-013.2/TC-013.3 results still need to be run on the STP reference machine before NFR-001 can be signed off.
