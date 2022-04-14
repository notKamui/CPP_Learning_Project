#include "AircraftManager.hpp"

void AircraftManager::add(std::unique_ptr<Aircraft>& aircraft)
{
    aircrafts.emplace_back(std::move(aircraft));
}

void AircraftManager::move(float dt)
{
    for (auto it = aircrafts.begin(); it != aircrafts.end();)
    {
        if (!(*it)->finished)
        {
            (*it)->move(dt);
            ++it;
        }
        else
        {
            it = aircrafts.erase(it);
        }
    }
}
