#pragma once

#include <reflection/reflection.h>

#include <utils/utils.h>

#include <type_traits>

namespace smartref {

// TODO: -cmaster
//       The semantics of the reify function are completely lost:
//       We should somehow specify that we want to instantiate a class
//       that implements the member (function), from which we inherit.
//       Maybe a better api would be to transform the reflection into another
//       reflection (of the inheritable class), and simply do a reify on that.
//       That basically requires only a single reify overload.
//       This might also raise the question whether we actually need this function,
//       because it looks very similar to decltype. Maybe we could even make
//       it an alias template, by passing the reflection (type) as the first
//       argument (or even a reflection value), and return the inheritable class.
// TODO: -cmaster What about:
//
//       using vector_emplace_back = Reflection<...>;
//       auto emplace_back_forwarder = make_forwarder(vector_emplace_back{});
//       using emplace_back_forwarder_class = unreflect<emplace_back_forwarder>;
//
//       template<auto reflection>
//       using unreflect = decltype(unreflect_impl(reflection));

template<class Derived, typename Reflection>
using using_member_t = decltype(
  reflection::reify<Derived>(
    Reflection{}
  )
);

template<class Derived, typename Members>
struct MembersImpl;

template<class Derived, typename... Reflections>
struct MembersImpl<Derived, utils::type_list<Reflections...>>
  : using_member_t<Derived, Reflections>...
{
  using using_member_t<Derived, Reflections>::operator=...;
};

template<
  class Delegate,
  class Derived>
using Members = MembersImpl<
  Derived,
  decltype(members(reflection::reflect<utils::Delayed<Delegate, Derived>>))
>;

} // namespace smartref
