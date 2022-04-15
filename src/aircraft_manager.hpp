#pragma once

#include "aircraft.hpp"

#include <memory>
#include <string>
#include <vector>

class AircraftManager : public GL::DynamicObject
{
public:
    void add(std::unique_ptr<Aircraft>& aircraft);

    void move(float d) override;

    long get_required_fuel() const;

private:
    std::vector<std::unique_ptr<Aircraft>> aircrafts;
};