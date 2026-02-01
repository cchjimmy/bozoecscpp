#include "world.hpp"
#include <cassert>
#include <iostream>
#include <vector>

typedef struct Position {
  int x, y;
} Position;

typedef struct Velocity {
  int x, y;
} Velocity;

int main() {
  std::cout << "-- add component --" << std::endl;
  {
    World test;
    auto e = test.addEntity();
    auto &p = test.addComponent<Position>(e, {.x = 10});
    assert(p.x == 10 && p.y == 0);
  }

  std::cout << "-- get component --" << std::endl;
  {
    World test;
    auto e = test.addEntity();
    auto &p = test.addComponent<Position>(e);
    auto &p1 = test.getComponent<Position>(e);
    assert(&p == &p1);
  }

  std::cout << "-- get and modify component --" << std::endl;
  {
    World test;
    auto e = test.addEntity();
    auto &p = test.addComponent<Position>(e, {.x = 10});
    p.y = 20;
    auto &p2 = test.getComponent<Position>(e);
    assert(p2.x == 10 && p2.y == 20 && &p == &p2);
  }

  std::cout << "-- remove component --" << std::endl;
  {
    World test;
    auto e1 = test.addEntity();
    auto &p1 = test.addComponent<Position>(e1, {.x = 3, .y = 1});
    test.removeComponent<Position>(e1);
    assert(test.hasComponent<Position>(e1) == false);
  }

  std::cout << "-- copy entity --" << std::endl;
  {
    World test;
    auto e = test.addEntity();
    auto &p = test.addComponent<Position>(e, {.x = 10, .y = 1});
    auto e1 = test.copy(e);
    auto &p1 = test.getComponent<Position>(e1);
    assert(p1.x == 10 && p1.y == 1);
  }

  std::cout << "-- add entity --" << std::endl;
  {
    World test;
    for (int i = 0; i < 10; i++) {
      auto e = test.addEntity();
    }
    assert(test.entityCount() == 10);
  }

  std::cout << "-- query --" << std::endl;
  {
    World test;
    for (int i = 0; i < 10; i++) {
      auto e = test.addEntity();
      auto &p = test.addComponent<Position>(e, {.x = 10, .y = 1});
    }

    typedef struct {
    } NotInUse;

    auto e = test.addEntity();
    test.addComponent<Position>(e);
    test.addComponent<Velocity>(e);

    auto q1 = test.query<And<Position>>();
    auto q2 = test.query<And<Position, Velocity>>();
    auto q3 = test.query<Not<Velocity>>();
    auto q4 = test.query<And<Position, NotInUse>>();
    assert(q1.size() == 11 && q2.size() == 1 && q3.size() == 10 &&
           q4.size() == 0);
  }
}
