#include "Intersection.h"
#include <stdexcept>
#include <utility>

Intersection::Intersection(std::string id) : id_(std::move(id)) {
    if (id_.empty()) {
        throw std::invalid_argument("Intersection ID must not be empty.");
    }
}
const std::string& Intersection::id() const noexcept { return id_; }
SignalState Intersection::signalState() const noexcept { return signalState_; }
void Intersection::setSignalState(SignalState state) noexcept { signalState_ = state; }
