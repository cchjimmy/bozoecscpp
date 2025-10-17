#pragma once
#include "component.hpp"
#include "entity.hpp"
#include "query.hpp"
#include <iterator>
#include <set>
#include <unordered_map>
#include <vector>

class World {
public:
  template <typename T> static inline void registerComponent() {
    if (ComponentManager::isRegistered<T>())
      return;
    ComponentManager::reg<T>();
  }

  template <typename T>
  static inline T &addComponent(entityT entity, T values = {}) {
    registerComponent<T>();
    if (hasComponent<T>(entity)) {
      T &comp = ComponentManager::add<T>(entity);
      comp = values;
      return comp;
    }
    T &comp = ComponentManager::add<T>(entity);
    comp = values;
    mArchetypeMap[mEntityMasks[entity]].erase(entity);
    mEntityMasks[entity] |= 1 << ComponentManager::getId<T>();
    mArchetypeMap[mEntityMasks[entity]].insert(entity);
    return comp;
  }

  template <typename T> static inline bool removeComponent(entityT entity) {
    if (!hasComponent<T>(entity))
      return false;
    ComponentManager::remove<T>(entity);
    mArchetypeMap[mEntityMasks[entity]].erase(entity);
    mEntityMasks[entity] &= ~(1 << ComponentManager::getId<T>());
    mArchetypeMap[mEntityMasks[entity]].insert(entity);
    return true;
  }

  template <typename T> static inline bool hasComponent(entityT entity) {
    return mEntityMasks[entity] & (1 << ComponentManager::getId<T>());
  }

  template <typename T> static inline T &getComponent(entityT entity) {
    if (!hasComponent<T>(entity))
      throw std::invalid_argument("Entity " + std::to_string(entity) +
                                  " does not have component " +
                                  typeid(T).name());
    return ComponentManager::get<T>(entity);
  }

  static entityT createEntity() {
    const entityT e = newEntity();
    mEntityMasks[e] = 0;
    mArchetypeMap[0].insert(e);
    return e;
  };

  static void deleteEntity(entityT entity) {
    mEntitiesToDelete.push_back(entity);
  };

  entityT addEntity(entityT entity = World::createEntity()) {
    mLocalEntities.insert(entity);
    return entity;
  };

  void removeEntity(entityT entity) { mLocalEntities.erase(entity); };

  template <typename... SubQueries> inline std::vector<entityT> query() {
    Query<SubQueries...> q;
    std::set<entityT> temp;
    for (auto &archetype : mArchetypeMap) {
      if (archetype.second.size() > 0 &&
          (archetype.first & q.andMask) == q.andMask &&
          (archetype.first & q.notMask) == 0) {
        temp.insert(archetype.second.begin(), archetype.second.end());
      }
    }
    std::vector<entityT> res;
    std::set_intersection(temp.begin(), temp.end(), mLocalEntities.begin(),
                          mLocalEntities.end(), std::back_inserter(res));
    return res;
  };

  template <typename... Fns>
    requires(std::is_convertible_v<Fns, void (*)(World &)>, ...)
  inline void update(Fns... fns) {
    (fns(*this), ...);
    while (mEntitiesToDelete.size()) {
      entityT entity = mEntitiesToDelete.back();
      mEntitiesToDelete.pop_back();
      for (auto &world : mWorlds) {
        world->removeEntity(entity);
      }
      ComponentManager::remove(entity);
      int mask = mEntityMasks.at(entity);
      mArchetypeMap[mask].erase(entity);
      mEntityMasks.erase(entity);
    }
  }

  entityT copy(entityT entity) {
    const int mask = mEntityMasks[entity];
    const entityT copy = ComponentManager::copy(entity);
    mEntityMasks[copy] = mask;
    mArchetypeMap[mask].insert(copy);
    return copy;
  }

  size_t entityCount() { return mLocalEntities.size(); };

  World();
  ~World();

private:
  static std::vector<World *> mWorlds;
  static std::unordered_map<entityT, int> mEntityMasks;
  static std::unordered_map<int, std::set<entityT>> mArchetypeMap;
  static std::vector<entityT> mEntitiesToDelete;

  std::set<entityT> mLocalEntities;
};
