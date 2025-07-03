#include "world.hpp"
#include "entity.hpp"
#include <cstdarg>
#include <ctime>
#include <optional>
#include <vector>

std::vector<World *> World::mWorlds;
std::unordered_map<std::type_index, std::unordered_map<entityT, int>>
    World::mIndexMap;
std::unordered_map<entityT, int> World::mEntityMasks;
std::unordered_map<int, std::set<entityT>> World::mArchetypeMap = {
    {0, std::set<int>()}};
std::unordered_map<int, void (*)(entityT)> World::mRemoveMap;

World::~World() {
  mWorlds.clear();
  mIndexMap.clear();
  mEntityMasks.clear();
  mArchetypeMap.clear();
}

World::World() {
  mWorlds.push_back(this);
  dtMicro = timeMicro = 0;
}

entityT World::createEntity() {
  const entityT e = EntityManager::create();
  mEntityMasks[e] = 0;
  mArchetypeMap[0].insert(e);
  return e;
}

entityT World::deleteEntity(entityT entity) {
  for (auto &world : mWorlds) {
    world->removeEntity(entity);
  }

  const auto &types = ComponentManager::types();
  for (int i = 0, l = types.size(), m = mEntityMasks[entity]; i < l; i++) {
    if (!(m & 1 << i))
      continue;
    mRemoveMap[i](entity);
  }
  EntityManager::remove(EntityManager::find(entity));
  return entity;
}

entityT World::addEntity(std::optional<entityT> entity) {
  if (!entity.has_value())
    entity = createEntity();
  mLocalEntities.insert(entity.value());
  return entity.value();
}

size_t World::entityCount() { return mLocalEntities.size(); }

bool World::entityExists(entityT entity) {
  return mLocalEntities.contains(entity);
}

entityT World::removeEntity(entityT entity) {
  mLocalEntities.erase(entity);
  return entity;
}
