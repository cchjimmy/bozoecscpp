#pragma once
#include "component.hpp"
#include "entity.hpp"
#include "query.hpp"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <unordered_map>
#include <vector>

class World {
public:
  template <typename T> static inline void registerComponent() {
    ComponentManager::reg<T>();
    mRemoveMap[ComponentManager::getId<T>()] = [](entityT e) {
      ComponentManager::remove<T>(e);
    };
  }

  template <typename T> static inline T &addComponent(entityT entity) {
    if (!ComponentManager::isRegistered<T>())
      registerComponent<T>();
    if (hasComponent<T>(entity))
      throw std::invalid_argument("Entity " + std::to_string(entity) +
                                  " already has component " + typeid(T).name());

    mIndexMap[typeid(T)][entity] = ComponentManager::size<T>();

    T &comp = ComponentManager::create<T>();

    mArchetypeMap[mEntityMasks[entity]].erase(entity);
    mEntityMasks[entity] |= 1 << ComponentManager::getId<T>();
    mArchetypeMap[mEntityMasks[entity]].insert(entity);
    return comp;
  }

  template <typename T> static inline T &removeComponent(entityT entity) {
    if (!hasComponent<T>(entity))
      throw std::invalid_argument("Entity " + std::to_string(entity) +
                                  " does not have component " +
                                  typeid(T).name());

    T &removed = ComponentManager::remove<T>(mIndexMap[typeid(T)][entity]);
    entityT backEntity;

    for (auto &kv : mIndexMap[typeid(T)]) {
      if (kv.second != mIndexMap[typeid(T)][entity])
        continue;
      backEntity = kv.first;
      break;
    }

    mIndexMap[typeid(T)][backEntity] = mIndexMap[typeid(T)][entity];

    mArchetypeMap[mEntityMasks[entity]].erase(entity);
    mEntityMasks[entity] &= ~(1 << ComponentManager::getId<T>());
    mArchetypeMap[mEntityMasks[entity]].insert(entity);
    return removed;
  }

  template <typename T> static inline bool hasComponent(entityT entity) {
    return mEntityMasks[entity] & (1 << ComponentManager::getId<T>());
  }

  template <typename T> static inline T &getComponent(entityT entity) {
    if (!hasComponent<T>(entity))
      throw std::invalid_argument("Entity " + std::to_string(entity) +
                                  " does not have component " +
                                  typeid(T).name());

    return ComponentManager::get<T>(mIndexMap[typeid(T)][entity]);
  }

  static entityT createEntity();
  static entityT deleteEntity(entityT entity);

  entityT addEntity(std::optional<entityT> entity = std::nullopt);
  entityT removeEntity(entityT entity);
  bool entityExists(entityT entity);

  template <typename... SubQueries> inline std::vector<entityT> query() {
    Query<SubQueries...> q;
    std::vector<entityT> res;
    for (auto &archetype : mArchetypeMap) {
      if (archetype.second.size() > 0 &&
          (archetype.first & q.andMask) == q.andMask &&
          (archetype.first | q.orMask) > 0 &&
          (archetype.first & q.notMask) == 0) {
        std::set_intersection(archetype.second.begin(), archetype.second.end(),
                              mLocalEntities.begin(), mLocalEntities.end(),
                              std::back_inserter(res));
      }
    }
    return res;
  };

  template <typename... Fns>
    requires(std::is_convertible_v<Fns, void (*)(World &)>, ...)
  inline void update(Fns... fns) {
    auto start = std::chrono::system_clock::now();
    (fns(*this), ...);
    dtMicro = static_cast<time_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now() - start)
            .count());
    timeMicro += dtMicro;
  }

  size_t entityCount();

  World();
  ~World();

  time_t timeMicro, dtMicro;

private:
  static std::vector<World *> mWorlds;
  static std::unordered_map<std::type_index, std::unordered_map<entityT, int>>
      mIndexMap;
  static std::unordered_map<entityT, int> mEntityMasks;
  static std::unordered_map<int, std::set<entityT>> mArchetypeMap;
  static std::unordered_map<int, void (*)(entityT)> mRemoveMap;

  std::set<entityT> mLocalEntities;
};
