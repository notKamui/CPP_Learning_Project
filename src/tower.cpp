#include "tower.hpp"

#include "airport.hpp"
#include "terminal.hpp"

#include <cassert>

WaypointQueue Tower::get_circle() const
{
    return { { Point3D { -1.5f, -1.5f, .5f }, wp_air },
             { Point3D { 1.5f, -1.5f, .5f }, wp_air },
             { Point3D { 1.5f, 1.5f, .5f }, wp_air },
             { Point3D { -1.5f, 1.5f, .5f }, wp_circle } };
}

WaypointQueue Tower::get_instructions(Aircraft& aircraft)
{
    if (!aircraft.is_at_terminal)
    {
        return get_circle();
    }
    else
    {
        // get a path for the craft to start
        const auto terminal_num = reserved_terminals[&aircraft];
        Terminal& terminal      = airport.get_terminal(terminal_num);
        if (!terminal.is_servicing())
        {
            terminal.finish_service();
            reserved_terminals.erase(&aircraft);
            aircraft.is_at_terminal = false;
            aircraft.serviced = true;
            return airport.start_path(terminal_num);
        }
        else
        {
            return {};
        }
    }
}

WaypointQueue Tower::reserve_terminal(Aircraft& aircraft)
{
    // try and reserve a terminal for the craft to land
    const auto vp = airport.reserve_terminal(aircraft);
    if (!vp.first.empty())
    {
        reserved_terminals[&aircraft] = vp.second;
        return vp.first;
    }
    return {};
}

void Tower::arrived_at_terminal(const Aircraft& aircraft)
{
    auto t = reserved_terminals.find(&aircraft);
    if (t == reserved_terminals.end()) return;
    auto t_id = t->second;
    airport.get_terminal(t_id).start_service(aircraft);
}

void Tower::release_terminal(const Aircraft &aircraft)
{
    auto t = reserved_terminals.find(&aircraft);
    if (t == reserved_terminals.end()) return;
    auto t_id = t->second;
    airport.get_terminal(t_id).release();
    reserved_terminals.erase(t);
}
