#include "aircraft_factory.hpp"

const std::string airlines[8] = { "AF", "LH", "EY", "DL", "KL", "BA", "AY", "EY" };

std::unique_ptr<Aircraft> AircraftFactory::create_aircraft(AircraftType& type, Airport* airport)
{
    assert(airport); // make sure the airport is initialized before creating aircraft

    std::string flight_number;
    while (true) {
        flight_number = airlines[std::rand() % 8] + std::to_string(1000 + (rand() % 9000));
        if (used_flight_numbers.find(flight_number) == used_flight_numbers.end()) {
            used_flight_numbers.insert(flight_number);
            break;
        }
    }
    const float angle       = (rand() % 1000) * 2 * 3.141592f / 1000.f; // random angle between 0 and 2pi
    const Point3D start     = Point3D { std::sin(angle), std::cos(angle), 0 } * 3 + Point3D { 0, 0, 2 };
    const Point3D direction = (-start).normalize();

    return std::make_unique<Aircraft>(type, flight_number, start, direction, airport->get_tower());
}

std::unique_ptr<Aircraft> AircraftFactory::create_random_aircraft(Airport* airport)
{
    return create_aircraft(*(aircraft_types[rand() % 3]), airport);
}

// note: textures have to be initialized after initializing glut, so we'll need
// our own init here
void AircraftFactory::init_aircraft_types()
{
    aircraft_types[0] = new AircraftType { .02f, .05f, .02f, MediaPath { "l1011_48px.png" } };
    aircraft_types[1] = new AircraftType { .02f, .05f, .02f, MediaPath { "b707_jat.png" } };
    aircraft_types[2] = new AircraftType { .02f, .10f, .02f, MediaPath { "concorde_af.png" } };
}