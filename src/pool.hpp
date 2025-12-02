#pragma once
#include <unordered_map>
#include <vector>

template <typename K, typename V> class ObjectPoolMap {
public:
  ObjectPoolMap() = default;
  ~ObjectPoolMap() = default;

  V &add(const K &key) {
    if (mKeyToIndex.contains(key))
      return mStorage[mKeyToIndex[key]];
    if (mStorage.size() <= mKeyToIndex.size())
      mStorage.emplace_back();
    mKeyToIndex[key] = mKeyToIndex.size();
    mindexToKey.emplace_back(key);
    return mStorage[mKeyToIndex[key]];
  }

  void remove(const K &key) {
    if (!mKeyToIndex.contains(key))
      return;
    mStorage[mKeyToIndex[key]] = mStorage[mKeyToIndex[mindexToKey.back()]];
    mindexToKey[mKeyToIndex[key]] = mindexToKey.back();
    mindexToKey.pop_back();
    mKeyToIndex.erase(key);
  }

  V &get(const K &key) { return mStorage[mKeyToIndex.at(key)]; }

  bool has(const K &key) { return mKeyToIndex.contains(key); }

  void clear() {
    mStorage.clear();
    mKeyToIndex.clear();
    mindexToKey.clear();
  }

  void clean() { mStorage.resize(mKeyToIndex.size()); }

  size_t size() { return mKeyToIndex.size(); }

private:
  std::unordered_map<K, int> mKeyToIndex;
  std::vector<K> mindexToKey;
  std::vector<V> mStorage;
};
