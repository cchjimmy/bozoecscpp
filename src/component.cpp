#include "component.hpp"

std::vector<void *> ComponentManager::mPools;
std::unordered_map<std::type_index, int> ComponentManager::mIdMap;

int ComponentManager::typesLen() { return mPools.size(); }
