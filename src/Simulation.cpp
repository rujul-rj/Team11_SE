
#include "Simulation.h"

#include <map>
#include <stdexcept>
#include <utility>

Simulation::Simulation(TrafficNetwork network, double stepSeconds)
    : network_(std::move(network)), stepSeconds_(stepSeconds) {
    if (stepSeconds_ <= 0.0) {
        throw std::invalid_argument(
            "Simulation step duration must be positive."
        );
    }
}

void Simulation::addVehicle(const Vehicle& vehicle) {
    if (!network_.isValidRoute(vehicle.route())) {
        throw std::invalid_argument(
            "Vehicle " + vehicle.id() + " has an invalid route."
        );
    }

    for (const auto& existing : vehicles_) {
        if (existing.id() == vehicle.id()) {
            throw std::invalid_argument(
                "Duplicate vehicle ID: " + vehicle.id()
            );
        }
    }

    vehicles_.push_back(vehicle);
}

void Simulation::configureVehicleGeneration(
    std::vector<std::vector<std::string>> routes,
    std::size_t intervalSteps,
    std::size_t maxVehicles
) {
    if (intervalSteps == 0) {
        throw std::invalid_argument(
            "Vehicle generation interval must be positive."
        );
    }

    if (routes.empty() && maxVehicles > 0) {
        throw std::invalid_argument(
            "At least one route is required for vehicle generation."
        );
    }

    for (const auto& route : routes) {
        if (!network_.isValidRoute(route)) {
            throw std::invalid_argument(
                "Vehicle generation contains an invalid route."
            );
        }
    }

    generationRoutes_ = std::move(routes);
    generationIntervalSteps_ = intervalSteps;
    maxGeneratedVehicles_ = maxVehicles;
    generatedVehicles_ = 0;
    stepsSinceGeneration_ = 0;
}

void Simulation::generateVehicles() {
    if (generationIntervalSteps_ == 0 ||
        generatedVehicles_ >= maxGeneratedVehicles_ ||
        generationRoutes_.empty()) {
        return;
    }

    ++stepsSinceGeneration_;

    if (stepsSinceGeneration_ < generationIntervalSteps_) {
        return;
    }

    stepsSinceGeneration_ = 0;

    const auto& route =
        generationRoutes_[generatedVehicles_ % generationRoutes_.size()];

    std::string id;

    // Ensure generated IDs do not collide with existing vehicle IDs.
    do {
        id = "AUTO-" + std::to_string(nextVehicleNumber_++);

        bool duplicate = false;
        for (const auto& existing : vehicles_) {
            if (existing.id() == id) {
                duplicate = true;
                break;
            }
        }

        if (!duplicate) {
            break;
        }
    } while (true);

    addVehicle(Vehicle(id, route));
    ++generatedVehicles_;
}

void Simulation::refreshSignalQueues() {
    std::map<std::string, std::map<std::string, int>> counts;

    for (const auto& vehicle : vehicles_) {
        if (vehicle.hasArrived() || !vehicle.nextIntersection()) {
            continue;
        }

        const auto* road = network_.findRoad(
            vehicle.currentIntersection(),
            *vehicle.nextIntersection()
        );

        if (road) {
            ++counts[vehicle.currentIntersection()][road->id()];
        }
    }

    for (const auto& entry : network_.intersections()) {
        auto* controller = signalManager_.get(entry.first);

        if (!controller) {
            continue;
        }

        for (const auto& phase : controller->config().phases) {
            for (const auto& roadId : phase.roads) {
                const auto intersectionCounts = counts.find(entry.first);

                const int queueLength =
                    intersectionCounts == counts.end() ||
                    !intersectionCounts->second.count(roadId)
                        ? 0
                        : intersectionCounts->second.at(roadId);

                signalManager_.update_queue(
                    entry.first,
                    roadId,
                    queueLength
                );
            }
        }
    }
}

