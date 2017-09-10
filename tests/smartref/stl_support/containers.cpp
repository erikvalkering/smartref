#include <reflection/reflection.h>
#include <smartref/smartref.h>

#include <vector>

namespace tests_containers {

template<typename T>
struct Ref : smartref::using_<T>
{
  T ref;

  operator T &()
  {
    return ref;
  }
};

using T = std::vector<int>;

using reflection::reflect;

//! Member-types
static_assert(reflect<Ref<T>::value_type>             == reflect<T::value_type>);
static_assert(reflect<Ref<T>::allocator_type>         == reflect<T::allocator_type>);
static_assert(reflect<Ref<T>::size_type>              == reflect<T::size_type>);
static_assert(reflect<Ref<T>::difference_type>        == reflect<T::difference_type>);
static_assert(reflect<Ref<T>::reference>              == reflect<T::reference>);
static_assert(reflect<Ref<T>::const_reference>        == reflect<T::const_reference>);
static_assert(reflect<Ref<T>::pointer>                == reflect<T::pointer>);
static_assert(reflect<Ref<T>::const_pointer>          == reflect<T::const_pointer>);
static_assert(reflect<Ref<T>::iterator>               == reflect<T::iterator>);
static_assert(reflect<Ref<T>::const_iterator>         == reflect<T::const_iterator>);
static_assert(reflect<Ref<T>::reverse_iterator>       == reflect<T::reverse_iterator>);
static_assert(reflect<Ref<T>::const_reverse_iterator> == reflect<T::const_reverse_iterator>);

//! Member functions
// TODO: (constructor)
// TODO: operator=
// TODO: assign
// TODO: get_allocator

//! Element access
// TODO: at
// TODO: operator[]
// TODO: front
// TODO: back
// TODO: data

//! Iterators
static_assert(reflect<decltype(std::declval<Ref<T>>().begin())> == reflect<decltype(std::declval<T>().begin())>);
// TODO: cbegin
static_assert(reflect<decltype(std::declval<Ref<T>>().end())> == reflect<decltype(std::declval<T>().end())>);
// TODO: cend
// TODO: rbegin
// TODO: crbegin
// TODO: rend
// TODO: crend

//! Capacity
// TODO: empty
// TODO: size
// TODO: max_size
// TODO: reserve
// TODO: capacity
// TODO: shrink_to_fit

//! Modifiers
// TODO: clear
// TODO: insert
// TODO: emplace
// TODO: erase
static_assert(reflect<decltype(std::declval<Ref<T>>().push_back(0))> == reflect<decltype(std::declval<T>().push_back(0))>);
// TODO: emplace_back
// TODO: pop_back
// TODO: resize
// TODO: swap

//! Non-member functions
// TODO: operator==
// TODO: operator!=
// TODO: operator<
// TODO: operator<=
// TODO: operator>
// TODO: operator>=
// TODO: std::swap

// TODO: Deduction guides

} // namespace tests_containers
