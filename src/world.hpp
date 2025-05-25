#pragma once
#include "component.hpp"
#include "entity.hpp"
#include "query.hpp"
#include <algorithm>
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
  }

  template <typename T> static inline T &addComponent(entityT entity) {
    if (!ComponentManager::isRegistered<T>())
      registerComponent<T>();
    if (hasComponent<T>(entity))
      throw std::invalid_argument("Entity " + std::to_string(entity) +
                                  " already has component " + typeid(T).name());

    std::vector<int> &indices = mIndexMap[ComponentManager::getId<T>()];
    if (entity >= indices.size()) {
      indices.resize(entity + 1);
    }
    indices[entity] = ComponentManager::size<T>();

    T &comp = ComponentManager::create<T>();

    std::vector<int> &owners = mOwnerMap[ComponentManager::getId<T>()];
    if (indices[entity] >= owners.size()) {
      owners.resize(indices[entity] + 1);
    }
    owners[indices[entity]] = entity;

    mArchetypeMap.at(mEntityMasks[entity]).erase(entity);
    mEntityMasks[entity] |= 1 << ComponentManager::getId<T>();
    try {
      mArchetypeMap.at(mEntityMasks[entity]);
    } catch (std::out_of_range &e) {
      mArchetypeMap.insert({mEntityMasks[entity], std::set<entityT>()});
    }
    mArchetypeMap.at(mEntityMasks[entity]).insert(entity);
    return comp;
  }

  template <typename T> static inline T &removeComponent(entityT entity) {
    if (!hasComponent<T>(entity))
      throw std::invalid_argument("Entity " + std::to_string(entity) +
                                  " does not have component " +
                                  typeid(T).name());
    std::vector<int> indices = mIndexMap[ComponentManager::getId<T>()];
    T &removed = ComponentManager::remove<T>(indices[entity]);
    std::vector<int> owners = mOwnerMap[ComponentManager::getId<T>()];
    entityT backEntity = owners[indices.back()];

    indices[backEntity] = indices[entity];
    owners[indices[backEntity]] = backEntity;

    mArchetypeMap.at(mEntityMasks[entity]).erase(entity);
    mEntityMasks[entity] &= ~(1 << ComponentManager::getId<T>());
    mArchetypeMap.at(mEntityMasks[entity]).insert(entity);
    return removed;
  }

  template <typename T> static inline bool hasComponent(entityT entity) {
    return mEntityMasks.at(entity) & (1 << ComponentManager::getId<T>());
  }

  template <typename T> inline T &getComponent(entityT entity) {
    if (!hasComponent<T>(entity))
      throw std::invalid_argument("Entity " + std::to_string(entity) +
                                  " does not have component " +
                                  typeid(T).name());
    if (!entityExists(entity))
      throw std::invalid_argument("Entity " + std::to_string(entity) +
                                  " does not exist in local World instance.");
    return ComponentManager::get<T>(
        mIndexMap[ComponentManager::getId<T>()][entity]);
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
      if ((archetype.first & q.andMask) == q.andMask and
          (archetype.first | q.orMask) > 0 and
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
    std::chrono::time_point now = std::chrono::system_clock::now();
    (fns(*this), ...);
    dtMicro = static_cast<time_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now() - now)
            .count());
    timeMicro += dtMicro;
  }

  size_t entityCount();

  World();
  ~World();

  time_t timeMicro, dtMicro;

private:
  static std::vector<World *> mWorlds;
  static std::unordered_map<int, std::vector<int>> mIndexMap;
  static std::vector<int> mEntityMasks;
  static std::unordered_map<int, std::set<entityT>> mArchetypeMap;
  static std::unordered_map<int, void (*)(entityT)> mRemoveMap;
  static std::unordered_map<int, std::vector<entityT>> mOwnerMap;

  std::set<entityT> mLocalEntities;
};
