#include "raylib.h"
#include "world.hpp"
#include <format>
#include <fstream>
#include <iostream>
#include <string>

// components
struct Velocity {
  float x, y;
};
struct IsPlayer {};
struct Circle {
  float radius = 5;
};
struct TransformSimple {
  float x, y, rad;
};
struct IsFood {};
struct Hierarchy {
  entityT parent = -1, child = -1;
};
struct Text {
  std::string content;
  int fontSize;
  Color color;
};

// singletons
namespace Config {
std::string recordPath = "record.txt";
int maxScore;

namespace viewport {
int width = 900;
int height = 600;
}; // namespace viewport

namespace snake {
int width = 30;
int speed = 150;
float angularSpeed = PI;
Color color = {.r = 200, .g = 160, .b = 160, .a = 255};
int startLen = 5;
}; // namespace snake

namespace food {
float radius = 30;
Color color = {.r = 255, .g = 100, .b = 100, .a = 255};
}; // namespace food
}; // namespace Config

// utils
int calcSnakeSize(World world, entityT from) {
  int size = 1;
  if (!world.hasComponent<Hierarchy>(from))
    return size;
  auto h = world.getComponent<Hierarchy>(from);
  while (h.child != -1) {
    size++;
    if (!world.hasComponent<Hierarchy>(h.child))
      break;
    h = world.getComponent<Hierarchy>(h.child);
  }
  return size;
}
float inline randRange(float max = 1, float min = 0) {
  return (float)rand() / RAND_MAX * (max - min) + min;
}
bool inline circlesOverlapped(Circle c1, TransformSimple t1, Circle c2,
                              TransformSimple t2) {
  return (t1.x - t2.x) * (t1.x - t2.x) + (t1.y - t2.y) * (t1.y - t2.y) <
         (c1.radius + c2.radius) * (c1.radius + c2.radius);
}
entityT findLastChild(World &world, entityT parent) {
  auto h = world.getComponent<Hierarchy>(parent);
  while (h.child != -1) {
    if (world.hasComponent<Hierarchy>(h.child) && parent != h.child) {
      parent = h.child;
      h = world.getComponent<Hierarchy>(h.child);
    } else {
      break;
    }
  }
  return parent;
}

