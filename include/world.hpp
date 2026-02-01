#pragma once
#include "component.hpp"
#include "entity.hpp"
#include "query.hpp"
#include <unordered_map>
#include <unordered_set>
#include <vector>

class World {
public:
  World() = default;
  ~World() = default;

  template <typename T> inline World &registerComponent() {
    mComponentManager.reg<T>();
    return *this;
  }

  template <typename T>
  inline T &addComponent(const entityT &entity, const T &values = {}) {
    mComponentManager.reg<T>();
    auto &cem = mCompEntityMap[mComponentManager.getId<T>()];
    if (cem.contains(entity)) {
      return mComponentManager.get<T>(entity) = values;
    }
    cem.emplace(entity);
    return mComponentManager.add<T>(entity) = values;
  }

  template <typename T> inline void removeComponent(const entityT &entity) {
    mComponentManager.reg<T>();
    auto &cem = mCompEntityMap[mComponentManager.getId<T>()];
    if (!cem.contains(entity))
      return;
    cem.erase(entity);
    mComponentManager.remove<T>(entity);
  }

  template <typename T> inline bool hasComponent(const entityT &entity) {
    return mComponentManager.has<T>(entity);
  }

  template <typename T> inline T &getComponent(const entityT &entity) {
    return mComponentManager.get<T>(entity);
  }

  void deleteEntity(const entityT &entity) {
    mEntitySet.erase(entity);
    mComponentManager.erase(entity);
    for (auto &entry : mCompEntityMap) {
      entry.second.erase(entity);
    }
  };

  entityT addEntity(const entityT &entity = newEntity()) {
    mEntitySet.emplace(entity);
    return entity;
  };

  template <typename... SubQueries> inline std::vector<entityT> query() {
    const Query<SubQueries...> q(mComponentManager);

    // set intersection
    std::unordered_set<entityT> temp = mEntitySet;
    size_t s = temp.size();
    for (const auto &id : q.andIds) {
      const auto &cem = mCompEntityMap[id];
      if (cem.size() > s)
        continue;
      temp = cem;
      s = temp.size();
    }
    for (const auto &id : q.andIds) {
      const auto &cem = mCompEntityMap[id];
      for (const auto &e : temp) {
        if (cem.contains(e))
          continue;
        temp.erase(e);
      }
    }

    // set difference
    for (const auto &id : q.notIds) {
      const auto &cem = mCompEntityMap[id];
      for (const auto &e : cem) {
        temp.erase(e);
      }
    }
    return std::vector<entityT>(temp.begin(), temp.end());
  };

  template <typename... Fns>
    requires(std::is_convertible_v<Fns, void (*)(World &)>, ...)
  inline void update(Fns... fns) {
    (fns(*this), ...);
  }

  entityT copy(const entityT &src, const entityT &dest = newEntity()) {
    mComponentManager.copy(src, dest);
    for (auto &entry : mCompEntityMap) {
      if (!entry.second.contains(src))
        continue;
      entry.second.emplace(dest);
    }
    mEntitySet.emplace(dest);
    return dest;
  }

  size_t entityCount() { return mEntitySet.size(); };

  void cleanObjectPools() { mComponentManager.clean(); }

private:
  std::unordered_map<int, std::unordered_set<entityT>> mCompEntityMap;
  std::unordered_set<entityT> mEntitySet;
  ComponentManager mComponentManager;
};
