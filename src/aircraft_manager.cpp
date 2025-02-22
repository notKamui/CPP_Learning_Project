#include "aircraft_manager.hpp"

#include "crashed_aircraft.hpp"

void AircraftManager::add(std::unique_ptr<Aircraft>& aircraft)
{
    assert(aircraft != nullptr);
    aircrafts.emplace_back(std::move(aircraft));
}

void AircraftManager::move(float dt)
{
    assert(dt >= 0.0f);
    std::sort(aircrafts.begin(), aircrafts.end(), [](const auto& a, const auto& b) {
        if (a->has_terminal() && !b->has_terminal()) return true;
        if (b->has_terminal() && !a->has_terminal()) return false;
        return a->fuel < b->fuel;
    });

    aircrafts.erase(std::remove_if(aircrafts.begin(), aircrafts.end(), [this, dt](const auto &aircraft) {
       if (aircraft->finished) return true;
       try {
           aircraft->move(dt);
           return false;
       } catch (const AircraftCrash& e) {
           std::cerr << e.what() << std::endl;
           crashed_aircrafts++;
           auto pos = aircraft->position();
           auto crash = std::make_unique<CrashedAircraft>(pos);
           crashed_aircrafts_vector.emplace_back(std::move(crash));
           return true;
       }
    }), aircrafts.end());
}

long AircraftManager::get_required_fuel() const
{
    return std::accumulate(aircrafts.begin(), aircrafts.end(), 0, [](long acc, const auto& aircraft) {
        return acc + (!aircraft->serviced && aircraft->is_low_on_fuel() ? Aircraft::FUEL_THRESHOLD - aircraft->fuel : 0);
    });
}
