#include "component.hpp"

std::unordered_map<std::type_index, void *> ComponentManager::mPoolMap;
std::unordered_map<std::type_index, int> ComponentManager::mIdMap;
