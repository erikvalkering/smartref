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

#define REFLECTION_REFLECTABLE_ADD_EMPTY_PREAMBLE(...)

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

#define REFLECTION_INJECT_MEMBER_FUNCTION(member, CONST_QUALIFIER, REF_QUALIFIER, MOVE_FUNCTION) \
  auto member() CONST_QUALIFIER REF_QUALIFIER                                                    \
    SFINAEABLE_RETURN(                                                                           \
      on_call(                                                                                   \
        reflector(*this),                                                                        \
        derived(MOVE_FUNCTION(*this)),                                                           \
        utils::type_list<>{}                                                                     \
      )                                                                                          \
    )                                                                                            \

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
    REFLECTION_INJECT_MEMBER_FUNCTION(member,      , & ,          )                                   \
    REFLECTION_INJECT_MEMBER_FUNCTION(member,      , &&, std::move)                                   \
    REFLECTION_INJECT_MEMBER_FUNCTION(member, const, & ,          )                                   \
    REFLECTION_INJECT_MEMBER_FUNCTION(member, const, &&, std::move)                                   \
  }                                                                                                   \

#define REFLECTION_INJECT_ASSIGNMENT_OPERATOR(member, CONST_QUALIFIER, REF_QUALIFIER, MOVE_FUNCTION)  \
  template<typename Arg>                                                                              \
  auto member(Arg &&arg) CONST_QUALIFIER REF_QUALIFIER                                                \
    SFINAEABLE_RETURN(                                                                                \
      on_call(                                                                                        \
        reflector(utils::delayed(*this, utils::type_list<Arg>{})),                                    \
        derived(MOVE_FUNCTION(utils::delayed(*this, utils::type_list<Arg>{}))),                       \
        utils::type_list<>{},                                                                         \
        std::forward<Arg>(arg)                                                                        \
      )                                                                                               \
    )                                                                                                 \

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
    REFLECTION_INJECT_ASSIGNMENT_OPERATOR(member,      , & ,          )                                       \
    REFLECTION_INJECT_ASSIGNMENT_OPERATOR(member,      , &&, std::move)                                       \
    REFLECTION_INJECT_ASSIGNMENT_OPERATOR(member, const, & ,          )                                       \
    REFLECTION_INJECT_ASSIGNMENT_OPERATOR(member, const, &&, std::move)                                       \
  }                                                                                                           \

#define REFLECTION_INJECT_MEMBER_FUNCTION_TEMPLATE(member, CONST_QUALIFIER, REF_QUALIFIER, MOVE_FUNCTION) \
  template<typename... ExplicitArgs, typename... Args>                                                    \
  auto member(Args &&... args) CONST_QUALIFIER REF_QUALIFIER                                              \
    SFINAEABLE_RETURN(                                                                                    \
      on_call(                                                                                            \
        reflector(utils::delayed(*this, utils::type_list<ExplicitArgs...>{})),                            \
        derived(MOVE_FUNCTION(utils::delayed(*this, utils::type_list<ExplicitArgs...>{}))),               \
        utils::type_list<ExplicitArgs...>{},                                                              \
        std::forward<Args>(args)...                                                                       \
      )                                                                                                   \
    )                                                                                                     \

#define REFLECTION_REFLECTABLE_ADD_MEMBER_FUNCTION_REFLECTOR(ReflectorClassName, member)                \
  template<typename Delay>                                                                              \
  class MemberFunctionInvoker                                                                           \
  {                                                                                                     \
  private:                                                                                              \
    /* TODO: See if we can merge the two call functions using if constexpr */                           \
    template<typename Obj, typename... Args>                                                            \
    friend auto call(const MemberFunctionInvoker &, Obj &&obj, utils::type_list<>, Args &&... args)     \
      SFINAEABLE_RETURN(std::forward<Obj>(obj).member(std::forward<Args>(args)...))                     \
                                                                                                        \
    template<typename... ExplicitArgs, typename Obj, typename... Args>                                  \
    friend auto call(                                                                                   \
      const MemberFunctionInvoker &,                                                                    \
      Obj &&obj,                                                                                        \
      utils::type_list<ExplicitArgs...>,                                                                \
      Args &&... args                                                                                   \
    )                                                                                                   \
      CONSTRAINED_SFINAEABLE_RETURN(                                                                    \
        sizeof...(ExplicitArgs) != 0,                                                                   \
        std::forward<Obj>(obj).template member<ExplicitArgs...>(std::forward<Args>(args)...)            \
      )                                                                                                 \
  };                                                                                                    \
                                                                                                        \
  template<typename Derived>                                                                            \
  class MemberFunctionExposer                                                                           \
    : public reflection::reflector_base<Derived>                                                        \
    , public MemberFunctionInvoker<Derived>                                                             \
  {                                                                                                     \
  public:                                                                                               \
    REFLECTION_INJECT_MEMBER_FUNCTION_TEMPLATE(member, const, & ,          )                            \
    REFLECTION_INJECT_MEMBER_FUNCTION_TEMPLATE(member, const, &&, std::move)                            \
    REFLECTION_INJECT_MEMBER_FUNCTION_TEMPLATE(member,      , & ,          )                            \
    REFLECTION_INJECT_MEMBER_FUNCTION_TEMPLATE(member,      , &&, std::move)                            \
  };                                                                                                    \
                                                                                                        \
  template<typename Derived>                                                                            \
  class ReflectorClassName                                                                              \
    : public MemberFunctionExposer<Derived>                                                             \
  {                                                                                                     \
  }                                                                                                     \

