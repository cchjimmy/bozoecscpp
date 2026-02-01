#pragma once
#include "entity.hpp"
#include "pool.hpp"
#include <memory>
#include <typeindex>
#include <unordered_map>

class ComponentManager {
public:
  ComponentManager() = default;
  ~ComponentManager() = default;

  template <typename T> inline void reg() {
    if (mIdMap.contains(typeid(T)))
      return;
    mIdMap[typeid(T)] = mIdMap.size();
    mPoolMap[typeid(T)] = std::make_shared<ObjectPoolMap<entityT, T>>();
  }

  template <typename T> inline T &add(const entityT &entity) {
    return getOPM<T>()->add(entity);
  }

  template <typename T> inline void remove(const entityT &entity) {
    getOPM<T>()->remove(entity);
  }

  template <typename T> inline bool has(const entityT &entity) {
    return mPoolMap.contains(typeid(T)) && getOPM<T>()->has(entity);
  }

  template <typename T> inline T &get(const entityT &entity) {
    return getOPM<T>()->get(entity);
  }

  template <typename T> inline size_t size() {
    return mPoolMap[typeid(T)]->size();
  }

  template <typename T> inline int getId() {
    return mIdMap.contains(typeid(T)) ? mIdMap[typeid(T)] : -1;
  }

  void copy(const entityT &src, const entityT &dest) {
    for (auto &pool : mPoolMap) {
      auto p =
          std::static_pointer_cast<ObjectPoolMap<entityT, void *>>(pool.second);
      if (!p->has(src))
        continue;
      p->add(dest) = p->get(src);
    }
  }

  void erase(const entityT &entity) {
    for (auto &pool : mPoolMap)
      std::static_pointer_cast<ObjectPoolMap<entityT, void *>>(pool.second)
          ->remove(entity);
  }

  void clean() {
    for (auto &pool : mPoolMap)
      pool.second->clean();
  }

private:
  std::unordered_map<std::type_index, std::shared_ptr<IObjectPoolMap>> mPoolMap;
  std::unordered_map<std::type_index, int> mIdMap;

  template <typename T> std::shared_ptr<ObjectPoolMap<entityT, T>> getOPM() {
    return std::static_pointer_cast<ObjectPoolMap<entityT, T>>(
        mPoolMap.at(typeid(T)));
  }
};
