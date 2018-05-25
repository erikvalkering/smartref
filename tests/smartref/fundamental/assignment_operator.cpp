#include <reflection/reflection.h>
#include <smartref/smartref.h>

#include <cassert>
#include <string>

namespace tests_assignment {

template<typename T>
struct Ref : smartref::using_<T>
{
  T ref;

  static auto &counter()
  {
    static auto count = 0;
    return count;
  }

  operator T &() &
  {
    ++counter();
    return ref;
  }

  operator T &&() &&
  {
    ++counter();
    return std::move(ref);
  }

  operator const T &() const &
  {
    ++counter();
    return ref;
  }

  operator const T &&() const &&
  {
    ++counter();
    return std::move(ref);
  }

  Ref(T arg) : ref{arg} {}

  using smartref::using_<T>::operator=;

  Ref() = default;

  Ref(const Ref &) = default;
  Ref &operator=(const Ref &) = default;
  Ref(Ref &&) = default;
  Ref &operator=(Ref &&) = default;
};

template<typename T, typename Delegate>
auto test = []{
  //! Uninitialized construction
  T a;

  //! Default construction
  T b{};
  T c = {};

  //! Copy construction
  T d{a};
  T e = a;
  T f = {a};
  auto g{a};
  auto h = a;
  auto i = {a};

  //! Move construction
  T j{std::move(a)};
  T k = std::move(a);
  T l = {std::move(a)};
  auto m{T{}};
  auto n = T{};
  auto o = {T{}};

  //! Delegate type copy construction
  auto delegate = Delegate{};
  T p{delegate};
  T q = delegate;
  T r = {delegate};
  auto s{T{delegate}};
  auto t = T{delegate};
  auto u = {T{delegate}};

  //! Delegate type move construction
  T v{std::move(delegate)};
  T w = std::move(delegate);
  T x = {std::move(delegate)};
  auto y{T{std::move(delegate)}};
  auto z = T{std::move(delegate)};
  auto A = {T{std::move(delegate)}};

  // TODO: Check whether semantics are right (e.g. (a = b)++ changes a, instead of a copy).
  //! Copy assignment
  a = b;
  a = b = c;
  a = (b = c);
  (a = b) = c;
  (a = b) = delegate; // TODO: also add similar checks for other tests.

  //! Move assignment
  a = std::move(b);
  a = b = std::move(c);
  a = (b = std::move(c));
  (a = b) = std::move(c);

  //! Delegate type copy assignment
  a = delegate;
  a = b = delegate;
  a = (b = delegate);
  (a = b) = delegate;

  //! Delegate type move assignment
  a = std::move(delegate);
  a = b = std::move(delegate);
  a = (b = std::move(delegate));
  (a = b) = std::move(delegate);

  return 0;
};

auto test_int = test<int, int>();

template<typename T>
void test_ref()
{
  Ref<T>::counter() = 0;
  test<Ref<T>, T>();
  printf("counter: %d\n", Ref<T>::counter());
  assert(Ref<T>::counter() == 34);
}

auto tests = []{
  test_ref<int>();
  test_ref<float>();
  test_ref<bool>();
  test_ref<std::string>();

  struct Foo {};
  test_ref<Foo>();

  return 0;
}();

} // namespace tests_assignment
