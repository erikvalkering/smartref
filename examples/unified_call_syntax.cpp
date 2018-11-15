/*********************************************/
/* Example: unified call syntax              */
/*********************************************/
#include <vector>
#include <iostream>

#define SMARTREF_NO_DEFAULT_SUPPORT
#include <smartref/smartref.h>

using namespace std;

namespace magic {

template<typename T>
class UFCS : public smartref::using_<T, UFCS<T>>
{
public:
  explicit UFCS(T data) : data{data} {}

  using smartref::using_<T, UFCS<T>>::operator=;

private:
  friend class smartref::access;

  operator T&() &{ return data; }
  operator T&&() &&{ return std::move(data); }
  operator const T &() &{ return data; }
  operator const T &&() &&{ return std::move(data); }

  T data;
};

auto wand = [](auto &&x) -> decltype(auto) {
  if constexpr (is_lvalue_reference<decltype(x)>::value)
  {
    return x; // TODO: not supported yet. Requires full reference-leaking support.
  }
  else
  {
    return UFCS<decltype(x) &&>{std::forward<decltype(x)>(x)};
  }
};

template<typename T, typename Invoker, typename... Hierarchy, typename ExplicitArgs, typename Using_, typename... Args>
auto on_call(UFCS<T> *, const Invoker &invoker, utils::type_list<Hierarchy...>, ExplicitArgs explicitArgs, Using_ &&self, Args &&... args)
  SFINAEABLE_RETURN(
    wand(
      call(
        invoker.as_nonmember(),
        explicitArgs,
        smartref::enable_adl(smartref::delegate_if_is_using<Hierarchy...>(std::forward<Using_>(self))),
        smartref::delegate_if_is_using<Hierarchy...>(std::forward<Args>(args))...
      )
    )
  )

} // namespace magic

REFLECTABLE_OPERATOR_INFIX(<<);

REFLECTABLE(sorted);
REFLECTABLE(unique);
REFLECTABLE(filtered);
REFLECTABLE(transform);
REFLECTABLE(sum);
REFLECTABLE(value_category);
REFLECTABLE_NAMESPACE(algorithms1);
REFLECTABLE_NAMESPACE(algorithms2);
REFLECTABLE_NAMESPACE(algorithms3);

// Algorithms

namespace algorithms1 {
auto sorted(std::vector<int> container) { return container; }
auto unique(std::vector<int> container) { return container; }
} // namespace algorithms1

namespace algorithms2 {
template<typename Predicate>
auto filtered(std::vector<int> container, Predicate predicate)   { return container; }
template<typename Operation>
auto transform(std::vector<int> container, Operation operation)  { return container; }
auto sum(std::vector<int> container)                             { return 42;        }
} // namespace algorithms2

namespace algorithms3 {
struct lvalue_ref {};
struct lvalue_ref_const {};
struct rvalue_ref {};
struct rvalue_ref_const {};
struct xvalue {};
struct xvalue_const {};

template<typename T>       double   value_category(T container, xvalue)           { return 42.0; }
template<typename T> const double   value_category(T container, xvalue_const)     { return 42.0; }
template<typename T>       double & value_category(T container, lvalue_ref)       { static double x = 42; return x; }
template<typename T> const double & value_category(T container, lvalue_ref_const) { static double x = 42; return x; }
template<typename T>       double &&value_category(T container, rvalue_ref)       { return std::move(42.0); }
template<typename T> const double &&value_category(T container, rvalue_ref_const) { return std::move(42.0); }

} // namespace algorithms3

// Functors
auto is_even = [](auto x) { return x % 2 == 0; };
auto squared = [](auto x) { return x * x;      };

auto $ = magic::wand;
auto test_value_categories() {
  using namespace algorithms3;

  {
          double   a1 = value_category(1234, xvalue{});
    const double   a2 = value_category(1234, xvalue{});
          // double & a3 = value_category(1234, xvalue{}); // <- should not compile
    const double & a4 = value_category(1234, xvalue{});
          double &&a5 = value_category(1234, xvalue{});
    const double &&a6 = value_category(1234, xvalue{});
  }

  {
          double   a1 = value_category(1234, xvalue_const{});
    const double   a2 = value_category(1234, xvalue_const{});
          // double & a3 = value_category(1234, xvalue_const{}); // <- should not compile
    const double & a4 = value_category(1234, xvalue_const{});
          double &&a5 = value_category(1234, xvalue_const{});
    const double &&a6 = value_category(1234, xvalue_const{});
  }

  {
          double   a1 = value_category(1234, lvalue_ref{});
    const double   a2 = value_category(1234, lvalue_ref{});
          double & a3 = value_category(1234, lvalue_ref{});
    const double & a4 = value_category(1234, lvalue_ref{});
          // double &&a5 = value_category(1234, lvalue_ref{}); // <- should not compile
    // const double &&a6 = value_category(1234, lvalue_ref{}); // <- should not compile
  }

  {
          double   a1 = value_category(1234, lvalue_ref_const{});
    const double   a2 = value_category(1234, lvalue_ref_const{});
          // double & a3 = value_category(1234, lvalue_ref_const{}); // <- should not compile
    const double & a4 = value_category(1234, lvalue_ref_const{});
          // double &&a5 = value_category(1234, lvalue_ref_const{}); // <- should not compile
    // const double &&a6 = value_category(1234, lvalue_ref_const{}); // <- should not compile
  }

  {
          double   a1 = value_category(1234, rvalue_ref{});
    const double   a2 = value_category(1234, rvalue_ref{});
          // double & a3 = value_category(1234, rvalue_ref{}); // <- should not compile
    const double & a4 = value_category(1234, rvalue_ref{});
          double &&a5 = value_category(1234, rvalue_ref{});
    const double &&a6 = value_category(1234, rvalue_ref{});
  }

  {
          double   a1 = value_category(1234, rvalue_ref_const{});
    const double   a2 = value_category(1234, rvalue_ref_const{});
          // double & a3 = value_category(1234, rvalue_ref_const{}); // <- should not compile
    const double & a4 = value_category(1234, rvalue_ref_const{});
          // double &&a5 = value_category(1234, rvalue_ref_const{}); // <- should not compile
    const double &&a6 = value_category(1234, rvalue_ref_const{});
  }
}

