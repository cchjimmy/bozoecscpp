#include "world.hpp"
#include <iostream>

typedef struct {
  float x, y;
} Position;

typedef struct {
  float x, y;
} Velocity;

void addEntityP(World &w) {
  const entityT e = w.addEntity();
  w.addComponent<Position>(e);
}
void addEntityV(World &w) {
  const entityT e = w.addEntity();
  w.addComponent<Velocity>(e);
}
void addEntityPV(World &w) {
  const entityT e = w.addEntity();
  w.addComponent<Position>(e);
  w.addComponent<Velocity>(e);
}

void Move(World &w) {
  for (auto &e : w.query<And<Position, Velocity>>()) {
    Position &p = w.getComponent<Position>(e);
    Velocity &v = w.getComponent<Velocity>(e);
    p.x += v.x * w.dtMicro / 1e6;
    p.y += v.y * w.dtMicro / 1e6;
    std::cout << "time: " << w.timeMicro / 1e6 << ", ";
    std::cout << "dt: " << w.dtMicro / 1e6 << ", ";
    std::cout << "Entity " << e << " Position: " << p.x << ", " << p.y << "\n";
  }
}

int main() {
  World w;
  for (int i = 0, l = 10; i < l; i++) {
    addEntityP(w);
  }
  for (int i = 0, l = 20; i < l; i++) {
    addEntityV(w);
  }
  for (int i = 0, l = 1; i < l; i++) {
    addEntityPV(w);
  }
  while (true)
    w.update(Move);
}
