#pragma once
#include "component.hpp"
#include <type_traits>
#include <vector>

template <typename... Components> class ISubQuery {
public:
  static std::vector<int> getIds(ComponentManager &cm) {
    std::vector<int> ids;
    (ids.emplace_back(cm.getId<Components>()), ...);
    return ids;
  }
  virtual ~ISubQuery() = default;
};

template <typename... Components> class And : public ISubQuery<Components...> {
public:
  And() : ISubQuery<Components...>() {};
};

template <typename... Components> class Not : public ISubQuery<Components...> {
public:
  Not() : ISubQuery<Components...>() {};
};

// credit:
// https://stackoverflow.com/questions/62672942/how-can-i-check-if-a-template-specialization-is-a-child-class-of-a-base-template#62673159
template <typename Derived, template <typename...> class Base>
struct is_instance_of : std::false_type {};

template <template <typename...> class Derived,
          template <typename...> class Base, typename... Args>
struct is_instance_of<Derived<Args...>, Base>
    : std::is_convertible<Derived<Args...> *, Base<Args...> *> {};

template <typename T, template <typename...> class Template>
inline constexpr bool is_instance_of_v = is_instance_of<T, Template>::value;

template <typename... SubQueries>
  requires(is_instance_of_v<SubQueries, ISubQuery>, ...)
class Query {
public:
  std::vector<int> andIds, notIds;

  Query(ComponentManager &cm) { (processSubQuery<SubQueries>(cm), ...); }

private:
  template <typename SubQuery>
    requires(is_instance_of_v<SubQuery, ISubQuery>)
  inline void processSubQuery(ComponentManager &cm) {
    std::vector<int> ids = SubQuery::getIds(cm);
    if constexpr (is_instance_of_v<SubQuery, And>) {
      andIds.reserve(andIds.size() + ids.size());
      andIds.insert(andIds.end(), ids.begin(), ids.end());
    } else if constexpr (is_instance_of_v<SubQuery, Not>) {
      notIds.reserve(notIds.size() + ids.size());
      notIds.insert(notIds.end(), ids.begin(), ids.end());
    }
  }
};
