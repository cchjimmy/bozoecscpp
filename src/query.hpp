#pragma once
#include "component.hpp"
#include <type_traits>

template <typename... Components> class ISubQuery {
public:
  static int getMask(ComponentManager &cm) {
    return (0 | ... | cm.getMask<Components>());
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
  int andMask = 0, notMask = 0;

  Query(ComponentManager &cm) { (processSubQuery<SubQueries>(cm), ...); }

private:
  template <typename SubQuery>
  inline void processSubQuery(ComponentManager &cm) {
    if constexpr (is_instance_of_v<SubQuery, And>) {
      andMask |= SubQuery::getMask(cm);
    } else if constexpr (is_instance_of_v<SubQuery, Not>) {
      notMask |= SubQuery::getMask(cm);
    }
  }
};
