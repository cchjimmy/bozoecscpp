#include "world.hpp"
#include "entity.hpp"
#include <cstdarg>
#include <ctime>
#include <optional>
#include <vector>

std::vector<World *> World::mWorlds;
std::unordered_map<entityT, int> World::mEntityMasks;
std::unordered_map<int, std::set<entityT>> World::mArchetypeMap = {
    {0, std::set<int>()}};
std::unordered_map<std::type_index, void (*)(entityT)> World::mRemoveMap;
std::vector<entityT> World::mEntitiesToDelete;

World::~World() {
  mWorlds.clear();
  mEntityMasks.clear();
  mArchetypeMap.clear();
}

World::World() { mWorlds.push_back(this); }

entityT World::createEntity() {
  const entityT e = rand();
  mEntityMasks[e] = 0;
  mArchetypeMap[0].insert(e);
  return e;
}

void World::deleteEntity(entityT entity) {
  mEntitiesToDelete.push_back(entity);
}

entityT World::addEntity(entityT entity) {
  mLocalEntities.insert(entity);
  return entity;
}

size_t World::entityCount() { return mLocalEntities.size(); }

void World::removeEntity(entityT entity) { mLocalEntities.erase(entity); }
