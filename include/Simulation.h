#pragma once
#include "TrafficNetwork.h"
#include "Vehicle.h"
#include "tms/signals/signal.hpp"
#include <cstddef>
#include <string>
#include <vector>

struct SimulationStats {
    std::size_t activeVehicles{0};
    std::size_t queuedVehicles{0};
    std::size_t throughput{0};
    double totalWaitingTimeSeconds{0.0};
};

struct SimulationSummary {
    std::size_t stepsExecuted{0};
    std::size_t vehiclesAdded{0};
    std::size_t totalArrived{0};
    std::size_t activeVehicles{0};
    double totalWaitingTimeSeconds{0.0};
    double averageWaitingTimeSeconds{0.0};
    double averageQueueLength{0.0};
    double throughputVehiclesPerSecond{0.0};
};

class Simulation {
public:
    explicit Simulation(TrafficNetwork network, double stepSeconds = 1.0);
    void addVehicle(const Vehicle& vehicle);
    void step();
    void run(std::size_t steps);
    void setSignal(const std::string& intersectionId, SignalState state);
    void setRoadBlocked(const std::string& from, const std::string& to, bool blocked);
    tms::signals::ValidationResult configureSignal(const tms::signals::IntersectionSignalConfig& config);
    tms::signals::SignalManager& signalManager() noexcept;
    const tms::signals::SignalManager& signalManager() const noexcept;
    bool emergencyArrived(const std::string& intersectionId, const std::string& roadId);
    bool emergencyPassed(const std::string& intersectionId, const std::string& roadId);
    const TrafficNetwork& network() const noexcept;
    const std::vector<Vehicle>& vehicles() const noexcept;
    const SimulationStats& lastStepStats() const noexcept;
    std::size_t totalArrived() const noexcept;
    std::size_t stepsExecuted() const noexcept;
    SimulationSummary summary() const noexcept;
private:
    void refreshSignalQueues();
    TrafficNetwork network_;
    std::vector<Vehicle> vehicles_;
    double stepSeconds_;
    double signalTimeRemainder_{0.0};
    tms::signals::SignalManager signalManager_;
    std::size_t totalArrived_{0};
    std::size_t stepsExecuted_{0};
    std::size_t cumulativeQueuedVehicles_{0};
    double cumulativeWaitingTimeSeconds_{0.0};
    SimulationStats lastStepStats_;
};
