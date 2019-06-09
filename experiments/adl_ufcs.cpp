#include <iostream>

using std::cout;
using std::endl;

namespace smartref {

template<size_t>
struct adl_tag;

template<size_t n>
using adl_tag_t = typename adl_tag<n>::type;

template<typename>
constexpr auto adl_tag_id = 0;

template<typename T>
class Ref
{
public:
  explicit Ref(T) {}

  auto d() { return 0; }
};

template<typename T, typename Tag>
struct adl_ref
{
  explicit adl_ref(T) {}
  operator T() { return {}; }
};

template<typename T>
auto foo2(Ref<T> x) {
  return foo(adl_ref<T, adl_tag_t<adl_tag_id<T>>>{x.d()});
}

} // namespace smartref

namespace algorithms {
  struct tag {};
}

template<>
struct smartref::adl_tag<0>
{
  using type = algorithms::tag;
};

namespace algorithms {

auto foo(int) { return 42; }

}

int main()
{
  cout << foo2(smartref::Ref{0}) << endl;
}
