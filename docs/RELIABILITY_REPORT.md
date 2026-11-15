# Reliability and Stress Test Report

**Repository:** Team11_SE  
**Commit tested:** 269166cb1472f7c585eb0846686e85a1cf074740  
**Date:** 15 November 2026  
**Workflow:** https://github.com/rujul-rj/Team11_SE/actions/runs/43693880691  
**Build:** Release / C++17

## TC-014.2 — Repeated runs with different seeds

- Runs: 100
- Seeds: 1 through 100
- Traffic density: HIGH
- Maximum generated vehicles per run: 100
- Run length: 300 simulation steps (10× a 30-step normal run)
- Variation: every fourth seed applies a 50% capacity factor to A→B; every fifth seed blocks B→C
- Validation: every run completed 300 steps, generated 100 vehicles and produced finite waiting-time, queue and throughput metrics.

**Result: PASS — 100/100 runs completed.**

## TC-014.1 / TC-014.4 — Long-run and all-roads-blocked stress

A separate 300-step scenario blocked all 12 directed roads in the four-intersection network, generated 100 vehicles and completed the run without abnormal termination. The end state correctly retained 100 active vehicles because no road was available.

**Result: PASS — all-roads-blocked stress scenario completed.**

## Reproducibility

Vehicle route selection uses an explicit seed. The unit suite verifies that the same seed reproduces the same generated route sequence. Strategy comparison uses identical deterministic demand for both strategies.

## Limitations

The reliability workload is a synthetic simulation workload. It does not replace manual usability validation or testing on the team's designated desktop/laptop.
