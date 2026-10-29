#include "Vehicle.h"
#include <stdexcept>
#include <utility>

Vehicle::Vehicle(std::string id, std::vector<std::string> route)
    : id_(std::move(id)), route_(std::move(route)) {
    if (id_.empty()) throw std::invalid_argument("Vehicle ID must not be empty.");
    if (route_.empty()) throw std::invalid_argument("Vehicle route must contain an intersection.");
    for (const auto& stop : route_) {
        if (stop.empty()) throw std::invalid_argument("Route intersection IDs must not be empty.");
    }
}
const std::string& Vehicle::id() const noexcept { return id_; }
const std::vector<std::string>& Vehicle::route() const noexcept { return route_; }
const std::string& Vehicle::currentIntersection() const { return route_.at(routeIndex_); }
bool Vehicle::hasArrived() const noexcept { return routeIndex_ + 1 >= route_.size(); }
const std::string* Vehicle::nextIntersection() const noexcept {
    if (hasArrived()) return nullptr;
    return &route_[routeIndex_ + 1];
}
double Vehicle::waitingTimeSeconds() const noexcept { return waitingTimeSeconds_; }
void Vehicle::moveToNextIntersection() {
    if (!hasArrived()) ++routeIndex_;
}
void Vehicle::waitOneStep(double stepSeconds) {
    if (stepSeconds < 0.0) throw std::invalid_argument("Step duration cannot be negative.");
    waitingTimeSeconds_ += stepSeconds;
}
