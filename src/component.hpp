#pragma once
#include "entity.hpp"
#include "pool.hpp"
#include <typeindex>
#include <unordered_map>
#include <vector>

class ComponentManager {
public:
  ComponentManager() = default;
  ~ComponentManager() {
    for (auto &pool : mPoolMap) {
      static_cast<ObjectPoolMap<entityT, void *> *>(pool.second)->clear();
      delete static_cast<ObjectPoolMap<entityT, void *> *>(pool.second);
    }
    mPoolMap.clear();
  }

  template <typename T> inline static void reg() {
    mIdMap[typeid(T)] = mIdMap.size();
    mPoolMap[typeid(T)] = static_cast<void *>(new ObjectPoolMap<entityT, T>);
  }

  template <typename T> inline static T &add(entityT entity) {
    return (static_cast<ObjectPoolMap<entityT, T> *>(mPoolMap.at(typeid(T)))
                ->add(entity));
  }

  template <typename T> inline static bool remove(entityT entity) {
    return (static_cast<ObjectPoolMap<entityT, T> *>(mPoolMap.at(typeid(T)))
                ->remove(entity));
  }

  template <typename T> inline static T &get(entityT entity) {
    return static_cast<ObjectPoolMap<entityT, T> *>(mPoolMap.at(typeid(T)))
        ->get(entity);
  }

  template <typename T> inline static void dereg() {
    mPoolMap.erase(typeid(T));
  }

  template <typename T> inline static bool isRegistered() {
    return mIdMap.find(typeid(T)) != mIdMap.end();
  }

  template <typename T> inline static size_t size() {
    return static_cast<ObjectPoolMap<entityT, T> *>(mPoolMap.at(typeid(T)))
        ->size();
  }

  static std::vector<std::type_index> types() {
    std::vector<std::type_index> res;
    for (auto &kv : mIdMap) {
      res.push_back(kv.first);
    }
    return res;
  }

  template <typename T> inline static int getId() {
    return mIdMap.contains(typeid(T)) ? mIdMap[typeid(T)] : -1;
  }

private:
  static std::unordered_map<std::type_index, void *> mPoolMap;
  static std::unordered_map<std::type_index, int> mIdMap;
};
