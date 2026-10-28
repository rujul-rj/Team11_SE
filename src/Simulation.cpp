#include "Simulation.h"
#include <stdexcept>
#include <string>
#include <utility>

Simulation::Simulation(TrafficNetwork network, double stepSeconds)
    : network_(std::move(network)), stepSeconds_(stepSeconds) {
    if (stepSeconds_ <= 0.0) throw std::invalid_argument("Simulation step duration must be positive.");
}
void Simulation::addVehicle(const Vehicle& vehicle) {
    if (!network_.isValidRoute(vehicle.route())) {
        throw std::invalid_argument("Vehicle " + vehicle.id() + " has an invalid route.");
    }
    for (const auto& existing : vehicles_) {
        if (existing.id() == vehicle.id()) throw std::invalid_argument("Duplicate vehicle ID: " + vehicle.id());
    }
    vehicles_.push_back(vehicle);
}
void Simulation::step() {
    lastStepStats_ = {};
    for (auto& vehicle : vehicles_) {
        if (vehicle.hasArrived()) continue;
        const auto* current = network_.findIntersection(vehicle.currentIntersection());
        const auto* next = vehicle.nextIntersection();
        if (current == nullptr || next == nullptr) continue;

        auto* road = network_.findRoad(vehicle.currentIntersection(), *next);
        if (road == nullptr || road->isBlocked() || current->signalState() == SignalState::Red) {
            vehicle.waitOneStep(stepSeconds_);
            ++lastStepStats_.queuedVehicles;
            continue;
        }
        vehicle.moveToNextIntersection();
        if (vehicle.hasArrived()) {
            ++lastStepStats_.throughput;
            ++totalArrived_;
        }
    }
    for (const auto& vehicle : vehicles_) {
        if (!vehicle.hasArrived()) ++lastStepStats_.activeVehicles;
        lastStepStats_.totalWaitingTimeSeconds += vehicle.waitingTimeSeconds();
    }
}
void Simulation::run(std::size_t steps) {
    for (std::size_t i = 0; i < steps; ++i) step();
}
void Simulation::setSignal(const std::string& intersectionId, SignalState state) {
    auto* intersection = network_.findIntersection(intersectionId);
    if (intersection == nullptr) throw std::invalid_argument("Unknown intersection: " + intersectionId);
    intersection->setSignalState(state);
}
void Simulation::setRoadBlocked(const std::string& from, const std::string& to, bool blocked) {
    auto* road = network_.findRoad(from, to);
    if (road == nullptr) throw std::invalid_argument("Unknown road: " + from + " -> " + to);
    road->setBlocked(blocked);
}
const TrafficNetwork& Simulation::network() const noexcept { return network_; }
const std::vector<Vehicle>& Simulation::vehicles() const noexcept { return vehicles_; }
const SimulationStats& Simulation::lastStepStats() const noexcept { return lastStepStats_; }
std::size_t Simulation::totalArrived() const noexcept { return totalArrived_; }
