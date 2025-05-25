#include "entity.hpp"

entityT EntityManager::mNextId;
ObjectPool<entityT> EntityManager::mPool;

EntityManager::~EntityManager() { mPool.clear(); }

entityT EntityManager::create() {
  entityT &e = mPool.addObj();
  e = mPool.size() - 1;
  return e;
}

entityT EntityManager::remove(int index) { return mPool.removeObj(index); }

int EntityManager::size() { return mPool.size(); }

entityT EntityManager::get(int index) { return mPool.at(index); }

int EntityManager::find(entityT entity) { return mPool.find(entity); }