// systems
void handleDrawSnake(World &world) {
  for (auto e : world.query<And<Hierarchy, TransformSimple, Color>>()) {
    auto &h = world.getComponent<Hierarchy>(e);
    if (h.child == -1 || !world.hasComponent<TransformSimple>(h.child))
      continue;
    auto &t = world.getComponent<TransformSimple>(e);
    auto &c = world.getComponent<Color>(e);
    auto &tChild = world.getComponent<TransformSimple>(h.child);
    DrawLineEx({.x = t.x, .y = t.y}, {.x = tChild.x, .y = tChild.y},
               Config::snake::width, c);
  }
}
void handleDrawText(World &world) {
  for (auto &e : world.query<And<Text, TransformSimple>>()) {
    const auto &text = world.getComponent<Text>(e);
    const auto &t = world.getComponent<TransformSimple>(e);
    DrawText(text.content.c_str(), t.x, t.y, text.fontSize, text.color);
  }
}
void handleDrawCircles(World &world) {
  for (auto &e : world.query<And<Circle, TransformSimple>>()) {
    const auto &c = world.getComponent<Circle>(e);
    const auto &t = world.getComponent<TransformSimple>(e);
    const auto &color =
        world.hasComponent<Color>(e) ? world.getComponent<Color>(e) : WHITE;
    DrawCircle(t.x, t.y, c.radius, color);
  }
}
void handleMovement(World &world) {
  for (auto &e : world.query<And<TransformSimple, Velocity>>()) {
    auto &t = world.getComponent<TransformSimple>(e);
    auto &v = world.getComponent<Velocity>(e);
    t.x += v.x * GetFrameTime();
    t.y += v.y * GetFrameTime();
  }
}
void handleInput(World &world) {
  for (auto &e : world.query<And<IsPlayer, Velocity>>()) {
    float angularVel = 0;
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
      angularVel -= Config::snake::angularSpeed;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
      angularVel += Config::snake::angularSpeed;
    angularVel *= GetFrameTime();
    auto &v = world.getComponent<Velocity>(e);
    const float c = cos(angularVel);
    const float s = sin(angularVel);
    const float x = v.x, y = v.y;
    v.x = c * x - s * y;
    v.y = s * x + c * y;
  }
}
void handleSpawnFood(World &world) {
  if (world.query<And<IsFood>>().size() > 0)
    return;
  // spawn food
  const auto food = world.addEntity();
  world.addComponent(food, IsFood{});
  world.addComponent(
      food, TransformSimple{.x = randRange() * Config::viewport::width,
                            .y = randRange() * Config::viewport::height});
  world.addComponent(food, Circle{.radius = Config::food::radius});
  world.addComponent(food, Config::food::color);
}
void handleEatFood(World &world) {
  for (auto &e : world.query<And<Hierarchy, Circle, TransformSimple>>()) {
    auto &h = world.getComponent<Hierarchy>(e);
    if (h.parent != -1) // find leading body part
      continue;
    const auto &t = world.getComponent<TransformSimple>(e);
    const auto &c = world.getComponent<Circle>(e);
    for (auto &food : world.query<And<IsFood, Circle, TransformSimple>>()) {
      const auto &foodT = world.getComponent<TransformSimple>(food);
      const auto &foodC = world.getComponent<Circle>(food);
      if (!circlesOverlapped(c, t, foodC, foodT))
        continue;
      world.deleteEntity(food);

      // spawn new body part
      const auto parent = findLastChild(world, e);
      const auto part = world.addEntity();
      world.getComponent<Hierarchy>(parent).child = part;
      world.addComponent(
          part, world.hasComponent<Circle>(parent)
                    ? world.getComponent<Circle>(parent)
                    : Circle{.radius = (float)Config::snake::width / 2});
      world.addComponent(part, world.hasComponent<Color>(parent)
                                   ? world.getComponent<Color>(parent)
                                   : WHITE);
      world.addComponent(part, world.hasComponent<TransformSimple>(parent)
                                   ? world.getComponent<TransformSimple>(parent)
                                   : TransformSimple{});
      world.addComponent(part, Hierarchy{.parent = parent});
      world.addComponent(part, Velocity{});
    }
  }
}
void handlePartMovement(World &world) {
  for (auto &e : world.query<And<Hierarchy, Velocity, TransformSimple>>()) {
    const auto &h = world.getComponent<Hierarchy>(e);
    if (h.parent == -1)
      continue;
    auto &t = world.getComponent<TransformSimple>(e);
    if (!world.hasComponent<TransformSimple>(h.parent))
      continue;
    const auto &tParent = world.getComponent<TransformSimple>(h.parent);
    const float dx = tParent.x - t.x;
    const float dy = tParent.y - t.y;
    if (dx * dx + dy * dy < Config::snake::width * Config::snake::width)
      continue;
    auto &v = world.getComponent<Velocity>(e);
    const float mag = sqrtf(dx * dx + dy * dy);
    v.x = dx / mag * Config::snake::speed;
    v.y = dy / mag * Config::snake::speed;
  }
}
void handleReset(World &world) {
  entityT player = -1;
  for (auto &e :
       world.query<And<IsPlayer, Hierarchy, Circle, TransformSimple>>()) {
    player = e;
    break;
  }
  if (player == -1) {
    player = world.addEntity();
    world.addComponent(player,
                       Circle{.radius = (float)Config::snake::width / 2});
    world.addComponent(player, Velocity{.x = (float)Config::snake::speed});
    world.addComponent(player, IsPlayer{});
    world.addComponent(player,
                       TransformSimple{.x = Config::viewport::width * 0.5f,
                                       .y = Config::viewport::height * 0.5f});
    world.addComponent(player, Config::snake::color);
    world.addComponent(player, Hierarchy{});
  }
  const auto &h = world.getComponent<Hierarchy>(player);
  const auto &c = world.getComponent<Circle>(player);
  const auto &t = world.getComponent<TransformSimple>(player);
  entityT collidedBody = -1;
  for (auto &other : world.query<And<Hierarchy, TransformSimple, Circle>>()) {
    if (other == player || other == h.child)
      continue;
    const auto &cOther = world.getComponent<Circle>(other);
    const auto &tOther = world.getComponent<TransformSimple>(other);
    if (!circlesOverlapped(c, t, cOther, tOther))
      continue;
    collidedBody = other;
    break;
  }
  if (t.x < (float)Config::snake::width / 2 ||
      t.x > Config::viewport::width - (float)Config::snake::width / 2 ||
      t.y < (float)Config::snake::width / 2 ||
      t.y > Config::viewport::height - (float)Config::snake::width / 2 ||
      collidedBody != -1 || world.entityCount() < Config::snake::startLen ||
      IsKeyPressed(KEY_R)) {

    int currentScore = calcSnakeSize(world, player) - Config::snake::startLen;
    std::fstream file;
    file.open(Config::recordPath, std::ios::in);
    if (file.is_open()) {
      std::string content;
      file >> content;
      Config::maxScore = content.size() > 0 ? std::stoi(content) : 0;
      Config::maxScore =
          Config::maxScore > currentScore ? Config::maxScore : currentScore;
    } else {
      std::cout << "cannot open file" << std::endl;
    }
    file.close();

    file.open(Config::recordPath, std::ios::out | std::ios::trunc);
    if (file.is_open()) {
      file << Config::maxScore;
    } else {
      std::cout << "cannot open file" << std::endl;
    }
    file.close();

    for (auto &e : world.query<Not<IsPlayer, Text>>()) {
      world.deleteEntity(e);
    }

    world.addComponent<Hierarchy>(player);
    auto &tPlayer = world.getComponent<TransformSimple>(player);
    tPlayer.x = (float)Config::viewport::width / 2;
    tPlayer.y = (float)Config::viewport::height / 2;
    auto &vPlayer = world.getComponent<Velocity>(player);
    vPlayer.x = Config::snake::speed;
    vPlayer.y = 0;
    auto parent = player;
    int sl = Config::snake::startLen - 1;
    while (sl) {
      const auto part = world.addEntity();
      world.getComponent<Hierarchy>(parent).child = part;
      world.addComponent(part,
                         Circle{.radius = (float)Config::snake::width / 2});
      world.addComponent(part, Velocity{});
      auto t = world.getComponent<TransformSimple>(parent);
      t.x -= Config::snake::width;
      world.addComponent(part, t);
      world.addComponent(part, Config::snake::color);
      world.addComponent(part, Hierarchy{.parent = parent});
      parent = part;
      sl--;
    }
  }
}

