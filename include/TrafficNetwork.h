#pragma once
#include "Intersection.h"
#include "Road.h"
#include <map>
#include <string>
#include <vector>

class TrafficNetwork {
public:
    void addIntersection(const Intersection& intersection);
    void addRoad(const Road& road);

    bool hasIntersection(const std::string& id) const noexcept;
    bool hasRoad(const std::string& from, const std::string& to) const noexcept;
    Road* findRoad(const std::string& from, const std::string& to) noexcept;
    const Road* findRoad(const std::string& from, const std::string& to) const noexcept;
    Intersection* findIntersection(const std::string& id) noexcept;
    const Intersection* findIntersection(const std::string& id) const noexcept;
    bool isValidRoute(const std::vector<std::string>& route) const noexcept;

    const std::map<std::string, Intersection>& intersections() const noexcept;
    const std::vector<Road>& roads() const noexcept;

private:
    std::map<std::string, Intersection> intersections_;
    std::vector<Road> roads_;
};
