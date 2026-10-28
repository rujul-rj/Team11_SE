#include "Simulation.h"
#include <cmath>
#include <map>
#include <stdexcept>
#include <utility>

Simulation::Simulation(TrafficNetwork network, double stepSeconds)
    : network_(std::move(network)), stepSeconds_(stepSeconds) {
    if (!std::isfinite(stepSeconds_) || stepSeconds_ <= 0.0) {
        throw std::invalid_argument("Simulation step duration must be finite and positive.");
    }
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
void Simulation::refreshSignalQueues() {
    std::map<std::string, std::map<std::string, int>> counts;
    for (const auto& v : vehicles_) {
        if (v.hasArrived() || !v.nextIntersection()) continue;
        const auto* road = network_.findRoad(v.currentIntersection(), *v.nextIntersection());
        if (road) ++counts[v.currentIntersection()][road->id()];
    }
    for (const auto& entry : network_.intersections()) {
        auto* c = signalManager_.get(entry.first);
        if (!c) continue;
        for (const auto& phase : c->config().phases) for (const auto& id : phase.roads) {
            const auto i = counts.find(entry.first);
            const int q = (i == counts.end() || !i->second.count(id)) ? 0 : i->second.at(id);
            signalManager_.update_queue(entry.first, id, q);
        }
    }
}
void Simulation::step() {
    lastStepStats_ = {};
    refreshSignalQueues();
    for (auto& v : vehicles_) {
        if (v.hasArrived()) continue;
        const auto* current = network_.findIntersection(v.currentIntersection());
        const auto* next = v.nextIntersection();
        if (!current || !next) continue;
        auto* road = network_.findRoad(v.currentIntersection(), *next);
        bool allowed = current->signalState() != SignalState::Red;
        if (signalManager_.has(current->id())) allowed = road && signalManager_.can_proceed(current->id(), road->id());
        if (!road || road->isBlocked() || !allowed) {
            v.waitOneStep(stepSeconds_);
            ++lastStepStats_.queuedVehicles;
            continue;
        }
        v.moveToNextIntersection();
        if (v.hasArrived()) { ++lastStepStats_.throughput; ++totalArrived_; }
    }
    signalTimeRemainder_ += stepSeconds_;
    const int seconds = static_cast<int>(signalTimeRemainder_);
    if (seconds > 0) { signalManager_.tick(seconds); signalTimeRemainder_ -= seconds; }
    for (const auto& v : vehicles_) {
        if (!v.hasArrived()) ++lastStepStats_.activeVehicles;
        lastStepStats_.totalWaitingTimeSeconds += v.waitingTimeSeconds();
    }
    ++stepsExecuted_;
    cumulativeQueuedVehicles_ += lastStepStats_.queuedVehicles;
    cumulativeWaitingTimeSeconds_ = lastStepStats_.totalWaitingTimeSeconds;
}
void Simulation::run(std::size_t steps) { for (std::size_t i = 0; i < steps; ++i) step(); }
void Simulation::setSignal(const std::string& id, SignalState state) {
    auto* i = network_.findIntersection(id);
    if (!i) throw std::invalid_argument("Unknown intersection: " + id);
    i->setSignalState(state);
}
void Simulation::setRoadBlocked(const std::string& from, const std::string& to, bool blocked) {
    auto* r = network_.findRoad(from, to);
    if (!r) throw std::invalid_argument("Unknown road: " + from + " -> " + to);
    r->setBlocked(blocked);
}
tms::signals::ValidationResult Simulation::configureSignal(const tms::signals::IntersectionSignalConfig& c) {
    if (!network_.hasIntersection(c.intersection_id)) {
        tms::signals::ValidationResult r; r.ok = false;
        r.errors.push_back("unknown network intersection '" + c.intersection_id + "'");
        return r;
    }
    for (const auto& p : c.phases) for (const auto& id : p.roads) {
        bool found = false;
        for (const auto& road : network_.roads()) {
            if (road.id() == id && road.from() == c.intersection_id) { found = true; break; }
        }
        if (!found) {
            tms::signals::ValidationResult r; r.ok = false;
            r.errors.push_back("road '" + id + "' is not outgoing from intersection '" + c.intersection_id + "'");
            return r;
        }
    }
    return signalManager_.configure(c);
}
tms::signals::SignalManager& Simulation::signalManager() noexcept { return signalManager_; }
const tms::signals::SignalManager& Simulation::signalManager() const noexcept { return signalManager_; }
bool Simulation::emergencyArrived(const std::string& i, const std::string& r) { return signalManager_.emergency_arrived(i, r); }
bool Simulation::emergencyPassed(const std::string& i, const std::string& r) { return signalManager_.emergency_passed(i, r); }
const TrafficNetwork& Simulation::network() const noexcept { return network_; }
const std::vector<Vehicle>& Simulation::vehicles() const noexcept { return vehicles_; }
const SimulationStats& Simulation::lastStepStats() const noexcept { return lastStepStats_; }
std::size_t Simulation::totalArrived() const noexcept { return totalArrived_; }
std::size_t Simulation::stepsExecuted() const noexcept { return stepsExecuted_; }
SimulationSummary Simulation::summary() const noexcept {
    SimulationSummary result;
    result.stepsExecuted = stepsExecuted_;
    result.vehiclesAdded = vehicles_.size();
    result.totalArrived = totalArrived_;
    result.activeVehicles = vehicles_.size() - totalArrived_;
    result.totalWaitingTimeSeconds = cumulativeWaitingTimeSeconds_;
    result.averageWaitingTimeSeconds = vehicles_.empty() ? 0.0
        : cumulativeWaitingTimeSeconds_ / static_cast<double>(vehicles_.size());
    result.averageQueueLength = stepsExecuted_ == 0 ? 0.0
        : static_cast<double>(cumulativeQueuedVehicles_) / static_cast<double>(stepsExecuted_);
    const double elapsedSeconds = static_cast<double>(stepsExecuted_) * stepSeconds_;
    result.throughputVehiclesPerSecond = elapsedSeconds <= 0.0 ? 0.0
        : static_cast<double>(totalArrived_) / elapsedSeconds;
    return result;
}