int main() {
  SetTargetFPS(60);
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(Config::viewport::width, Config::viewport::height, "snake");

  srand(time(NULL));

  World game;

  const auto score = game.addEntity();
  game.addComponent(score,
                    Text{.content = "test", .fontSize = 20, .color = WHITE});
  game.addComponent(score, TransformSimple{.x = 10, .y = 10});

  RenderTexture2D renderTarget =
      LoadRenderTexture(Config::viewport::width, Config::viewport::height);

  while (!WindowShouldClose()) {
    BeginTextureMode(renderTarget);

    ClearBackground({.r = 30, .g = 30, .b = 30, .a = 255});
    DrawFPS(0, 0);

    entityT player = -1;
    for (auto e : game.query<And<IsPlayer>>()) {
      player = e;
      break;
    }
    game.getComponent<Text>(score).content = std::format(
        "Max: {}; Score: {}", Config::maxScore,
        player != -1 ? calcSnakeSize(game, player) - Config::snake::startLen
                     : 0);

    game.update(handleDrawCircles, handleDrawSnake, handleDrawText, handleReset,
                handleInput, handleSpawnFood, handleEatFood, handlePartMovement,
                handleMovement);

    EndTextureMode();

    Vector2 offset = {};
    float scale = 1;
    if (Config::viewport::width / Config::viewport::height >
        GetScreenWidth() / GetScreenHeight()) {
      scale = (float)GetScreenWidth() / Config::viewport::width;
      offset.y =
          (float)(GetScreenHeight() - Config::viewport::height * scale) / 2;
    } else {
      scale = (float)GetScreenHeight() / Config::viewport::height;
      offset.x =
          (float)(GetScreenWidth() - Config::viewport::width * scale) / 2;
    }

    BeginDrawing();

    DrawTexturePro(renderTarget.texture,
                   {.width = (float)Config::viewport::width,
                    .height = -(float)Config::viewport::height},
                   {
                       .x = offset.x,
                       .y = offset.y,
                       .width = Config::viewport::width * scale,
                       .height = Config::viewport::height * scale,
                   },
                   {}, 0, WHITE);

    EndDrawing();
  }

  UnloadRenderTexture(renderTarget);
  CloseWindow();
}
