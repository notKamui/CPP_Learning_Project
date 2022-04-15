#include "aircraft_manager.hpp"

void AircraftManager::add(std::unique_ptr<Aircraft>& aircraft)
{
    aircrafts.emplace_back(std::move(aircraft));
}

void AircraftManager::move(float dt)
{
    std::sort(aircrafts.begin(), aircrafts.end(), [](const auto& a, const auto& b) {
        if (a->has_terminal()) {
            return true;
        } else if (b->has_terminal()) {
            return false;
        } else {
            return a->fuel < b->fuel;
        }
    });

    aircrafts.erase(std::remove_if(aircrafts.begin(), aircrafts.end(), [dt](const auto &aircraft) {
       if (aircraft->finished) {
           return true;
       }
       else
       {
           aircraft->move(dt);
           return false;
       }
    }), aircrafts.end());
}
