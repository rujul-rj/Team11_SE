# Team 11 Traffic Management System — C++ Starter

This is a small, runnable C++17 baseline for the first implementation tasks:
- Road, lane, and intersection model (Issue #3)
- Vehicle generation and route validation (Issue #5)
- Vehicle movement and queues (Issue #6)
- Modular components with a lightweight test executable (related to #8 and #31)

It is a simulation, not a controller for real traffic signals. Signal control is intentionally simple so it can be integrated with the team's fixed-time/adaptive signal work.

## Build and run

### Windows (Visual Studio Build Tools or MinGW with CMake)
From this folder:

```powershell
cmake -S . -B build
cmake --build build --config Release
.\build\Release\traffic_sim.exe
```

For a single-config generator such as MinGW, the executable may be at `build\traffic_sim.exe`.

### Linux / WSL
```bash
cmake -S . -B build
cmake --build build -j
./build/traffic_sim
```

## Run tests

```bash
ctest --test-dir build --output-on-failure
```

## Model and behavior

- Roads are directed from one intersection to another and have a lane count.
- A vehicle route is a sequence of intersection IDs; each consecutive pair must be connected by a road.
- Vehicles move one road per simulation step when the signal at their current intersection is green and the road is not blocked.
- At a red signal or blocked road, a vehicle remains queued at its current intersection and its waiting time increases.
- Each simulation step updates the model and records aggregate queue length, waiting time, and throughput.
- The demo uses deterministic vehicle arrivals for reproducible output.

## Important integration notes

1. This is a starter implementation created because the repository ZIP provided did not include existing C++ source files. Review it with your teammates before merging.
2. Signal timing is represented by a simple per-intersection RED/GREEN state. Replace or extend this with the team's signal-control module.
3. Incident support is represented by blocking/unblocking a road.
4. The demo/test harness is deliberately dependency-free. If the team requires GoogleTest, the tests can be migrated later.
5. Add each source file to the repository, build locally, run tests, then open a pull request. Do not overwrite teammates' uncommitted local work.
