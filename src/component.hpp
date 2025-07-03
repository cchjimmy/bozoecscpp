#pragma once
#include "pool.hpp"
#include <typeindex>
#include <unordered_map>
#include <vector>

class ComponentManager {
public:
  ComponentManager() = default;
  ~ComponentManager() {
    for (auto &pool : mPoolMap) {
      static_cast<ObjectPool<void *> *>(pool.second)->clear();
      delete static_cast<ObjectPool<void *> *>(pool.second);
    }
    mPoolMap.clear();
  }

  template <typename T> inline static void reg() {
    mIdMap[typeid(T)] = mIdMap.size();
    mPoolMap[typeid(T)] = static_cast<void *>(new ObjectPool<T>);
  }

  template <typename T> inline static T &create() {
    throwNotRegistered<T>();
    return (static_cast<ObjectPool<T> *>(mPoolMap.at(typeid(T)))->addObj());
  }

  template <typename T> inline static T &remove(int index) {
    throwNotRegistered<T>();
    return (
        static_cast<ObjectPool<T> *>(mPoolMap.at(typeid(T)))->removeObj(index));
  }

  template <typename T> inline static T &get(int index) {
    throwNotRegistered<T>();
    return static_cast<ObjectPool<T> *>(mPoolMap.at(typeid(T)))->at(index);
  }

  template <typename T> inline static void dereg() {
    mPoolMap.erase(typeid(T));
  }

  template <typename T> inline static bool isRegistered() {
    return mIdMap.find(typeid(T)) != mIdMap.end();
  }

  template <typename T> inline static void throwNotRegistered() {
    if (!isRegistered<T>())
      throw std::invalid_argument(std::string("Component ") + typeid(T).name() +
                                  " is not registered.");
  }

  template <typename T> inline static ObjectPool<T> &at() {
    return *static_cast<ObjectPool<T> *>(mPoolMap.at(typeid(T)));
  }

  template <typename T> inline static size_t size() {
    return static_cast<ObjectPool<T> *>(mPoolMap.at(typeid(T)))->size();
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
