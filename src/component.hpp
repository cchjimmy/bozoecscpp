#pragma once
#include "entity.hpp"
#include "pool.hpp"
#include <typeindex>
#include <unordered_map>

class ComponentManager {
public:
  ComponentManager() = default;
  ~ComponentManager() = default;

  template <typename T> inline void reg() {
    mIdMap[typeid(T)] = mIdMap.size();
    mPoolMap[typeid(T)] = reinterpret_cast<ObjectPoolMap<entityT, void *> *>(
        new ObjectPoolMap<entityT, T>());
  }

  template <typename T> inline T &add(const entityT &entity) {
    return reinterpret_cast<ObjectPoolMap<entityT, T> *>(mPoolMap[typeid(T)])
        ->add(entity);
  }

  template <typename T> inline void remove(const entityT &entity) {
    mPoolMap[typeid(T)]->remove(entity);
  }

  template <typename T> inline bool has(const entityT &entity) {
    return mPoolMap.contains(typeid(T)) && mPoolMap[typeid(T)]->has(entity);
  }

  template <typename T> inline T &get(const entityT &entity) {
    return reinterpret_cast<ObjectPoolMap<entityT, T> *>(mPoolMap[typeid(T)])
        ->get(entity);
  }

  template <typename T> inline void dereg() { mPoolMap.erase(typeid(T)); }

  template <typename T> inline bool isRegistered() {
    return mIdMap.find(typeid(T)) != mIdMap.end();
  }

  template <typename T> inline size_t size() {
    return mPoolMap[typeid(T)]->size();
  }

  template <typename T> inline int getId() {
    return mIdMap.contains(typeid(T)) ? mIdMap[typeid(T)] : -1;
  }

  void copy(const entityT &src, const entityT &dest) {
    for (auto &pool : mPoolMap) {
      if (!pool.second->has(src))
        continue;
      pool.second->add(dest) = pool.second->get(src);
    }
  }

  void erase(const entityT &entity) {
    for (auto &pool : mPoolMap)
      pool.second->remove(entity);
  }

  void clean() {
    for (auto &pool : mPoolMap)
      pool.second->clean();
  }

private:
  std::unordered_map<std::type_index, ObjectPoolMap<entityT, void *> *>
      mPoolMap;
  std::unordered_map<std::type_index, int> mIdMap;
};
