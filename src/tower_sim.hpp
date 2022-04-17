#pragma once

#include "aircraft_factory.hpp"
#include "aircraft_manager.hpp"
#include <ctime>

class Airport;
struct AircraftType;

class TowerSimulation
{
private:
    struct ContextInitializer {
         ContextInitializer(int argc, char** argv) {
             MediaPath::initialize(argv[0]);
             std::srand(static_cast<unsigned int>(std::time(nullptr)));
             GL::init_gl(argc, argv, "Airport Tower Simulation");
         }
    };

    const ContextInitializer init;
    bool help        = false;
    AircraftFactory aircraft_factory;
    AircraftManager aircraft_manager;
    Airport* airport = nullptr;

    TowerSimulation(const TowerSimulation&) = delete;
    TowerSimulation& operator=(const TowerSimulation&) = delete;

    void add_aircraft();

    void create_keystrokes();
    void display_help() const;

    void init_airport();

public:
    TowerSimulation(int argc, char** argv);
    ~TowerSimulation();

    void launch();
};
