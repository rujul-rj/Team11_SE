#include "Road.h"
#include <stdexcept>
#include <utility>

Road::Road(std::string id, std::string fromIntersection, std::string toIntersection,
           int lanes, bool blocked)
    : id_(std::move(id)), from_(std::move(fromIntersection)),
      to_(std::move(toIntersection)), lanes_(lanes), blocked_(blocked) {
    if (id_.empty() || from_.empty() || to_.empty()) {
        throw std::invalid_argument("Road ID and intersection IDs must not be empty.");
    }
    if (from_ == to_) {
        throw std::invalid_argument("A road must connect two different intersections.");
    }
    if (lanes_ <= 0) {
        throw std::invalid_argument("Road lane count must be positive.");
    }
}

const std::string& Road::id() const noexcept { return id_; }
const std::string& Road::from() const noexcept { return from_; }
const std::string& Road::to() const noexcept { return to_; }
int Road::lanes() const noexcept { return lanes_; }
bool Road::isBlocked() const noexcept { return blocked_; }
void Road::setBlocked(bool blocked) noexcept { blocked_ = blocked; }
