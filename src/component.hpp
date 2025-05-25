#pragma once
#include "pool.hpp"
#include <typeindex>
#include <unordered_map>

class ComponentManager {
public:
  ComponentManager() = default;
  ~ComponentManager() {
    for (auto &pool : mPools) {
      delete static_cast<ObjectPool<void *> *>(pool);
    }
    mPools.clear();
  }

  template <typename T> inline static void reg() {
    mIdMap[std::type_index(typeid(T))] = mPools.size();
    mPools.insert(mPools.begin() + mIdMap[std::type_index(typeid(T))],
                  static_cast<void *>(new ObjectPool<T>));
  }

  template <typename T> inline static T &create() {
    throwNotRegistered<T>();
    return (static_cast<ObjectPool<T> *>(
                mPools.at(mIdMap[std::type_index(typeid(T))]))
                ->addObj());
  }

  template <typename T> inline static T &remove(int index) {
    throwNotRegistered<T>();
    return (static_cast<ObjectPool<T> *>(
                mPools.at(mIdMap[std::type_index(typeid(T))]))
                ->removeObj(index));
  }

  template <typename T> inline static T &get(int index) {
    throwNotRegistered<T>();
    return static_cast<ObjectPool<T> *>(
               mPools.at(mIdMap[std::type_index(typeid(T))]))
        ->at(index);
  }

  template <typename T> inline static void dereg() {
    mPools.erase(mPools.begin() + mIdMap[std::type_index(typeid(T))]);
  }

  template <typename T> inline static bool isRegistered() {
    return mIdMap.find(std::type_index(typeid(T))) != mIdMap.end();
  }

  template <typename T> inline static void throwNotRegistered() {
    if (!isRegistered<T>())
      throw std::invalid_argument(std::string("Component ") +
                                  std::type_index(typeid(T)).name() +
                                  " is not registered.");
  }

  template <typename T> inline static ObjectPool<T> &at() {
    return *static_cast<ObjectPool<T> *>(
        mPools.at(mIdMap[std::type_index(typeid(T))]));
  }

  template <typename T> inline static size_t size() {
    return static_cast<ObjectPool<T> *>(
               mPools.at(mIdMap[std::type_index(typeid(T))]))
        ->size();
  }

  static int typesLen();

  template <typename T> inline static int getId() {
    return mIdMap[std::type_index(typeid(T))];
  }

private:
  static std::vector<void *> mPools;
  static std::unordered_map<std::type_index, int> mIdMap;
};
