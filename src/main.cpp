#include "tower_sim.hpp"

void test_generic_points() {
    Point<float, 4> p1;
    Point<float, 4> p2;
    auto p3 = p1 + p2;
    p1 += p2;
    p1 *= 3; // ou 3.f, ou 3.0 en fonction du type de Point
    p3 * 5;

    // Point2D { 1, 2, 3};
    Point<float, 4> p4 = p1;
    //Point<std::string, 2> p5;

    Point2D p6 = { 1, 2};
    std::cout << p6.s() << std::endl;
}

int main(int argc, char** argv)
{
    test_generic_points();
    TowerSimulation simulation { argc, argv };
    simulation.launch();

    return 0;
}