#include <cstdint>
#define SDL_MAIN_USE_CALLBACKS 1

#include "world.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
// #include <SDL3_ttf/SDL_ttf.h>

static SDL_Window *window;
static SDL_Renderer *renderer;
static World game;

// components
struct Transform {
  float x, y, scaleX = 1, scaleY = 1;
};
struct Color {
  int r, g, b, a = 255;
};
struct Rect {
  float w, h;
};
struct Velocity {
  float x, y;
};

// singletons
namespace TimeSeconds {
float accum, delta;
};

// systems
void handleDrawRect(World &world) {
  for (auto e : world.query<And<Transform, Rect, Color>>()) {
    auto &t = world.getComponent<Transform>(e);
    auto &r = world.getComponent<Rect>(e);
    auto &c = world.getComponent<Color>(e);
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_FRect rect{t.x - r.w / 2, t.y - r.h / 2, r.w, r.h};
    SDL_RenderFillRect(renderer, &rect);
  }
}
void handleMovement(World &world) {
  for (auto e : world.query<And<Transform, Velocity>>()) {
    auto &t = world.getComponent<Transform>(e);
    auto &v = world.getComponent<Velocity>(e);
    t.x += v.x * TimeSeconds::delta;
    t.y += v.y * TimeSeconds::delta;
  }
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  if (!SDL_Init(SDL_INIT_VIDEO))
    return SDL_APP_FAILURE;
  if (!SDL_CreateWindowAndRenderer("withSdl3", 900, 600, 0, &window, &renderer))
    return SDL_APP_FAILURE;

  for (int i = 0; i < 1; i++) {
    auto e = game.addEntity();
    game.addComponent(e, Transform{.x = 450, .y = 300});
    game.addComponent(e, Rect{100, 100});
    game.addComponent(e, Color{100, 200, 100, 255});
    game.addComponent(e, Velocity{10, 0});
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  default:
    return SDL_APP_CONTINUE;
  }
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  uint64_t past = SDL_GetTicks();
  SDL_SetRenderDrawColor(renderer, 33, 33, 33, 255);
  SDL_RenderClear(renderer);
  game.update(handleDrawRect, handleMovement);
  SDL_RenderPresent(renderer);
  TimeSeconds::delta = (SDL_GetTicks() - past) / 1000.f;
  TimeSeconds::accum += TimeSeconds::delta;
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
