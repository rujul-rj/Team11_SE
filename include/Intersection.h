#pragma once
#include <string>

enum class SignalState { Red, Green };

class Intersection {
public:
    explicit Intersection(std::string id);

    const std::string& id() const noexcept;
    SignalState signalState() const noexcept;
    void setSignalState(SignalState state) noexcept;

private:
    std::string id_;
    SignalState signalState_{SignalState::Green};
};