#define REFLECTION_REFLECTABLE_ADD_FREE_FUNCTION_REFLECTOR(ReflectorClassName, member)                  \
  template<typename Delay>                                                                              \
  class FreeFunctionInvoker                                                                             \
  {                                                                                                     \
  public:                                                                                               \
    template<typename Obj, typename... Args>                                                            \
    friend auto call(                                                                                   \
      const FreeFunctionInvoker &,                                                                      \
      Obj &&obj,                                                                                        \
      utils::type_list<>,                                                                               \
      Args &&... args                                                                                   \
    )                                                                                                   \
      -> decltype(adl_tricks::member(std::forward<Obj>(obj), std::forward<Args>(args)...))              \
    {                                                                                                   \
      /* The reason why this needs to be put in a separate class,                                    */ \
      /* is because otherwise the ADL would find the function                                        */ \
      /* defined in the smart reference itself.                                                      */ \
      return adl_tricks::member(std::forward<Obj>(obj), std::forward<Args>(args)...);                   \
    }                                                                                                   \
                                                                                                        \
    template<typename... ExplicitArgs, typename Obj, typename... Args>                                  \
    friend auto call(                                                                                   \
      const FreeFunctionInvoker &,                                                                      \
      Obj &&obj,                                                                                        \
      utils::type_list<ExplicitArgs...>,                                                                \
      Args &&... args                                                                                   \
    )                                                                                                   \
      -> decltype(                                                                                      \
           adl_tricks::member<ExplicitArgs...>(std::forward<Obj>(obj), std::forward<Args>(args)...)     \
         )                                                                                              \
    {                                                                                                   \
      /* The reason why this needs to be put in a separate class, */                                    \
      /* is because otherwise the ADL would find the function     */                                    \
      /* defined in the smart reference itself.                   */                                    \
      using reflectable::member;                                                                        \
      return member<ExplicitArgs...>(std::forward<Obj>(obj), std::forward<Args>(args)...);              \
    }                                                                                                   \
  };                                                                                                    \
                                                                                                        \
  template<typename Derived, typename Delay = void>                                                     \
  class FreeFunctionExposer                                                                             \
    : public reflection::reflector_base<Derived>                                                        \
    , public FreeFunctionInvoker<Delay>                                                                 \
  {                                                                                                     \
  public:                                                                                               \
    template<typename... ExplicitArgs, typename Self, typename... Args>                                 \
    friend auto member(Self &&self, Args &&... args)                                                    \
      SFINAEABLE_RETURN(                                                                                \
        on_call(                                                                                        \
          utils::Delayed<FreeFunctionExposer, Args...>{},                                               \
          derived(std::forward<Self>(self)),                                                            \
          utils::type_list<ExplicitArgs...>{},                                                          \
          std::forward<Args>(args)...                                                                   \
        )                                                                                               \
      )                                                                                                 \
  };                                                                                                    \
                                                                                                        \
  template<typename Derived, typename Delay = void>                                                     \
  class ReflectorClassName                                                                              \
    : public FreeFunctionExposer<Derived, Delay>                                                        \
  {                                                                                                     \
  }                                                                                                     \

#define REFLECTION_REFLECTABLE_ADD_FREE_FUNCTION_PREAMBLE(member)        \
  namespace reflectable {                                                \
                                                                         \
  /* This member is there purely to allow for doing e.g. */              \
  /* 'using reflection::foo', which will import 'foo' as */              \
  /* a function template.                                */              \
  template<typename... Args>                                             \
  auto member(...) -> std::enable_if_t<utils::always_false<Args...>>;    \
                                                                         \
  } /* namespace reflectable */                                          \
                                                                         \
  namespace adl_tricks {                                                 \
                                                                         \
  using reflectable::member;                                             \
                                                                         \
  /* This function template can be used within trailing return types, */ \
  /* in combination with SFINAE, such that ADL still works if in the  */ \
  /* function's body a 'using declaration' was used.                  */ \
  template<typename... ExplicitArgs, typename... Args>                   \
  auto member(Args &&... args)                                           \
    -> decltype(member<ExplicitArgs...>(std::forward<Args>(args)...));   \
                                                                         \
  } /* namespace adl_tricks */                                           \
