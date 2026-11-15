#include "Road.h"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

Road::Road(std::string id, std::string fromIntersection, std::string toIntersection,
           int lanes, bool blocked)
    : id_(std::move(id)), from_(std::move(fromIntersection)),
      to_(std::move(toIntersection)), lanes_(lanes),
      capacity_(lanes > 0 && lanes <= std::numeric_limits<int>::max() / 20 ? lanes * 20 : 0),
      blocked_(blocked) {
    if (id_.empty() || from_.empty() || to_.empty()) {
        throw std::invalid_argument("Road ID and intersection IDs must not be empty.");
    }
    if (from_ == to_) {
        throw std::invalid_argument("A road must connect two different intersections.");
    }
    if (lanes_ <= 0 || capacity_ <= 0) {
        throw std::invalid_argument("Road lane count must be positive and capacity must be representable.");
    }
}

Road::Road(std::string id, std::string fromIntersection, std::string toIntersection,
           int lanes, int capacity, bool blocked)
    : id_(std::move(id)), from_(std::move(fromIntersection)),
      to_(std::move(toIntersection)), lanes_(lanes), capacity_(capacity), blocked_(blocked) {
    if (id_.empty() || from_.empty() || to_.empty()) {
        throw std::invalid_argument("Road ID and intersection IDs must not be empty.");
    }
    if (from_ == to_) {
        throw std::invalid_argument("A road must connect two different intersections.");
    }
    if (lanes_ <= 0 || capacity_ <= 0) {
        throw std::invalid_argument("Road lane count and capacity must be positive.");
    }
}

const std::string& Road::id() const noexcept { return id_; }
const std::string& Road::from() const noexcept { return from_; }
const std::string& Road::to() const noexcept { return to_; }
int Road::lanes() const noexcept { return lanes_; }
int Road::capacity() const noexcept { return capacity_; }
int Road::effectiveCapacity() const noexcept {
    return static_cast<int>(std::floor(static_cast<double>(capacity_) * capacityReductionFactor_));
}
double Road::capacityReductionFactor() const noexcept { return capacityReductionFactor_; }
void Road::setCapacityReductionFactor(double factor) {
    if (!std::isfinite(factor) || factor < 0.0 || factor > 1.0) {
        throw std::invalid_argument("Road capacity reduction factor must be within [0, 1].");
    }
    capacityReductionFactor_ = factor;
}
bool Road::isBlocked() const noexcept { return blocked_; }
void Road::setBlocked(bool blocked) noexcept { blocked_ = blocked; }
