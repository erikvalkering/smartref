#include "../include/utils/utils.h"

#include <iostream>
#include <type_traits>

using namespace std;

namespace reflection {
template<typename CRTP, typename... Ignore>
struct Exposer;
} // namespace reflection

namespace smartref {

// template<typename...>
// struct Nested {
template<typename Delegate, typename CRTP, typename... Ignore>
struct Member : reflection::Exposer<utils::Delayed<CRTP, Delegate>, Member<Delegate, CRTP, Ignore...>, Ignore...>
{
  Member() {}
  Member(const Member &) {}
  Member(Member &&) {}
  Member &operator=(const Member &) {return *this;}
  Member &operator=(Member &&) {return *this;}

  using reflection::Exposer<utils::Delayed<CRTP, Delegate>, Member<Delegate, CRTP, Ignore...>, Ignore...>::operator=;
};
// };

template<typename Delegate, typename CRTP, typename... Ignore>
struct using_ : /*Nested<Delegate, CRTP>::template */Member<Delegate, CRTP, using_<Delegate, CRTP, Ignore...>, Ignore...>
// struct using_ : reflection::Exposer<CRTP>
{
  using_() = default;
  using_(const using_ &) = default;
  using_(using_ &&) = default;
  using_ &operator=(const using_ &) = default;
  using_ &operator=(using_ &&) = default;

  using /*Nested<Delegate, CRTP>::template */Member<Delegate, CRTP, using_<Delegate, CRTP, Ignore...>, Ignore...>::operator=;
  // using reflection::Exposer<CRTP>::operator=;
};

template<typename Delegate, typename Derived, typename... Ignore>
auto delegate_type_impl(const using_<Delegate, Derived, Ignore...> &) -> Delegate;

template<typename Using_, typename... Args>
using DelegateType = decltype(
  delegate_type_impl(
    std::declval<
      // utils::Delayed<
        utils::remove_cvref_t<utils::Delayed<Using_, Args...>> &,
        // utils::remove_cvref_t<Using_> &,
        Args...
      // >
    >()
  )
);

struct access
{
  template<typename Using_>
  static auto delegate(Using_ &&base)
    -> utils::like_t<Using_, DelegateType<Using_>>
  {
    using delegate_t = utils::like_t<Using_, DelegateType<Using_>>;
    return static_cast<delegate_t>(std::forward<Using_>(base));
  }
};

template<typename Using_, typename = DelegateType<Using_>>
auto delegate_if_is_using_impl(Using_ &&base, int)
  -> decltype(access::delegate(std::forward<Using_>(base)))
{
  return access::delegate(std::forward<Using_>(base));
}

template<typename Obj>
auto delegate_if_is_using_impl(Obj &&obj, ...)
  -> decltype(std::forward<Obj>(obj))
{
  return std::forward<Obj>(obj);
}

template<typename Obj>
// decltype(auto) delegate_if_is_using(Obj &&obj)
auto delegate_if_is_using(Obj &&obj)
  -> decltype(delegate_if_is_using_impl(std::forward<Obj>(obj), 0))
{
  // return access::delegate(std::forward<Obj>(obj));
  return delegate_if_is_using_impl(std::forward<Obj>(obj), 0);
}

template<typename Obj, auto x = &Obj::is_smartref>
auto delegate_if_is_using2(Obj &&obj)
  -> decltype(access::delegate(std::forward<Obj>(obj)))
{
  return access::delegate(std::forward<Obj>(obj));
}

template<typename Invoker, typename Using_, typename Args>
auto on_call(Invoker invoker, Using_ &&self, Args && args)
  SFINAEABLE_RETURN(
    call(
      utils::delayed(invoker, utils::type_list<Args>{}),
      utils::delayed(delegate_if_is_using(utils::delayed(std::forward<Using_>(self), utils::type_list<Args>{})), utils::type_list<Args>{}),
      utils::delayed(delegate_if_is_using(utils::delayed(std::forward<Args>(args), utils::type_list<Args>{})), utils::type_list<Args>{})
    )
  )
// {
//   // static_assert(is_same<Args, int>::value);
//   return 3456;
//     // call(
//     //   utils::delayed(invoker, args),
//     //   // utils::delayed(explicitArgs, args...),
//     //   utils::delayed(delegate_if_is_using(utils::delayed(std::forward<Using_>(self), args)), args),
//     //   utils::delayed(delegate_if_is_using(utils::delayed(std::forward<Args>(args), args)), args)
//     // );
// }

} // namespace smartref

template<typename T>
struct Derived : smartref::using_<T, Derived<T>, Derived<T>>
{
  Derived() = default;
  Derived(const Derived &) = default;
  Derived(Derived &&) = default;
  Derived &operator=(const Derived &) = default;
  Derived &operator=(Derived &&) = default;

  // Derived(const T &value) : data{value} {}
  // Derived(char name) : data{}, name{name} {}

private:
  friend class smartref::access;
  constexpr static auto is_smartref = true;

  operator T &() {
    cout << "Requesting reference for " << name << ": " << data << endl;
    return data;
  }

  T data;
  char name;

public:
  using smartref::using_<T, Derived<T>, Derived<T>>::operator=;
};

namespace reflection {

template<class Derived>
class reflector_base
{
private:
  template<typename Self/*, typename D = void*/>
  friend auto derived(Self &&self)
    -> utils::Delayed<utils::like_t<Self, Derived>, Self/*, D*/>
  {
    return static_cast<utils::like_t<Self, Derived>>(std::forward<Self>(self));
  }
};

template<typename Obj, typename CRTP>
using sfinae_please = enable_if_t<
  !is_same<utils::remove_cvref_t<Obj>, CRTP>::value,
  Obj
>;

template<typename CRTP>
class Invoker
{
private:
  template<typename Obj, typename Arg, typename = void>
  friend auto call(const Invoker &, /*utils::type_list<>, */Obj &&obj, Arg &&arg)
    SFINAEABLE_RETURN(std::forward<Obj>(obj) = std::forward<utils::Delayed<Arg, Obj>>(arg))
};

template<typename Arg, typename Ok, typename... Ignore>
using sfinae_please2 = enable_if_t<
  (!is_same<utils::remove_cvref_t<Arg>, Ignore>::value && ...),
  Ok>;

template<typename CRTP, typename... Ignore>
class Exposer : reflector_base<CRTP>
{
public:
  Exposer() = default;
  Exposer(const Exposer &) = default;
  Exposer(Exposer &&) = default;
  Exposer &operator=(const Exposer &) = default;
  Exposer &operator=(Exposer &&) = default;

  template<typename Arg, typename D = float>
  auto operator=(Arg &&arg) &
    SFINAEABLE_RETURN(
      on_call(
        utils::delayed(Invoker<sfinae_please2<Arg, CRTP, Ignore...>>{}, D{}),
        utils::delayed(derived(utils::delayed(*this, utils::type_list<Arg>{})), D{}),
        utils::delayed(std::forward<Arg>(arg), utils::type_list<Arg>{}, D{})
      )
    )
};

} // namespace reflection

struct x : Derived<int> {};

int main()
{
  Derived<int> x;
  Derived<float> y{};

  x = 1;
  y = 2.1;
  y = 3.1;
  y = x;
}
