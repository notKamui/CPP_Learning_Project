#pragma once

#include <algorithm>
#include <numeric>
#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <type_traits>

template <typename T, size_t dim>
struct Point
{
    std::array<std::enable_if_t<std::is_arithmetic_v<T>, T>, dim> values;

    Point() {};

    template<typename... Ts>
    explicit Point(Ts&&... args)
        : values { static_cast<T>(args)...}
    {
        static_assert(sizeof...(Ts) == dim, "dim must match the number of arguments");
    }

    /*Point(float x, float y) : values { x, y } {
        static_assert(dim == 2, "dim must be 2");
        std::cout << "Point(float x, float y)" << std::endl;
    }

    Point(float x, float y, float z) : values { x, y, z } {
        static_assert(dim == 3, "dim must be 3");
    }*/

    float& x() {
        static_assert(dim >= 1);
        return values[0];
    }
    float x() const {
        static_assert(dim >= 1);
        return values[0];
    }

    float& y() {
        static_assert(dim >= 2);
        return values[1];
    }
    float y() const {
        static_assert(dim >= 2);
        return values[1];
    }

    float& z() {
        static_assert(dim >= 3);
        return values[2];
    }
    float z() const {
        static_assert(dim >= 3);
        return values[2];
    }

    Point& operator+=(const Point& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::plus<>());
        return *this;
    }

    Point& operator-=(const Point& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::minus<>());
        return *this;
    }

    Point& operator*=(const Point& other)
    {
        std::transform(values.begin(), values.end(), other.values.begin(), values.begin(), std::multiplies<>());
        return *this;
    }

    Point& operator*=(const float scalar)
    {
        std::transform(values.begin(), values.end(), values.begin(), [scalar](const auto& v) { return v * scalar; });
        return *this;
    }

    Point operator+(const Point& other) const
    {
        Point result = *this;
        result += other;
        return result;
    }

    Point operator-(const Point& other) const
    {
        Point result = *this;
        result -= other;
        return result;
    }

    Point operator*(const Point& other) const
    {
        Point result = *this;
        result *= other;
        return result;
    }

    Point operator*(const float scalar) const
    {
        Point result = *this;
        result *= scalar;
        return result;
    }

    Point operator-() const
    {
        Point tmp = {};
        std::transform(values.begin(), values.end(), tmp.values.begin(), [](T v) { return -v; });
        return tmp;
    }

    float length() const {
        return std::sqrt(std::inner_product(values.begin(), values.end(), values.begin(), T{}));
    }

    float distance_to(const Point& other) const { return (*this - other).length(); }

    Point& normalize(const float target_len = 1.0f)
    {
        const float current_len = length();
        if (current_len == 0)
        {
            throw std::logic_error("cannot normalize vector of length 0");
        }

        *this *= (target_len / current_len);
        return *this;
    }

    Point& cap_length(const float max_len)
    {
        assert(max_len > 0);

        const float current_len = length();
        if (current_len > max_len)
        {
            *this *= (max_len / current_len);
        }

        return *this;
    }

    const std::string s() const {
        std::string s = "[";
        for (size_t i = 0; i < dim; ++i) {
            s += std::to_string(values[i]);
            if (i < dim - 1) {
                s += ", ";
            }
        }
        return s + "]";
    }
};

using Point2D = Point<float, 2>;
using Point3D = Point<float, 3>;

// our 3D-coordinate system will be tied to the airport: the runway is parallel to the x-axis, the z-axis
// points towards the sky, and y is perpendicular to both thus,
// {1,0,0} --> {.5,.5}   {0,1,0} --> {-.5,.5}   {0,0,1} --> {0,1}
inline Point2D project_2D(const Point3D& p)
{
    return Point2D { .5f * p.x() - .5f * p.y(), .5f * p.x() + .5f * p.y() + p.z() };
}
