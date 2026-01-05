#pragma once
#include <unordered_map>
#include <vector>

class IObjectPoolMap {
public:
  virtual void clear() = 0;
  virtual void clean() = 0;
  virtual size_t size() = 0;
  virtual ~IObjectPoolMap() {};
};

template <typename K, typename V> class ObjectPoolMap : public IObjectPoolMap {
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
    const int index = mKeyToIndex[key];
    const K backKey = mindexToKey.back();
    mStorage[index] = mStorage[mKeyToIndex[backKey]];
    mindexToKey[index] = backKey;
    mindexToKey.pop_back();
    mKeyToIndex[backKey] = index;
    mKeyToIndex.erase(key);
  }

  V &get(const K &key) { return mStorage[mKeyToIndex.at(key)]; }

  bool has(const K &key) { return mKeyToIndex.contains(key); }

  void clear() override {
    mStorage.clear();
    mKeyToIndex.clear();
    mindexToKey.clear();
  }

  void clean() override { mStorage.resize(mKeyToIndex.size()); }

  size_t size() override { return mKeyToIndex.size(); }

private:
  std::unordered_map<K, int> mKeyToIndex;
  std::vector<K> mindexToKey;
  std::vector<V> mStorage;
};
