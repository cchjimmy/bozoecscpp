#include "raylib.h"
#include "world.hpp"
#include <cmath>
#include <cstdlib>
#include <string>

#define PARTICLE_COUNT 1000

typedef struct {
  float x, y;
} Position;

typedef struct {
  float x, y;
} Velocity;

typedef struct {
  float width, height;
} Rect;

auto addParticle(World &world, int x = 0, int y = 0, int w = 10, int h = 10) {
  const auto e = world.addEntity();
  World::addComponent<Velocity>(e);
  World::addComponent<Color>(e);
  auto &p = World::addComponent<Position>(e);
  auto &r = World::addComponent<Rect>(e);
  p.x = x, p.y = y, r.width = w, r.height = h;
  return e;
}

void handleDrawing(World &world) {
  for (auto e : world.query<And<Position, Rect>>()) {
    const auto &p = World::getComponent<Position>(e);
    const auto &r = World::getComponent<Rect>(e);
    const auto &c = World::getComponent<Color>(e);
    DrawRectangleLines(p.x - r.width * 0.5, p.y - r.height * 0.5, r.width,
                       r.height, c);
  }

  DrawText((std::string("FPS: ") + std::to_string(1e6 / world.dtMicro)).c_str(),
           0, 0, 24, BLACK);
}

void handleMovement(World &world) {
  for (auto e : world.query<And<Position, Velocity>>()) {
    auto &p = World::getComponent<Position>(e);
    auto &v = World::getComponent<Velocity>(e);
    p.x += v.x * world.dtMicro / 1e6;
    p.y += v.y * world.dtMicro / 1e6;
  }
}

void handleCollision(World &world) {
  const auto screenWidth = GetScreenWidth();
  const auto screenHeight = GetScreenHeight();
  for (auto e : world.query<And<Position, Velocity, Rect>>()) {
    auto &p = World::getComponent<Position>(e);
    auto &v = World::getComponent<Velocity>(e);
    const auto &r = World::getComponent<Rect>(e);
    if (p.x - r.width * 0.5 < 0 || p.x + r.width * 0.5 > screenWidth) {
      v.x *= -1;
      p.x =
          p.x < screenWidth * 0.5 ? r.width * 0.5 : screenWidth - r.width * 0.5;
    }
    if (p.y - r.height * 0.5 < 0 || p.y + r.height * 0.5 > screenHeight) {
      v.y *= -1;
      p.y = p.y < screenHeight * 0.5 ? r.height * 0.5
                                     : screenHeight - r.height * 0.5;
    }
  }
}

float random(float max = 1, float min = 0) {
  return (float)rand() / RAND_MAX * (max - min) + min;
}

int main() {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(300, 150, "particles");
  World game;
  const auto screenWidth = GetScreenWidth();
  const auto screenHeight = GetScreenHeight();
  for (int i = 0; i < PARTICLE_COUNT; i++) {
    const auto e = addParticle(game);
    auto &p = World::getComponent<Position>(e);
    auto &v = World::getComponent<Velocity>(e);
    auto &c = World::getComponent<Color>(e);
    auto &r = World::getComponent<Rect>(e);
    p.x = random(screenWidth);
    p.y = random(screenHeight);
    float dir = random(2 * PI);
    int mag = 100;
    v.x = cos(dir) * mag;
    v.y = sin(dir) * mag;
    c.r = random(255);
    c.g = random(255);
    c.b = random(255);
    c.a = 255;
    r.width = random(50, 10);
    r.height = random(50, 10);
  }
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(GRAY);
    game.update(handleDrawing, handleMovement, handleCollision);
    EndDrawing();
  }
  CloseWindow();
}
