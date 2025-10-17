#include "query.hpp"
#include "world.hpp"
#include <cassert>
#include <iostream>

typedef struct {
  int x, y;
} Position;

typedef struct {
  int x, y;
} Velocity;

int main() {
  std::cout << "-- add test --" << std::endl;
  {
    World test;
    auto e = test.addEntity();
    auto &p = World::addComponent<Position>(e, {.x = 10, .y = 1});
    assert(p.x == 10 && p.y == 1);
  }

  std::cout << "-- remove test --" << std::endl;
  {
    World test;
    auto e = test.addEntity();
    auto &p = World::addComponent<Position>(e, {.x = 10, .y = 1});
    World::removeComponent<Position>(e);
    p = World::addComponent<Position>(e);
    assert(p.x == 0 && p.y == 0);
  }

  std::cout << "-- copy test --" << std::endl;
  {
    World test;
    auto e = test.addEntity();
    auto &p = World::addComponent<Position>(e, {.x = 10, .y = 1});
    auto e1 = test.copy(e);
    auto &p1 = World::getComponent<Position>(e1);
    assert(p.x == p1.x && p.y == p1.y);
  }

  std::cout << "-- add entity test --" << std::endl;
  {
    World test;
    for (int i = 0; i < 10; i++) {
      auto e = test.addEntity();
    }
    assert(test.entityCount() == 10);
  }

  std::cout << "-- query test --" << std::endl;
  {
    World test;
    for (int i = 0; i < 10; i++) {
      auto e = test.addEntity();
      auto &p = World::addComponent<Position>(e, {.x = 10, .y = 1});
    }
    auto e = test.addEntity();
    World::addComponent<Position>(e);
    World::addComponent<Velocity>(e);
    auto q = test.query<And<Position>>();
    auto q1 = test.query<And<Position, Velocity>>();
    assert(q.size() == 11);
    assert(q1.size() == 1);
  }
}
