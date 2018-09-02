#pragma once

#include <utils/utils.h>

#include <cstddef>

namespace reflection {

template<class Derived>
class reflector_base
{
private:
  template<typename Self>
  friend auto reflector(Self &&self)
  {
    return std::add_const_t<utils::remove_cvref_t<Self>>{};
  }

  template<typename Self>
  friend auto derived(Self &&self)
    -> utils::Delayed<utils::like_t<Self, Derived>, Self>
  {
    return static_cast<utils::like_t<Self, Derived>>(std::forward<Self>(self));
  }
};

template<class Reflection, class Derived, typename... Hierarchy>
using detect_is_member_type = decltype(
  on_call(
    std::declval<const Derived *>(),
    std::declval<const Reflection &>(),
    utils::type_list<Hierarchy...>{},
    utils::type_list<>{},
    std::declval<Derived &>()
  )
);

template<typename Derived>
constexpr auto is_reflector(const reflector_base<Derived> &) { return true;  }
constexpr auto is_reflector(...)                             { return false; }

template<typename Reflector>
struct InvokerImpl
{
  using type = typename Reflector::Invoker;
};

template<typename Reflector>
using Invoker = typename InvokerImpl<Reflector>::type;

template<typename T, typename... Hierarchy>
using fail_if_in_hierarchy = std::enable_if_t<
  utils::none_of<
    utils::remove_cvref_t<T>,
    Hierarchy...
  >,
  T
>;

} // namespace reflection

// TODO: Hard-code class names
// TODO: Rename 'member'
// TODO: Use 'struct' where possible
// TODO: Delay parameter for free function invoker is not necessary I think
// TODO: Remove more copy-paste

