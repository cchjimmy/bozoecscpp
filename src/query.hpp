#pragma once
#include "component.hpp"
#include <type_traits>

template <typename... Components> class ISubQuery {
public:
  static int getMask() {
    static int mask = (0 | ... | (1 << ComponentManager::getId<Components>()));
    return mask;
  }
  virtual ~ISubQuery() = default;
};

template <typename... Components> class And : public ISubQuery<Components...> {
public:
  And() : ISubQuery<Components...>() {};
};

template <typename... Components> class Or : public ISubQuery<Components...> {
public:
  Or() : ISubQuery<Components...>() {};
};

template <typename... Components> class Not : public ISubQuery<Components...> {
public:
  Not() : ISubQuery<Components...>() {};
};

template <typename T, template <typename...> class Template>
struct is_instance_of : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct is_instance_of<Template<Args...>, Template> : std::true_type {};

template <typename T, template <typename...> class Template>
inline constexpr bool is_instance_of_v = is_instance_of<T, Template>::value;

template <typename... SubQueries> class Query {
public:
  int andMask = 0, orMask = 0, notMask = 0;

  Query() { (processSubQuery<SubQueries>(), ...); }

private:
  template <typename SubQuery> inline void processSubQuery() {
    if constexpr (is_instance_of_v<SubQuery, And>) {
      andMask |= SubQuery::getMask();
    } else if constexpr (is_instance_of_v<SubQuery, Or>) {
      orMask |= SubQuery::getMask();
    } else if constexpr (is_instance_of_v<SubQuery, Not>) {
      notMask |= SubQuery::getMask();
    }
  }
};
