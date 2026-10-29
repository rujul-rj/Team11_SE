#include "TrafficNetwork.h"
#include <stdexcept>

void TrafficNetwork::addIntersection(const Intersection& intersection) {
    const auto inserted = intersections_.emplace(intersection.id(), intersection);
    if (!inserted.second) throw std::invalid_argument("Duplicate intersection ID: " + intersection.id());
}
void TrafficNetwork::addRoad(const Road& road) {
    if (!hasIntersection(road.from()) || !hasIntersection(road.to())) {
        throw std::invalid_argument("Both endpoints of a road must exist in the network.");
    }
    if (hasRoad(road.from(), road.to())) {
        throw std::invalid_argument("A road already connects " + road.from() + " to " + road.to());
    }
    roads_.push_back(road);
}
bool TrafficNetwork::hasIntersection(const std::string& id) const noexcept {
    return intersections_.find(id) != intersections_.end();
}
bool TrafficNetwork::hasRoad(const std::string& from, const std::string& to) const noexcept {
    return findRoad(from, to) != nullptr;
}
Road* TrafficNetwork::findRoad(const std::string& from, const std::string& to) noexcept {
    for (auto& road : roads_) if (road.from() == from && road.to() == to) return &road;
    return nullptr;
}
const Road* TrafficNetwork::findRoad(const std::string& from, const std::string& to) const noexcept {
    for (const auto& road : roads_) if (road.from() == from && road.to() == to) return &road;
    return nullptr;
}
Intersection* TrafficNetwork::findIntersection(const std::string& id) noexcept {
    auto it = intersections_.find(id);
    return it == intersections_.end() ? nullptr : &it->second;
}
const Intersection* TrafficNetwork::findIntersection(const std::string& id) const noexcept {
    auto it = intersections_.find(id);
    return it == intersections_.end() ? nullptr : &it->second;
}
bool TrafficNetwork::isValidRoute(const std::vector<std::string>& route) const noexcept {
    if (route.empty()) return false;
    for (const auto& id : route) if (!hasIntersection(id)) return false;
    for (std::size_t i = 1; i < route.size(); ++i) {
        if (!hasRoad(route[i - 1], route[i])) return false;
    }
    return true;
}
const std::map<std::string, Intersection>& TrafficNetwork::intersections() const noexcept { return intersections_; }
const std::vector<Road>& TrafficNetwork::roads() const noexcept { return roads_; }
