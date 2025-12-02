#pragma once
#include "component.hpp"
#include "entity.hpp"
#include "query.hpp"
#include <set>
#include <unordered_map>
#include <vector>

class World {
public:
  World() = default;
  ~World() = default;

  template <typename T> inline World &registerComponent() {
    if (!mComponentManager.isRegistered<T>())
      mComponentManager.reg<T>();
    return *this;
  }

  template <typename T>
  inline T &addComponent(const entityT &entity, const T &values = {}) {
    registerComponent<T>();
    T &comp = mComponentManager.add<T>(entity);
    int mask = mEntityMasks[entity];
    const int compId = mComponentManager.getId<T>();
    comp = values;
    if (mask & (1 << compId))
      return comp;
    mArchetypeMap[mask].erase(entity);
    mask = mEntityMasks[entity] |= 1 << compId;
    mArchetypeMap[mask].insert(entity);
    return comp;
  }

  template <typename T> inline void removeComponent(const entityT &entity) {
    int mask = mEntityMasks[entity];
    const int compId = mComponentManager.getId<T>();
    if ((mask & (1 << compId)) == 0)
      return;
    mComponentManager.remove<T>(entity);
    mArchetypeMap[mask].erase(entity);
    mask = mEntityMasks[entity] ^= 1 << compId;
    mArchetypeMap[mask].insert(entity);
  }

  template <typename T> inline bool hasComponent(const entityT &entity) {
    return mComponentManager.has<T>(entity);
  }

  template <typename T> inline T &getComponent(const entityT &entity) {
    return mComponentManager.get<T>(entity);
  }

  void deleteEntity(const entityT &entity) {
    mComponentManager.erase(entity);
    int mask = mEntityMasks.at(entity);
    mArchetypeMap[mask].erase(entity);
    mEntityMasks.erase(entity);
  };

  entityT addEntity(const entityT &entity = newEntity()) {
    if (mEntityMasks.contains(entity))
      return entity;
    mEntityMasks[entity] = 0;
    mArchetypeMap[0].insert(entity);
    return entity;
  };

  template <typename... SubQueries> inline std::vector<entityT> query() {
    Query<SubQueries...> q(mComponentManager);
    std::vector<entityT> res;
    for (auto &archetype : mArchetypeMap) {
      if (archetype.second.size() > 0 &&
          (archetype.first & q.andMask) == q.andMask &&
          (archetype.first & q.notMask) == 0)
        res.insert(res.end(), archetype.second.begin(), archetype.second.end());
    }
    return res;
  };

  template <typename... Fns>
    requires(std::is_convertible_v<Fns, void (*)(World &)>, ...)
  inline void update(Fns... fns) {
    (fns(*this), ...);
  }

  entityT copy(const entityT &src, const entityT &dest = newEntity()) {
    mComponentManager.copy(src, dest);
    int mask = mEntityMasks[src];
    mEntityMasks[dest] = mask;
    mArchetypeMap[mask].insert(dest);
    return dest;
  }

  size_t entityCount() { return mEntityMasks.size(); };

  void cleanObjectPools() { mComponentManager.clean(); }

private:
  std::unordered_map<entityT, int> mEntityMasks;
  std::unordered_map<int, std::set<entityT>> mArchetypeMap;
  ComponentManager mComponentManager;
};
