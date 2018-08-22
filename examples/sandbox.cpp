#include "stdlibs_for_debugging.h"

#include "foobar.h"

// TODO (unit test):
// - reflection.h, REFLECTABLE(function), namespace::ClassTemplate, namespace::function(ClassTemplate), SmartRef
// - also make sure that no invalid free functions are added (e.g. push_back on a vector)

#include <smartref/smartref.h>

#include <iostream>
#include <vector>
#include <typeinfo>

using namespace std;
using namespace foobar;
using smartref::using_;

template<typename T>
class Property : public using_<T>
{
// TODO: Add proper reference-leaking control by allowing
//       the user-defined conversion functions to be private.
public:
  using using_<T>::operator=;

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

/*
class JSONValue : public using_<JSONValue, double>,
          public using_<JSONValue, string>,
          public using_<JSONValue, vector<JSONValue>>,
          public using_<JSONValue, map<JSONValue, JSONValue>>
{
public:
  operator double &()
  {
    return get<double>(data);
  }

  operator double &()
  {
    return get<string>(data);
  }

  operator double &()
  {
    return get<vector<JSONValue>>(data);
  }

  operator double &()
  {
    return get<map<JSONValue, JSONValue>>(data);
  }

private:
  variant<
    double,
    string,
    vector<JSONValue>,
    map<JSONValue, JSONValue>
  > data;
};
*/

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

// TODO: Move this to fundamental operator support of library
REFLECTABLE_OPERATOR_INFIX(+);

int main()
{
/*
  JSONValue json = {};

  json["asdf"] = 1.0;
  json[123456] = nullptr;
  json.DOT(qwerty) = "the other (third) operator dot proposal";
  json.qwerty = "__getattr__ for C++";
*/

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

  Property<int> x{};
  Property<int> y{};

  x = 5;

  cout << x.data << endl;
  cout << (x = 9) << endl;

  // TODO: Proper reference leaking control
  //       i.e. {x + y} -> Property<decltype(delegate(x)+delegate(y))>
  {
    auto u = x + 1;
    auto v = 1 + y;
    auto w = x + y;

    cout << u << endl;
    cout << typeid(u).name() << endl;
    cout << v << endl;
    cout << typeid(v).name() << endl;
    cout << w << endl;
    cout << typeid(w).name() << endl;
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