#define REFLECTION_REFLECTABLE_ADD_EMPTY_PREAMBLE(...)
#define REFLECTION_REFLECTABLE_ADD_EMPTY_INVOKER(...)
#define REFLECTION_REFLECTABLE_ADD_EMPTY_EXPOSER(ReflectorClassName, ...) \
  template<typename...>                                                   \
  struct ReflectorClassName {}                                            \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_TYPE_INVOKER(ReflectorClassName, member)        \
  template<class Delayed>                                                                 \
  class ReflectorClassName##Invoker                                                       \
  {                                                                                       \
  private:                                                                                \
    template<typename Obj>                                                                \
    friend auto call(const ReflectorClassName##Invoker &, utils::type_list<>, Obj &&obj)  \
      -> typename utils::remove_cvref_t<Obj>::member;                                     \
  }                                                                                       \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_TYPE_EXPOSER(ReflectorClassName, member)  \
  template<class Derived, typename... Hierarchy>                                    \
  class ReflectorClassName                                                          \
    : public reflection::reflector_base<Derived>                                    \
  {                                                                                 \
  private:                                                                          \
    template<typename Reflector>                                                    \
    friend class reflection::InvokerImpl;                                           \
                                                                                    \
    using Invoker = ReflectorClassName##Invoker<Derived>;                           \
                                                                                    \
  public:                                                                           \
    using member = utils::detected_or_t<                                            \
      void,                                                                         \
      detect_is_member_type,                                                        \
      Invoker,                                                                      \
      Derived,                                                                      \
      Hierarchy...>;                                                                \
  }                                                                                 \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_NON_TEMPLATE_INVOKER(ReflectorClassName, member) \
  template<typename Delayed>                                                                        \
  class ReflectorClassName##Invoker                                                                 \
  {                                                                                                 \
  private:                                                                                          \
    template<typename Obj>                                                                          \
    friend auto call(const ReflectorClassName##Invoker &, utils::type_list<>, Obj &&obj)            \
      SFINAEABLE_RETURN(std::forward<Obj>(obj).member())                                            \
  }                                                                                                 \

#define REFLECTION_INJECT_MEMBER_FUNCTION_NON_TEMPLATE(ReflectorClassName, member, CONST_QUALIFIER, REF_QUALIFIER, MOVE_FUNCTION) \
  auto member() CONST_QUALIFIER REF_QUALIFIER                                                                                     \
    SFINAEABLE_RETURN(                                                                                                            \
      on_call(                                                                                                                    \
        static_cast<utils::Delayed<Derived, Args...> *>(nullptr),                                                                 \
        ReflectorClassName##Invoker<Derived>{},                                                                                   \
        utils::type_list<Hierarchy...>{},                                                                                         \
        utils::type_list<>{},                                                                                                     \
        derived(MOVE_FUNCTION(*this))                                                                                             \
      )                                                                                                                           \
    )                                                                                                                             \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_NON_TEMPLATE_EXPOSER(ReflectorClassName, member)   \
  template<typename Derived, typename... Hierarchy>                                                   \
  class ReflectorClassName                                                                            \
    : public reflection::reflector_base<Derived>                                                      \
  {                                                                                                   \
  public:                                                                                             \
    REFLECTION_INJECT_MEMBER_FUNCTION_NON_TEMPLATE(ReflectorClassName, member,      , & ,          )  \
    REFLECTION_INJECT_MEMBER_FUNCTION_NON_TEMPLATE(ReflectorClassName, member,      , &&, std::move)  \
    REFLECTION_INJECT_MEMBER_FUNCTION_NON_TEMPLATE(ReflectorClassName, member, const, & ,          )  \
    REFLECTION_INJECT_MEMBER_FUNCTION_NON_TEMPLATE(ReflectorClassName, member, const, &&, std::move)  \
  }                                                                                                   \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_OPERATOR_INFIX_INVOKER(ReflectorClassName, member) \
  template<typename Delayed>                                                                          \
  class ReflectorClassName##Invoker                                                                   \
  {                                                                                                   \
  private:                                                                                            \
    template<typename Obj, typename Arg>                                                              \
    friend auto call(const ReflectorClassName##Invoker &, utils::type_list<>, Obj &&obj, Arg &&arg)   \
      SFINAEABLE_RETURN(std::forward<Obj>(obj) member std::forward<Arg>(arg))                         \
  }                                                                                                   \

#define REFLECTION_INJECT_OPERATOR_INFIX(ReflectorClassName, member, CONST_QUALIFIER, REF_QUALIFIER, MOVE_FUNCTION) \
  template<typename Arg>                                                                                            \
  auto operator member(Arg &&arg) CONST_QUALIFIER REF_QUALIFIER                                                     \
    SFINAEABLE_RETURN(                                                                                              \
      on_call(                                                                                                      \
        static_cast<                                                                                                \
          utils::Delayed<                                                                                           \
            Derived,                                                                                                \
            fail_if_in_hierarchy<Arg, ReflectorClassName, Hierarchy...>                                             \
          > *                                                                                                       \
        >(nullptr),                                                                                                 \
        ReflectorClassName##Invoker<Arg>{},                                                                         \
        utils::type_list<Hierarchy...>{},                                                                           \
        utils::type_list<>{},                                                                                       \
        derived(MOVE_FUNCTION(utils::delayed(*this, utils::type_list<Arg>{}))),                                     \
        std::forward<fail_if_in_hierarchy<Arg, ReflectorClassName, Hierarchy...>>(arg)                              \
      )                                                                                                             \
    )                                                                                                               \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_OPERATOR_INFIX_EXPOSER(ReflectorClassName, member) \
  template<typename Derived, typename... Hierarchy>                                                   \
  class ReflectorClassName                                                                            \
    : public reflection::reflector_base<Derived>                                                      \
  {                                                                                                   \
  public:                                                                                             \
    ReflectorClassName() = default;                                                                   \
    ReflectorClassName(const ReflectorClassName &) = default;                                         \
    ReflectorClassName(ReflectorClassName &&) = default;                                              \
    ReflectorClassName &operator=(const ReflectorClassName &) = default;                              \
    ReflectorClassName &operator=(ReflectorClassName &&) = default;                                   \
                                                                                                      \
    REFLECTION_INJECT_OPERATOR_INFIX(ReflectorClassName, member,      , & ,          )                \
    REFLECTION_INJECT_OPERATOR_INFIX(ReflectorClassName, member,      , &&, std::move)                \
    REFLECTION_INJECT_OPERATOR_INFIX(ReflectorClassName, member, const, & ,          )                \
    REFLECTION_INJECT_OPERATOR_INFIX(ReflectorClassName, member, const, &&, std::move)                \
  }                                                                                                   \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_INVOKER(ReflectorClassName, member)                    \
  template<typename Delay>                                                                                \
  class ReflectorClassName##Invoker                                                                       \
  {                                                                                                       \
  private:                                                                                                \
    template<typename Obj, typename... Args>                                                              \
    friend auto call(const ReflectorClassName##Invoker &, utils::type_list<>, Obj &&obj, Args &&... args) \
      SFINAEABLE_RETURN(std::forward<Obj>(obj).member(std::forward<Args>(args)...))                       \
                                                                                                          \
    template<typename... ExplicitArgs, typename Obj, typename... Args>                                    \
    friend auto call(                                                                                     \
      const ReflectorClassName##Invoker &,                                                                \
      utils::type_list<ExplicitArgs...>,                                                                  \
      Obj &&obj,                                                                                          \
      Args &&... args                                                                                     \
    )                                                                                                     \
      CONSTRAINED_SFINAEABLE_RETURN(                                                                      \
        sizeof...(ExplicitArgs) != 0,                                                                     \
        std::forward<Obj>(obj).template member<ExplicitArgs...>(std::forward<Args>(args)...)              \
      )                                                                                                   \
  }                                                                                                       \

#define REFLECTION_INJECT_MEMBER_FUNCTION(ReflectorClassName, member, CONST_QUALIFIER, REF_QUALIFIER, MOVE_FUNCTION)  \
  template<typename... ExplicitArgs, typename... Args>                                                                \
  auto member(Args &&... args) CONST_QUALIFIER REF_QUALIFIER                                                          \
    SFINAEABLE_RETURN(                                                                                                \
      on_call(                                                                                                        \
        static_cast<utils::Delayed<Derived, Args...> *>(nullptr),                                                     \
        ReflectorClassName##Invoker<Derived>{},                                                                       \
        utils::type_list<Hierarchy...>{},                                                                             \
        utils::type_list<ExplicitArgs...>{},                                                                          \
        derived(MOVE_FUNCTION(utils::delayed(*this, utils::type_list<ExplicitArgs...>{}))),                           \
        std::forward<Args>(args)...                                                                                   \
      )                                                                                                               \
    )                                                                                                                 \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_EXPOSER(ReflectorClassName, member)  \
  template<typename Derived, typename... Hierarchy>                                     \
  class ReflectorClassName                                                              \
    : public reflection::reflector_base<Derived>                                        \
  {                                                                                     \
  public:                                                                               \
    REFLECTION_INJECT_MEMBER_FUNCTION(ReflectorClassName, member, const, & ,          ) \
    REFLECTION_INJECT_MEMBER_FUNCTION(ReflectorClassName, member, const, &&, std::move) \
    REFLECTION_INJECT_MEMBER_FUNCTION(ReflectorClassName, member,      , & ,          ) \
    REFLECTION_INJECT_MEMBER_FUNCTION(ReflectorClassName, member,      , &&, std::move) \
  }                                                                                     \

#define REFLECTION_REFLECTABLE_ADD_FREE_FUNCTION_INVOKER(ReflectorClassName, member)                \
  template<typename Delay>                                                                          \
  class ReflectorClassName##Invoker                                                                 \
  {                                                                                                 \
  public:                                                                                           \
    template<typename Obj, typename... Args>                                                        \
    friend auto call(                                                                               \
      const ReflectorClassName##Invoker &,                                                          \
      utils::type_list<>,                                                                           \
      Obj &&obj,                                                                                    \
      Args &&... args                                                                               \
    )                                                                                               \
      -> decltype(adl_tricks::member(std::forward<Obj>(obj), std::forward<Args>(args)...))          \
    {                                                                                               \
      using reflectable::member;                                                                    \
      return member(std::forward<Obj>(obj), std::forward<Args>(args)...);                           \
    }                                                                                               \
                                                                                                    \
    template<typename... ExplicitArgs, typename Obj, typename... Args>                              \
    friend auto call(                                                                               \
      const ReflectorClassName##Invoker &,                                                          \
      utils::type_list<ExplicitArgs...>,                                                            \
      Obj &&obj,                                                                                    \
      Args &&... args                                                                               \
    )                                                                                               \
      -> decltype(                                                                                  \
           adl_tricks::member<ExplicitArgs...>(std::forward<Obj>(obj), std::forward<Args>(args)...) \
         )                                                                                          \
    {                                                                                               \
      using reflectable::member;                                                                    \
      return member<ExplicitArgs...>(std::forward<Obj>(obj), std::forward<Args>(args)...);          \
    }                                                                                               \
  }                                                                                                 \

#define REFLECTION_REFLECTABLE_ADD_FREE_FUNCTION_EXPOSER(ReflectorClassName, member)                  \
  template<typename Derived, typename Delay = void, typename... Hierarchy>                            \
  class ReflectorClassName                                                                            \
    : public reflection::reflector_base<Derived>                                                      \
  {                                                                                                   \
  public:                                                                                             \
    template<typename... ExplicitArgs, typename Self, typename... Args>                               \
    friend auto member(Self &&self, Args &&... args)                                                  \
      SFINAEABLE_RETURN(                                                                              \
        on_call(                                                                                      \
          static_cast<utils::Delayed<Derived, Args...> *>(nullptr),                                   \
          ReflectorClassName##Invoker<Delay>{},                                                       \
          utils::type_list<Hierarchy...>{},                                                           \
          utils::type_list<ExplicitArgs...>{},                                                        \
          std::forward<Self>(self),                                                                   \
          std::forward<Args>(args)...                                                                 \
        )                                                                                             \
      )                                                                                               \
  }                                                                                                   \

#define REFLECTION_REFLECTABLE_ADD_FREE_FUNCTION_OPERATOR_EXPOSER(ReflectorClassName, member) \
  REFLECTION_REFLECTABLE_ADD_FREE_FUNCTION_EXPOSER(ReflectorClassName, operator member)       \

#define REFLECTION_REFLECTABLE_ADD_FREE_FUNCTION_PREAMBLE(member)        \
  namespace reflectable {                                                \
                                                                         \
  /* This member is there purely to allow for doing e.g. */              \
  /* 'using reflection::foo', which will treat 'foo' as */               \
  /* a function template.                                */              \
  template<typename... Args>                                             \
  auto member(Args...)                                                   \
    -> std::enable_if_t<utils::always_false<Args...>>;                   \
                                                                         \
  } /* namespace reflectable */                                          \
                                                                         \
  namespace adl_tricks {                                                 \
                                                                         \
  using reflectable::member;                                             \
                                                                         \
  template<typename... Args>                                             \
  auto member(Args &&... args)                                           \
    -> decltype(member(std::forward<Args>(args)...));                    \
                                                                         \
  /* This function template can be used within trailing return types, */ \
  /* in combination with SFINAE, such that ADL still works if in the  */ \
  /* function's body a 'using declaration' was used.                  */ \
  template<typename... ExplicitArgs, typename... Args>                   \
  auto member(Args &&... args)                                           \
    -> std::enable_if_t<                                                 \
         sizeof...(ExplicitArgs) != 0,                                   \
         decltype(member<ExplicitArgs...>(std::forward<Args>(args)...))  \
       >;                                                                \
                                                                         \
  } /* namespace adl_tricks */                                           \
