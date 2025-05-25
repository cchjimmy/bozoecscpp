#pragma once
#include "pool.hpp"

typedef int entityT;

class EntityManager {
public:
  EntityManager() = default;
  ~EntityManager();
  static entityT create();
  static entityT remove(int index);
  static int size();
  static entityT get(int index);
  static int find(entityT entity);

private:
  static entityT mNextId;
  static ObjectPool<entityT> mPool;
};
