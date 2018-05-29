#pragma once

#include <utils/utils.h>

#include <cstddef>

namespace reflection {

template<class Derived>
class reflector_base
{
private:
  template<typename Self>
  friend auto derived(Self &&self)
    -> utils::Delayed<utils::like_t<Self, Derived>, Self>
  {
    return static_cast<utils::like_t<Self, Derived>>(std::forward<Self>(self));
  }

  template<typename Self>
  friend auto reflector(Self &&self)
  {
    return std::add_const_t<utils::remove_cvref_t<Self>>{};
  }
};

template<class Reflection, class Derived>
using detect_is_member_type = decltype(
  on_call(
    std::declval<const Reflection &>(),
    std::declval<Derived &>(),
    utils::type_list<>{}
  )
);

} // namespace reflection

#define REFLECTION_REFLECTABLE_ADD_EMPTY_REFLECTOR(ReflectorClassName, ...) \
  template<class>                                                           \
  struct ReflectorClassName {}                                              \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_TYPE_REFLECTOR(ReflectorClassName, member)  \
  template<class Derived>                                                             \
  class ReflectorClassName                                                            \
    : public reflection::reflector_base<Derived>                                      \
  {                                                                                   \
  private:                                                                            \
    template<typename Obj, typename... Args>                                          \
    friend auto call(const ReflectorClassName &, Obj &&obj, utils::type_list<>)       \
      -> typename utils::remove_cvref_t<Obj>::member;                                 \
                                                                                      \
  public:                                                                             \
    using member = utils::detected_or_t<                                              \
      void,                                                                           \
      detect_is_member_type,                                                          \
      ReflectorClassName,                                                             \
      Derived>;                                                                       \
  }                                                                                   \

// TODO: -cmaster Get rid of code duplication
#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR_NON_TEMPLATE(ReflectorClassName, member) \
  template<typename Derived>                                                                          \
  class ReflectorClassName                                                                            \
    : public reflection::reflector_base<Derived>                                                      \
  {                                                                                                   \
  private:                                                                                            \
    template<typename Obj>                                                                            \
    friend auto call(const ReflectorClassName &, Obj &&obj, utils::type_list<>)                       \
      SFINAEABLE_RETURN(std::forward<Obj>(obj).member())                                              \
                                                                                                      \
  public:                                                                                             \
    auto member() &                                                                                   \
      SFINAEABLE_RETURN(                                                                              \
        on_call(                                                                                      \
          reflector(*this),                                                                           \
          derived(*this),                                                                             \
          utils::type_list<>{}                                                                        \
        )                                                                                             \
      )                                                                                               \
                                                                                                      \
    auto member() &&                                                                                  \
      SFINAEABLE_RETURN(                                                                              \
        on_call(                                                                                      \
          reflector(*this),                                                                           \
          derived(std::move(*this)),                                                                  \
          utils::type_list<>{}                                                                        \
        )                                                                                             \
      )                                                                                               \
                                                                                                      \
    auto member() const &                                                                             \
      SFINAEABLE_RETURN(                                                                              \
        on_call(                                                                                      \
          reflector(*this),                                                                           \
          derived(*this),                                                                             \
          utils::type_list<>{}                                                                        \
        )                                                                                             \
      )                                                                                               \
                                                                                                      \
    auto member() const &&                                                                            \
      SFINAEABLE_RETURN(                                                                              \
        on_call(                                                                                      \
          reflector(*this),                                                                           \
          derived(std::move(*this)),                                                                  \
          utils::type_list<>{}                                                                        \
        )                                                                                             \
      )                                                                                               \
  }                                                                                                   \

