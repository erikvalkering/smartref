#include "stdlibs_for_debugging.h"

#include "foobar.h"

// TODO (unit test):
// - reflection.h, REFLECTABLE(function), namespace::ClassTemplate, namespace::function(ClassTemplate), SmartRef
// - also make sure that no invalid free functions are added (e.g. push_back on a vector)

#include <smartref/smartref.h>

#include <iostream>
#include <vector>
#include <typeinfo>
#include <iterator>

using namespace std;
using smartref::using_;

template<typename T>
class Property : public using_<T>
{
public:
  Property() = default;
  Property(T value) : data{value} {}

  using using_<T>::operator=;

private:
  friend class smartref::access;

  operator T &() &
  {
    return data;
  }

  operator T &&() &&
  {
    return move(data);
  }

  operator const T &() const &
  {
    return data;
  }

  operator const T &&() const &&
  {
    return move(data);
  }

public:
  T data;
};

namespace foobar {

void asdf(Foo)              { cout << "asdf(Foo)"              << endl; }
void asdf(Derived)          { cout << "asdf(Derived)"          << endl; }

template<typename T>
void asdf(ClassTemplate<T>) { cout << "asdf(ClassTemplate<T>)" << endl; }

} // namespace foobar

namespace foobar2 {

struct A {};
struct B {};

template<typename>
void qwerty(A)
{
  cout << "foobar2::qwerty<T>(A)" << endl;
}

template<typename>
void qwerty(foobar::ClassTemplate<A>)
{
  cout << "foobar2::qwerty<T>(foobar::ClassTemplate<A>)" << endl;
}

template<typename, typename U>
void qwerty(foobar::ClassTemplate<U>)
{
  cout << "foobar2::qwerty<T>(foobar::ClassTemplate<U>)" << endl;
}

} // namespace foobar2

using namespace foobar;

int main()
{
  cout << "Hello, Wandbox!" << endl;

  Property<vector<int>> v;

  v.push_back(1);
  v.push_back(2);
  v.push_back(3);

  // The following statements should *not* compile
  // v.push_back(1, 2, 3);
  // push_back(v, 1);

  for (auto x : v)
  {
    cout << x << endl;
  }

  // This is a great example of a fully-encapsulated smart reference:
  // back_inserter requires:
  // - push_back() member function
  // - value_type member type
  //
  // Furthermore, because of the full encapsulation of Property<T>,
  // begin(v) returns a Property<vector<int>::iterator_type>
  // Which in turn requires the following:
  // - operator* for dereferencing
  // - operator++ for incrementing
  // - operator!= for comparing begin() and end()
  //
  // And after being referenced, it returns again a Property<vector<int>::iterator &>
  // which requires:
  // - operator= for assigning the value (via push_back())
  Property<vector<float>> v2;
  cout << "type of begin(v): " << typeid(begin(v)).name() << endl;
  copy(begin(v), end(v), back_inserter(v2));
  copy(begin(v2), end(v2), ostream_iterator<double>(cout, " "));
  cout << endl;

  Property<int> x{};
  Property<int> y{};

  x = 5;

  cout << "x.data:  " << x.data  << " [5]" << endl;
  cout << "x:       " << x       << " [5]" << endl;
  cout << "(x = 9): " << (x = 9) << " [9]" << endl;

  // TODO: Proper reference leaking control
  //       i.e. {x + y} -> Property<decltype(delegate(x)+delegate(y))>
  {
    auto u = x + 1;
    auto v = 1 + y;
    auto w = x + y;
    auto z = Property<int>{x + y};

    assert(u == 10);
    cout << "u: " << u << " [10]" << endl;
    cout << "typeid(u).name(): " << typeid(u).name() << endl;

    assert(v == 1);
    cout << "v: " << v << " [1]" << endl;
    cout << "typeid(v).name(): " << typeid(v).name() << endl;

    assert(w == 9);
    cout << "w: " << w << " [9]" << endl;
    cout << "typeid(w).name(): " << typeid(w).name() << endl;

    assert(z == 9);
    cout << "z: " << z << " [9]" << endl;
    cout << "typeid(z).name(): " << typeid(z).name() << endl;
  }

  {
    Property<int>   x{};
    Property<float> y{};

    x = 1;
    y = 2.1;
    y = 3.1;
    y = x;
  }

  Property<Foo> foo;
  foo.foo();

  Property<Bar> bar;
  bar.bar();
  bar.bar2();
  bar.bar3();

  Property<Baz> baz;
  baz.baz();
  baz.baz2();

  Property<Bat> bat;
  bat.bat();
  bat.bat2();

  {
    Bar bar;
    bar.bar();
    bar.bar2();
    bar.bar3();

    Baz baz;
    baz.baz();
    baz.baz2();

    Bat bat;
    bat.bat();
    bat.bat2();
  }

  {
    Property<Bla> bla;
    bla.foo();
    bla.bar();
    auto x = decltype(bla)::baz{1234};
    cout << typeid(x).name() << " " << x << endl;
    auto y = decltype(bla)::bla{};
    y.foo();
    y.bar();
  }

  {
    Property<Overloads> o;
    o.foo();
    o.foo(0);
    o.bar<int>();
  }

  {
    Property<GenericClassA> a;
    a.foobar();
    a.foobar(1);
    a.foobar(1.0);
    static_assert(is_same<decltype(a)::some_type, GenericClassA::some_type>::value);

    Property<GenericClassB> b;
    b.foobar();
    b.foobar(1);
    static_assert(is_same<decltype(b)::some_type, GenericClassB::some_type>::value);

    Property<ClassTemplate<int>> c;
    c.foobarbaz();
    static_assert(is_same<decltype(c)::some_foo_type, ClassTemplate<int>::some_foo_type>::value);

    Property<ClassTemplate<float>> d;
    d.foobarbaz();
    static_assert(is_same<decltype(d)::some_foo_type, ClassTemplate<float>::some_foo_type>::value);
  }

  {
    Property<ConstClass> non_const_obj;
    const Property<ConstClass> const_obj;

    non_const_obj.foo();  // Should compile
    non_const_obj.bar();  // Should compile
    const_obj.foo();      // Should compile
    // const_obj.bar();      // Should not compile
  }

  {
    Property<RefClass> obj;
    const Property<RefClass> cobj;

    obj.foo(); // "RefClass::foo() &"
    move(obj).foo(); // "RefClass::foo() &&"
    cobj.foo(); // "RefClass::foo() const &"
    move(cobj).foo(); // "RefClass::foo() const &&"
  }

  {
    Property<Foo> foo;
    Property<Derived> derived;
    Property<MoreDerived> moreDerived;
    Property<ClassTemplate<int>> tmpl;

    asdf(foo);          // "asdf(Foo)"
    asdf(derived);      // "asdf(Derived)"
    asdf(moreDerived);  // "asdf(Derived)"
    asdf(tmpl);         // "asdf(ClassTemplate<T>)"
  }

  {
                           foobar2::A   tmpl1;
             ClassTemplate<foobar2::A > tmpl2;
             ClassTemplate<foobar2::B > tmpl3;
    Property<              foobar2::A > tmpl4;
    Property<ClassTemplate<foobar2::A>> tmpl5;
    Property<ClassTemplate<foobar2::B>> tmpl6;

    using reflectable::qwerty;
    qwerty<int>(tmpl1);
    qwerty<int>(tmpl2);
    qwerty<int>(tmpl3);
    qwerty<int>(tmpl4);
    qwerty<int>(tmpl5);
    qwerty<int>(tmpl6);
  }
}