void Simulation::step() {
    lastStepStats_ = {};

    // Generate new traffic before updating queues and moving vehicles.
    generateVehicles();
    refreshSignalQueues();

    for (auto& vehicle : vehicles_) {
        if (vehicle.hasArrived()) {
            continue;
        }

        const auto* current =
            network_.findIntersection(vehicle.currentIntersection());

        const auto* next = vehicle.nextIntersection();

        if (!current || !next) {
            continue;
        }

        auto* road = network_.findRoad(
            vehicle.currentIntersection(),
            *next
        );

        bool allowed = current->signalState() != SignalState::Red;

        if (signalManager_.has(current->id())) {
            allowed = road &&
                signalManager_.can_proceed(current->id(), road->id());
        }

        if (!road || road->isBlocked() || !allowed) {
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

    // Advance signal time after movement.
    signalTimeRemainder_ += stepSeconds_;

    const int seconds = static_cast<int>(signalTimeRemainder_);

    if (seconds > 0) {
        signalManager_.tick(seconds);
        signalTimeRemainder_ -= seconds;
    }

    for (const auto& vehicle : vehicles_) {
        if (!vehicle.hasArrived()) {
            ++lastStepStats_.activeVehicles;
        }

        lastStepStats_.totalWaitingTimeSeconds +=
            vehicle.waitingTimeSeconds();
    }
}

void Simulation::run(std::size_t steps) {
    for (std::size_t i = 0; i < steps; ++i) {
        step();
    }
}

void Simulation::setSignal(
    const std::string& intersectionId,
    SignalState state
) {
    auto* intersection = network_.findIntersection(intersectionId);

    if (!intersection) {
        throw std::invalid_argument(
            "Unknown intersection: " + intersectionId
        );
    }

    intersection->setSignalState(state);
}

void Simulation::setRoadBlocked(
    const std::string& from,
    const std::string& to,
    bool blocked
) {
    auto* road = network_.findRoad(from, to);

    if (!road) {
        throw std::invalid_argument(
            "Unknown road: " + from + " -> " + to
        );
    }

    road->setBlocked(blocked);
}

tms::signals::ValidationResult Simulation::configureSignal(
    const tms::signals::IntersectionSignalConfig& config
) {
    if (!network_.hasIntersection(config.intersection_id)) {
        tms::signals::ValidationResult result;
        result.ok = false;
        result.errors.push_back(
            "unknown network intersection '" +
            config.intersection_id + "'"
        );
        return result;
    }

    for (const auto& phase : config.phases) {
        for (const auto& roadId : phase.roads) {
            bool found = false;

            for (const auto& road : network_.roads()) {
                if (road.id() == roadId &&
                    road.from() == config.intersection_id) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                tms::signals::ValidationResult result;
                result.ok = false;
                result.errors.push_back(
                    "road '" + roadId +
                    "' is not outgoing from intersection '" +
                    config.intersection_id + "'"
                );
                return result;
            }
        }
    }

    return signalManager_.configure(config);
}

tms::signals::SignalManager& Simulation::signalManager() noexcept {
    return signalManager_;
}

const tms::signals::SignalManager&
Simulation::signalManager() const noexcept {
    return signalManager_;
}

bool Simulation::emergencyArrived(
    const std::string& intersectionId,
    const std::string& roadId
) {
    return signalManager_.emergency_arrived(intersectionId, roadId);
}

bool Simulation::emergencyPassed(
    const std::string& intersectionId,
    const std::string& roadId
) {
    return signalManager_.emergency_passed(intersectionId, roadId);
}

const TrafficNetwork& Simulation::network() const noexcept {
    return network_;
}

const std::vector<Vehicle>& Simulation::vehicles() const noexcept {
    return vehicles_;
}

const SimulationStats& Simulation::lastStepStats() const noexcept {
    return lastStepStats_;
}

std::size_t Simulation::totalArrived() const noexcept {
    return totalArrived_;
}
