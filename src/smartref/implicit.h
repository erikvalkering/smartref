#pragma once

#include <reflection/reflection.h>

#include <type_traits>

namespace smartref {

// TODO: The semantics of the reify function are completely lost:
//       We should somehow specify that we want to instantiate a class
//       that implements the member (function), from which we inherit.
//       Maybe a better api would be to transform the reflection into another
//       reflection (of the inheritable class), and simply do a reify on that.
//       That basically requires only a single reify overload.
//       This might also raise the question whether we actually need this function,
//       because it looks very similar to decltype. Maybe we could even make
//       it an alias template, by passing the reflection (type) as the first
//       argument (or even a reflection value), and return the inheritable class.
template<typename Delegate, class Derived, typename Reflection>
using using_member_t = decltype(
    reflection::reify<Derived>(
        Reflection{}
    )
);

template<typename tag, typename Delegate, class Derived, typename Members>
struct ReflectedMembersImpl;

template<typename tag, typename Delegate, class Derived, typename... Reflections>
struct ReflectedMembersImpl<tag, Delegate, Derived, std::tuple<Reflections...>>
    : using_member_t<Delegate, Derived, Reflections>...
{
    using using_member_t<Delegate, Derived, Reflections>::operator=...;
};

template<
    typename Delegate,
    class Derived,
    class ReflectionClass = Delegate>
using ReflectedMembers = ReflectedMembersImpl<
    ReflectionClass,
    Delegate,
    Derived,
    decltype(members(reflection::reflect<ReflectionClass>))
>;

} // namespace smartref
