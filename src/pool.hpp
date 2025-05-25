#pragma once
#include <algorithm>
#include <stdexcept>
#include <vector>

template <typename T> class ObjectPool {
public:
  ObjectPool() = default;
  ~ObjectPool() { mStorage.clear(); }

  T &addObj() {
    try {
      mStorage.at(mSize);
    } catch (std::out_of_range &e) {
      mStorage.emplace_back();
    }
    mSize++;
    return mStorage.at(mSize - 1);
  }

  T &removeObj(int index) {
    if (index > mSize) {
      throw std::invalid_argument("Index out of range.");
    }
    T &removed = mStorage.at(index);
    mStorage[index] = mStorage.back();
    mStorage[mStorage.size() - 1] = removed;
    mSize--;
    return removed;
  }

  int find(T &obj) {
    return std::find(mStorage.begin(), mStorage.end(), obj) - mStorage.begin();
  }

  T &at(int index) {
    if (index > mSize) {
      throw std::invalid_argument("Index out of range.");
    }
    return mStorage.at(index);
  }

  T &operator[](int index) { return at(index); }

  void clear() { mStorage.clear(); }

  size_t size() { return mSize; }

private:
  std::vector<T> mStorage;
  size_t mSize = 0;
};