auto test_value_categories_non_wrapped_deduced() {
  using namespace algorithms3;

  {
          auto   a1 = value_category(1234, xvalue{});
    const auto   a2 = value_category(1234, xvalue{});
          // auto & a3 = value_category(1234, xvalue{}); // <- should not compile
    const auto & a4 = value_category(1234, xvalue{});
          auto &&a5 = value_category(1234, xvalue{});
    const auto &&a6 = value_category(1234, xvalue{});
  }

  {
          auto   a1 = value_category(1234, xvalue_const{});
    const auto   a2 = value_category(1234, xvalue_const{});
          // auto & a3 = value_category(1234, xvalue_const{}); // <- should not compile
    const auto & a4 = value_category(1234, xvalue_const{});
          auto &&a5 = value_category(1234, xvalue_const{});
    const auto &&a6 = value_category(1234, xvalue_const{});
  }

  {
          auto   a1 = value_category(1234, lvalue_ref{});
    const auto   a2 = value_category(1234, lvalue_ref{});
          auto & a3 = value_category(1234, lvalue_ref{});
    const auto & a4 = value_category(1234, lvalue_ref{});
          auto &&a5 = value_category(1234, lvalue_ref{});
    // const auto &&a6 = value_category(1234, lvalue_ref{}); // <- should not compile
  }

  {
          auto   a1 = value_category(1234, lvalue_ref_const{});
    const auto   a2 = value_category(1234, lvalue_ref_const{});
          auto & a3 = value_category(1234, lvalue_ref_const{});
    const auto & a4 = value_category(1234, lvalue_ref_const{});
          auto &&a5 = value_category(1234, lvalue_ref_const{});
    // const auto &&a6 = value_category(1234, lvalue_ref_const{}); // <- should not compile
  }

  {
          auto   a1 = value_category(1234, rvalue_ref{});
    const auto   a2 = value_category(1234, rvalue_ref{});
          // auto & a3 = value_category(1234, rvalue_ref{}); // <- should not compile
    const auto & a4 = value_category(1234, rvalue_ref{});
          auto &&a5 = value_category(1234, rvalue_ref{});
    const auto &&a6 = value_category(1234, rvalue_ref{});
  }

  {
          auto   a1 = value_category(1234, rvalue_ref_const{});
    const auto   a2 = value_category(1234, rvalue_ref_const{});
          auto & a3 = value_category(1234, rvalue_ref_const{});
    const auto & a4 = value_category(1234, rvalue_ref_const{});
          auto &&a5 = value_category(1234, rvalue_ref_const{});
    const auto &&a6 = value_category(1234, rvalue_ref_const{});
  }
}

int main()
{
  auto v = vector<int>{1, 2, 2, 1, 3, 2, 4, 5};
  // auto v = vector{1, 2, 2, 1, 3, 2, 4, 5};

  {
    using namespace algorithms1;
    using namespace algorithms2;

    // Variant 1a: single-line, read from inside outwards (continuously looking left *and* right)
    auto s = sum(transform(filtered(unique(sorted(v)), is_even), squared));
    cout << "Variant 1a: " << s << endl;
  }

  {
    using namespace algorithms1;
    using namespace algorithms2;

    // Variant 1b: multi-line, read from inside outwards (continuously looking left *and* right)
    auto s = sum(
               transform(
                 filtered(
                   unique(sorted(v)),
                   is_even
                 ),
                 squared
               )
             );

    cout << "Variant 1b: " << s << endl;
  }

  {
    // Variant 2a: single-line, read from left to right
    auto s = $(v).sorted().unique().filtered(is_even).transform(squared).sum();
    cout << "Variant 2a: " << s << endl;
  }

  {
    // Variant 2b: multi-line, read from top to bottom
    auto s = $(v).sorted()
                 .unique()
                 .filtered(is_even)
                 .transform(squared)
                 .sum();

    cout << "Variant 2b: " << s << endl;
  }
}
