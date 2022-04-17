#pragma once

#include <memory>
#include "aircraft.hpp"
#include "aircraft_types.hpp"
#include "airport.hpp"

class AircraftFactory
{
public:
    AircraftFactory() {
        init_aircraft_types();
    }

    std::unique_ptr<Aircraft> create_aircraft(AircraftType& type, Airport* airport);
    std::unique_ptr<Aircraft> create_random_aircraft(Airport* airport);

    long count_aircraft_from_airline(unsigned int airline) const;

private:
    static constexpr size_t NUM_AIRCRAFT_TYPES = 3;
    AircraftType* aircraft_types[NUM_AIRCRAFT_TYPES] {};
    std::set<std::string> used_flight_numbers = {};
    void init_aircraft_types();
};