#pragma once

#include "aircraft.hpp"

#include <memory>
#include <string>
#include <vector>

class AircraftManager : public GL::DynamicObject
{
private:
    std::vector<std::unique_ptr<Aircraft>> aircrafts;
    int crashed_aircrafts = 0;

public:
    void add(std::unique_ptr<Aircraft>& aircraft);

    void move(float d) override;

    long get_required_fuel() const;

    int get_n_crashed_aircrafts() const { return crashed_aircrafts; }
};