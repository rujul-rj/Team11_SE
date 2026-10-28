#pragma once
#include <string>

class Road {
public:
    Road(std::string id, std::string fromIntersection, std::string toIntersection,
         int lanes = 1, bool blocked = false);

    const std::string& id() const noexcept;
    const std::string& from() const noexcept;
    const std::string& to() const noexcept;
    int lanes() const noexcept;
    bool isBlocked() const noexcept;
    void setBlocked(bool blocked) noexcept;

private:
    std::string id_;
    std::string from_;
    std::string to_;
    int lanes_;
    bool blocked_;
};
