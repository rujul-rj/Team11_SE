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
  --xml-pretty docs/coverage/coverage.xml
```

## Performance validation

The exact numeric performance target must be copied from the approved SRS before a compliance claim is made. Measure a defined workload on the intended environment and record:
- number of intersections, roads and vehicles
- simulation steps and configured step duration
- compiler, optimization level, CPU and OS
- elapsed wall-clock time and repeated-run statistics
- target from the SRS and measured result

The current CI repeat-run check is a reliability check, not a measured proof that an SRS performance target is met. Do not report performance compliance until the SRS target and actual timing evidence are attached.