// TODO: Get rid of code duplication
#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR_ASSIGNMENT_OPERATOR(ReflectorClassName, member)  \
  template<typename Derived>                                                                                  \
  class ReflectorClassName                                                                                    \
    : public reflection::reflector_base<Derived>                                                              \
  {                                                                                                           \
  private:                                                                                                    \
    template<typename Obj, typename Arg>                                                                      \
    friend auto call(const ReflectorClassName &, Obj &&obj, utils::type_list<>, Arg &&arg)                    \
      SFINAEABLE_RETURN(std::forward<Obj>(obj) = std::forward<Arg>(arg))                                      \
                                                                                                              \
  public:                                                                                                     \
    ReflectorClassName() = default;                                                                           \
    ReflectorClassName(const ReflectorClassName &) = default;                                                 \
    ReflectorClassName(ReflectorClassName &&) = default;                                                      \
    ReflectorClassName &operator=(const ReflectorClassName &) = default;                                      \
    ReflectorClassName &operator=(ReflectorClassName &&) = default;                                           \
                                                                                                              \
    template<typename Arg>                                                                                    \
    auto member(Arg &&arg) &                                                                                  \
      SFINAEABLE_RETURN(                                                                                      \
        on_call(                                                                                              \
          reflector(utils::delayed(*this, utils::type_list<Arg>{})),                                          \
          derived(utils::delayed(*this, utils::type_list<Arg>{})),                                            \
          utils::type_list<>{},                                                                               \
          std::forward<Arg>(arg)                                                                              \
        )                                                                                                     \
      )                                                                                                       \
                                                                                                              \
    template<typename Arg>                                                                                    \
    auto member(Arg &&arg) &&                                                                                 \
      SFINAEABLE_RETURN(                                                                                      \
        on_call(                                                                                              \
          reflector(utils::delayed(*this, utils::type_list<Arg>{})),                                          \
          derived(std::move(utils::delayed(*this, utils::type_list<Arg>{}))),                                 \
          utils::type_list<>{},                                                                               \
          std::forward<Arg>(arg)                                                                              \
        )                                                                                                     \
      )                                                                                                       \
                                                                                                              \
    template<typename Arg>                                                                                    \
    auto member(Arg &&arg) const &                                                                            \
      SFINAEABLE_RETURN(                                                                                      \
        on_call(                                                                                              \
          reflector(utils::delayed(*this, utils::type_list<Arg>{})),                                          \
          derived(utils::delayed(*this, utils::type_list<Arg>{})),                                            \
          utils::type_list<>{},                                                                               \
          std::forward<Arg>(arg)                                                                              \
        )                                                                                                     \
      )                                                                                                       \
                                                                                                              \
    template<typename Arg>                                                                                    \
    auto member(Arg &&arg) const &&                                                                           \
      SFINAEABLE_RETURN(                                                                                      \
        on_call(                                                                                              \
          reflector(utils::delayed(*this, utils::type_list<Arg>{})),                                          \
          derived(std::move(utils::delayed(*this, utils::type_list<Arg>{}))),                                 \
          utils::type_list<>{},                                                                               \
          std::forward<Arg>(arg)                                                                              \
        )                                                                                                     \
      )                                                                                                       \
  }                                                                                                           \

#define MEMBER1(member, CONST_QUALIFIER, REF_QUALIFIER, MOVE_FUNCTION)                      \
  template<typename... ExplicitArgs, typename... Args>                                      \
  auto member(Args &&... args) CONST_QUALIFIER REF_QUALIFIER                                \
    SFINAEABLE_RETURN(                                                                      \
      on_call(                                                                              \
        reflector(utils::delayed(*this, utils::type_list<ExplicitArgs...>{})),              \
        derived(MOVE_FUNCTION(utils::delayed(*this, utils::type_list<ExplicitArgs...>{}))), \
        utils::type_list<ExplicitArgs...>{},                                                \
        std::forward<Args>(args)...                                                         \
      )                                                                                     \
    )                                                                                       \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR(ReflectorClassName, member)                \
  template<typename Derived>                                                                            \
  class ReflectorClassName                                                                              \
    : public reflection::reflector_base<Derived>                                                        \
  {                                                                                                     \
  private:                                                                                              \
    template<typename Obj, typename... Args>                                                            \
    friend auto call(const ReflectorClassName &, Obj &&obj, utils::type_list<>, Args &&... args)        \
      SFINAEABLE_RETURN(std::forward<Obj>(obj).member(std::forward<Args>(args)...))                     \
                                                                                                        \
    template<typename... ExplicitArgs, typename Obj, typename... Args>                                  \
    friend auto call(                                                                                   \
      const ReflectorClassName &,                                                                       \
      Obj &&obj,                                                                                        \
      utils::type_list<ExplicitArgs...>,                                                                \
      Args &&... args                                                                                   \
    )                                                                                                   \
      -> std::enable_if_t<                                                                              \
        sizeof...(ExplicitArgs) != 0,                                                                   \
        decltype(std::forward<Obj>(obj).template member<ExplicitArgs...>(std::forward<Args>(args)...))  \
      >                                                                                                 \
    {                                                                                                   \
      return std::forward<Obj>(obj).template member<ExplicitArgs...>(std::forward<Args>(args)...);      \
    }                                                                                                   \
                                                                                                        \
  public:                                                                                               \
    MEMBER1(member, const, & ,          )                                                               \
    MEMBER1(member, const, &&, std::move)                                                               \
    MEMBER1(member,      , & ,          )                                                               \
    MEMBER1(member,      , &&, std::move)                                                               \
  }                                                                                                     \
