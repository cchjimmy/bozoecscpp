#pragma once
#include <unordered_map>
#include <vector>

template <typename K, typename V> class ObjectPoolMap {
public:
  ObjectPoolMap() = default;
  ~ObjectPoolMap();

  V &add(K key) {
    if (mIndices.contains(key))
      return mStorage.at(mIndices.at(key));
    if (mSize >= mStorage.size())
      mStorage.emplace_back();
    mIndices[key] = mSize;
    mSize++;
    return mStorage.at(mSize - 1);
  }

  bool remove(K key) {
    if (!mIndices.contains(key))
      return false;
    int index = mIndices[key];
    mIndices.erase(key);
    V &removed = mStorage[index];
    mStorage[index] = mStorage[mSize - 1];
    mStorage[mSize - 1] = removed;
    for (auto &entry : mIndices) {
      if (entry.second != mSize - 1)
        continue;
      mIndices[entry.first] = index;
      break;
    }
    mSize--;
    return true;
  }

  V &get(K key) {
    if (!mIndices.contains(key))
      throw std::logic_error("Key not found.");
    int index = mIndices.at(key);
    if (index >= mSize) {
      throw std::out_of_range("Index out of range.");
    }
    return mStorage.at(index);
  }

  void clear() { mStorage.clear(); }

  size_t size() { return mSize; }

private:
  std::unordered_map<K, int> mIndices;
  std::vector<V> mStorage;
  size_t mSize = 0;
};
