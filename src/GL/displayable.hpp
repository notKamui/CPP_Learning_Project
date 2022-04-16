#pragma once

#include <algorithm>
#include <cassert>
#include <vector>

namespace GL {

// a displayable object can be displayed and has a z-coordinate indicating who
// is displayed before whom ;]
class Displayable;

inline std::vector<const Displayable*> display_queue;

class Displayable
{
protected:
    float z = 0;

public:
    Displayable(const float z_) : z { z_ } {
        display_queue.emplace_back(this);
    }
    virtual ~Displayable() {
        // remove this from display_queue
        display_queue.erase(std::find(display_queue.begin(), display_queue.end(), this));
    }

    virtual void display() const = 0;

    float get_z() const { return z; }
};

struct disp_z_cmp
{
    bool operator()(const Displayable* a, const Displayable* b) const
    {
        assert(a != nullptr && b != nullptr);
        const auto az = a->get_z();
        const auto bz = b->get_z();
        return (az == bz) ? (a > b) : (az > bz);
    }
};

} // namespace GL
