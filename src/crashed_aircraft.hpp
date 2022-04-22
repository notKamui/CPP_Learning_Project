#pragma once

#include <iostream>
#include "GL/texture.hpp"
#include "GL/displayable.hpp"
#include "img/image.hpp"

class CrashedAircraft : public GL::Displayable
{
private:
    const GL::Texture2D sprite;
    Point3D pos;

public:
    CrashedAircraft(const Point3D pos_)
    : GL::Displayable { pos_.x() + pos_.y() }
    , sprite { new img::Image { crashed_aircraft_sprite_path.get_full_path() } }
    , pos { pos_ }
    {
        pos.z() = 0;
        std::cout << "Crash detected" << std::endl;
    };

    void display() const override { sprite.draw(project_2D(pos), Point2D { .3f, .3f }); }
};