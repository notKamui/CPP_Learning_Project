#include "tower_sim.hpp"

#include "GL/opengl_interface.hpp"
#include "aircraft.hpp"
#include "airport.hpp"
#include "config.hpp"
#include "img/image.hpp"
#include "img/media_path.hpp"

#include <cstdlib>

using namespace std::string_literals;

TowerSimulation::TowerSimulation(int argc, char** argv) :
    init { argc, argv },
    help { (argc > 1) && (std::string { argv[1] } == "--help"s || std::string { argv[1] } == "-h"s) }
{
    create_keystrokes();
}

TowerSimulation::~TowerSimulation()
{
    delete airport;
}

void TowerSimulation::add_aircraft()
{
    auto aircraft = aircraft_factory.create_random_aircraft(airport);
    aircraft_manager.add(aircraft);
}

void TowerSimulation::create_keystrokes()
{
    GL::keystrokes.emplace('x', []() { GL::exit_loop(); });
    GL::keystrokes.emplace('q', []() { GL::exit_loop(); });
    GL::keystrokes.emplace('c', [this]() { add_aircraft(); });
    GL::keystrokes.emplace('+', []() { GL::change_zoom(0.95f); });
    GL::keystrokes.emplace('-', []() { GL::change_zoom(1.05f); });
    GL::keystrokes.emplace('f', []() { GL::toggle_fullscreen(); });
    GL::keystrokes.emplace('a', []() { GL::ticks_per_sec--; std::max(GL::ticks_per_sec, 1u); }); // fps--
    GL::keystrokes.emplace('z', []() { GL::ticks_per_sec++; }); // fps++
    GL::keystrokes.emplace('e', []() { GL::simulation_speed -= .1f; std::max(GL::simulation_speed, 0.0f); }); // speed--
    GL::keystrokes.emplace('r', []() { GL::simulation_speed += .1f; }); // speed++
    GL::keystrokes.emplace('p', []() { GL::paused = !GL::paused; });
    GL::keystrokes.emplace('m', [this]() { std::cout << aircraft_manager.get_n_crashed_aircrafts() << " aircraft(s) crashed in total" << std::endl; });

    for (auto i = 0u; i < 8; ++i)
    {
        GL::keystrokes.emplace(i + '0', [this, i]() {
            std::cout << "Airline n°" << i << " has " << aircraft_factory.count_aircraft_from_airline(i) << " flights" << std::endl;
        });
    }
}

void TowerSimulation::display_help() const
{
    std::cout << "This is an airport tower simulator" << std::endl
              << "the following keysstrokes have meaning:" << std::endl;

    for (const auto& [first, _] : GL::keystrokes)
    {
        std::cout << first << ' ';
    }

    std::cout << std::endl;
}

void TowerSimulation::init_airport()
{
    assert(airport == nullptr);
    airport = new Airport { aircraft_manager, one_lane_airport, Point3D { 0, 0, 0 },
                            new img::Image { one_lane_airport_sprite_path.get_full_path() } };

    GL::move_queue.emplace(airport);
    GL::move_queue.emplace(&aircraft_manager);
}

void TowerSimulation::launch()
{
    if (help)
    {
        display_help();
        return;
    }

    init_airport();
    assert(airport != nullptr);
    GL::loop();
}
