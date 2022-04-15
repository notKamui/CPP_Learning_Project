#include "aircraft_manager.hpp"

void AircraftManager::add(std::unique_ptr<Aircraft>& aircraft)
{
    aircrafts.emplace_back(std::move(aircraft));
}

void AircraftManager::move(float dt)
{
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
