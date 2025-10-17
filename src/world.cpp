#include "world.hpp"
#include "entity.hpp"
#include <cstdarg>
#include <ctime>
#include <vector>

std::vector<World *> World::mWorlds;
std::unordered_map<entityT, int> World::mEntityMasks;
std::unordered_map<int, std::set<entityT>> World::mArchetypeMap;
std::vector<entityT> World::mEntitiesToDelete;

World::~World() {
  mWorlds.clear();
  mEntityMasks.clear();
  mArchetypeMap.clear();
}

World::World() { mWorlds.push_back(this); }
