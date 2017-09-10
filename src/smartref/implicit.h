#pragma once

#include <reflection/reflection.h>

#include <type_traits>

namespace smartref {

template<typename Delegate, class Derived>
struct Forwarder
{
    // TODO: Maybe not access this directly,
    //       but using a helper function.
    using Class = Delegate;

    template<typename Self, typename F, typename... Args>
    auto operator()(Self &self, F f, Args &&... args)
    {
        static_assert(std::is_base_of<Self, Derived>::value);

        //! Downcast to the derived class
        auto &derived = static_cast<Derived &>(self);

        //! Now invoke the conversion operator
        auto &delegate = static_cast<Delegate &>(derived);

        return f(delegate, std::forward<decltype(args)>(args)...);
    }
};

template<typename Delegate, class Derived, typename Reflection>
using using_member_t = decltype(
    reflection::reify(
        Reflection{},
        Forwarder<Delegate, Derived>{}
    )
);

template<typename tag, typename Delegate, class Derived, typename Members>
struct ReflectedMembersImpl;

template<typename tag, typename Delegate, class Derived, typename... Reflections>
struct ReflectedMembersImpl<tag, Delegate, Derived, std::tuple<Reflections...>>
    : using_member_t<Delegate, Derived, Reflections>...
{
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
