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
constexpr auto is_valid = [](auto expression)
{
  auto expected = reflect<decltype(expression(std::declval<    T >()))>;
  auto actual   = reflect<decltype(expression(std::declval<Ref<T>>()))>;

  return actual == expected;
};

#define IS_VALID(expression) is_valid([](auto &&_) {expression;})

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
// TODO: Test all overloads
// TODO: (constructor)
// TODO: Why does operator= work? What does it do?
static_assert(IS_VALID(_.operator=(_)));
static_assert(IS_VALID(_ = _));
static_assert(IS_VALID(_.assign(0, 0)));
static_assert(IS_VALID(_.assign(begin(_), end(_))));
static_assert(IS_VALID(_.get_allocator()));

//! Element access
static_assert(IS_VALID(_.at(T::size_type{})));
static_assert(IS_VALID(_.operator[](0)));
static_assert(IS_VALID(_[0]));
static_assert(IS_VALID(_.front()));
static_assert(IS_VALID(_.back()));
static_assert(IS_VALID(_.data()));

//! Iterators
static_assert(IS_VALID(_.begin()));
static_assert(IS_VALID(_.cbegin()));
static_assert(IS_VALID(_.end()));
static_assert(IS_VALID(_.cend()));
static_assert(IS_VALID(_.rbegin()));
static_assert(IS_VALID(_.crbegin()));
static_assert(IS_VALID(_.rend()));
static_assert(IS_VALID(_.crend()));

//! Capacity
static_assert(IS_VALID(_.empty()));
static_assert(IS_VALID(_.size()));
static_assert(IS_VALID(_.max_size()));
static_assert(IS_VALID(_.reserve(T::size_type{})));
static_assert(IS_VALID(_.capacity()));
static_assert(IS_VALID(_.shrink_to_fit()));

//! Modifiers
static_assert(IS_VALID(_.clear()));
static_assert(IS_VALID(_.insert(begin(_), 0)));
static_assert(IS_VALID(_.emplace(begin(_), 0)));
static_assert(IS_VALID(_.template emplace<int>(begin(_), 0)));
static_assert(IS_VALID(_.erase(begin(_))));
static_assert(IS_VALID(_.push_back(0)));
static_assert(IS_VALID(_.emplace_back(0)));
static_assert(IS_VALID(_.template emplace_back<int>(0)));
static_assert(IS_VALID(_.pop_back()));
static_assert(IS_VALID(_.resize(T::size_type{})));
static_assert(IS_VALID(_.swap(_)));

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
