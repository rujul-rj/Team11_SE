#pragma once
#include "TrafficNetwork.h"
#include "Vehicle.h"
#include <cstddef>
#include <vector>

struct SimulationStats {
    std::size_t activeVehicles{0};
    std::size_t queuedVehicles{0};
    std::size_t throughput{0}; // vehicles that arrived during this step
    double totalWaitingTimeSeconds{0.0};
};

class Simulation {
public:
    explicit Simulation(TrafficNetwork network, double stepSeconds = 1.0);

    void addVehicle(const Vehicle& vehicle);
    void step();
    void run(std::size_t steps);
    void setSignal(const std::string& intersectionId, SignalState state);
    void setRoadBlocked(const std::string& from, const std::string& to, bool blocked);

    const TrafficNetwork& network() const noexcept;
    const std::vector<Vehicle>& vehicles() const noexcept;
    const SimulationStats& lastStepStats() const noexcept;
    std::size_t totalArrived() const noexcept;

private:
    TrafficNetwork network_;
    std::vector<Vehicle> vehicles_;
    double stepSeconds_;
    std::size_t totalArrived_{0};
    SimulationStats lastStepStats_;
};
