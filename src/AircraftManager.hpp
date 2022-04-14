#pragma once

#include <vector>
#include <string>
#include <memory>

#include "aircraft.hpp"

class AircraftManager : public GL::DynamicObject
{
public:
    void add(std::unique_ptr<Aircraft>& aircraft);

    void move(float d) override;

private:
    std::vector<std::unique_ptr<Aircraft>> aircrafts;
};