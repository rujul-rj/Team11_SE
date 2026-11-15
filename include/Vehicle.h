#pragma once
#include <string>
#include <vector>

enum class VehicleType { Normal, Emergency };

class Vehicle {
public:
    Vehicle(std::string id, std::vector<std::string> route, VehicleType type = VehicleType::Normal);

    const std::string& id() const noexcept;
    const std::vector<std::string>& route() const noexcept;
    const std::string& currentIntersection() const;
    bool hasArrived() const noexcept;
    const std::string* nextIntersection() const noexcept;
    double waitingTimeSeconds() const noexcept;
    VehicleType type() const noexcept;
    bool isEmergency() const noexcept;

    // Internal simulation operations.
    void moveToNextIntersection();
    void waitOneStep(double stepSeconds);

private:
    std::string id_;
    std::vector<std::string> route_;
    std::size_t routeIndex_{0};
    double waitingTimeSeconds_{0.0};
    VehicleType type_{VehicleType::Normal};
};
