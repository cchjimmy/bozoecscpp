#pragma once
#include "component.hpp"
#include "entity.hpp"
#include "query.hpp"
#include <iterator>
#include <optional>
#include <set>
#include <unordered_map>
#include <vector>

class World {
public:
  template <typename T> static inline void registerComponent() {
    if (ComponentManager::isRegistered<T>())
      return;
    ComponentManager::reg<T>();
    mRemoveMap[typeid(T)] = [](entityT e) { ComponentManager::remove<T>(e); };
  }

  template <typename T> static inline T &addComponent(entityT entity) {
    registerComponent<T>();
    if (hasComponent<T>(entity))
      throw std::invalid_argument("Entity " + std::to_string(entity) +
                                  " already has component " + typeid(T).name());

    T &comp = ComponentManager::add<T>(entity);

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

  static entityT createEntity();
  static void deleteEntity(entityT entity);

  entityT addEntity(entityT entity = World::createEntity());
  void removeEntity(entityT entity);

  template <typename... SubQueries> inline std::vector<entityT> query() {
    Query<SubQueries...> q;
    std::vector<entityT> res;
    for (auto &archetype : mArchetypeMap) {
      if (archetype.second.size() > 0 &&
          (archetype.first & q.andMask) == q.andMask &&
          (archetype.first & q.notMask) == 0) {
        res.insert(res.end(), archetype.second.begin(), archetype.second.end());
      }
    }
    std::set_intersection(res.begin(), res.end(), mLocalEntities.begin(),
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

      const auto &types = ComponentManager::types();
      for (int i = 0, l = types.size(), m = mEntityMasks[entity]; i < l; i++) {
        if (!(m & 1 << i))
          continue;
        mRemoveMap[types[i]](entity);
      }

      int mask = mEntityMasks.at(entity);
      mArchetypeMap[mask].erase(entity);
      mEntityMasks.erase(entity);
    }
  }

  size_t entityCount();

  World();
  ~World();

private:
  static std::vector<World *> mWorlds;
  static std::unordered_map<entityT, int> mEntityMasks;
  static std::unordered_map<int, std::set<entityT>> mArchetypeMap;
  static std::unordered_map<std::type_index, void (*)(entityT)> mRemoveMap;
  static std::vector<entityT> mEntitiesToDelete;

  std::set<entityT> mLocalEntities;
};
