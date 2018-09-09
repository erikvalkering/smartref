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

auto wand = [](auto x) { return UFCS<utils::remove_cvref_t<decltype(x)>>{x}; };

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
REFLECTABLE_NAMESPACE(algorithms1);
REFLECTABLE_NAMESPACE(algorithms2);

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

// Functors
auto is_even = [](auto x) { return x % 2 == 0; };
auto squared = [](auto x) { return x * x;      };

auto $ = magic::wand;

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
