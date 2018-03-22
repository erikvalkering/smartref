# smartref [![Build Status](https://travis-ci.org/erikvalkering/smartref.svg?branch=master)](https://travis-ci.org/erikvalkering/smartref)
Emulating Smart References in C++11/14/17

## Build instructions
Execute the following commands:
```bash
> mkdir build
> cd build
> cmake ..
> cmake --build .
```

this will create several test executables, which can be invoked using:
```bash
> ctest
```

# Introduction

There exist several proposals for adding language support for _smart references_, a long-demanded feature by the C++ community. In [p0416r1](https://wg21.link/p0416r1), a design is proposed by introducing an overloadable `operator.()` to the language, whereas in [p0060r0](https://wg21.link/p0060r0), a more sophisticated design was proposed, which involves having the compiler synthesize a function object, that is passed to an overloadable `operator.()`. Finally, [p0352r1](https://wg21.link/p0352r1) proposes an entirely different approach, by using an inheritance-like mechanism combined with a conversion function.

The `smartref` library emulates the smart reference proposal as proposed in [p0352r1 - Smart References through Delegation](https://wg21.link/p0352r1), by clever use of modern C++11/14/17.

# Background

Although there is demand for language support for _smart references_, it seems there is currently no consensus yet which direction to pursue. Even if it will eventually be adopted into the standard (C++20/23/26?), it might still take a while for the compiler vendors to catch up. And even then, it is sometimes not possible or difficult for software companies to upgrade their compiler toolchain to benefit from these new features.

Due to this slow adoption, developers might write their own boilerplate code to support all the delegation, or simply ignore the problem, and go for an inferior solution and sacrifice generic programming.

By factoring out the "smart reference" problem into a reusable library, we can provide a building block onto which people can start developing new abstractions, like proxies, rebindable references, properties, light-weight pimpl. Furthermore, by providing it in the form of a library, we can use it today!

# High level overview of the library

The core of the `smartref` library is the `using_` class-template, from which _smart reference_-classes can inherit. This template is parameterized over the _referred-to_ type, and defines a set of member-functions and member-types (unfortunately no member-variables, see the [Limitations](#limitations) section below), corresponding to those defined in that type. The member-types are simply aliases for the types defined in the referred-to type. The member-functions use the conversion function defined in the smart reference class (i.e. the class inheriting from the `using_` class), and delegate the function call to the referred-to object, which is returned by the conversion function.

For example, it becomes reasonably easy to implement a `proxy` class, for on-demand loading from disk of some wrapped container class (e.g. std::vector), while providing exactly the same interface as the wrapped class:
```c++
// smartref library                             // p0352r1
template<typename T>                            template<typename T>
class proxy : public using_<T>                  class proxy : public using T
{                                               {
public:                                         public:
    operator T &()                                  operator T &()
    {                                               {
        // ...lazy-load data_                           // ...lazy-load data_
        return data_;                                   return data_;
    }                                               }
    
    // ...other members                             // ...other members
private:                                        private:
    T data_;                                        T data_;
};                                              };
```

Now, this class can be used as follows:
```c++
proxy<vector<double>> v = read_from_file(...);

for (auto &x : v)
{
    x *= 2;
}
```

Out of the box, the `using_` class-template defines member-functions and member-types corresponding to all those found in the data types defined by the `STL`. In order to support user-defined types, their member-functions and member-types need to be _registered_. For this, the `smartref` library comes with a tiny reflection utility, which provides an intrusive as well as a non-intrusive `REFLECTABLE` macro, to annotate the classes.

For example, by wrapping the name of a member-function inside this macro, this member-function can be picked-up automatically by the `using_` class-template.

```c++
struct Foo
{
    int REFLECTABLE(bar)(bool a, unique_ptr<double> b) {...}
};

proxy<Foo> foo = ...;
auto x = foo.bar(true, make_unique<double>(3.141592654));
```

Non-intrusive reflection is also possible using the `REFLECTABLE` macro, for example to support 3rd-party data-types:

```c++
struct Bar
{
    template<typename T>
    bool baz(T x) {...}
};

REFLECTABLE(Bar, baz);

proxy<Bar> bar = ...;
if (bar.baz(foo)) ...
```

Generic reflection is also possible, by specifying `auto` as the class in the `REFLECTABLE` macro:

```c++
template<typename T>
struct Baz
{
    void foobar(T x) {...}
};

REFLECTABLE(auto, foobar);

proxy<Baz<int>> baz = ...;
baz.foobar(42);
```

> ##### Limitations
> - Member-variables are currently not supported. ~At the moment, I don't know of a way to add support for them without breaking the zero-overhead principle.~ Nevertheless, this might be some future research topic.

# Completion Status

The implementation has been tested using the following compilers:
- Clang 4.0.1

# Core features
- [x] Zero-overhead
- [x] All features are supported generically
- [x] Member support
  - [x] Types
    - [x] Member-types / nested classes
    - [ ] Alias templates / nested class templates
  - [x] Functions
    - [x] Non-templates
    - [x] Overloads
    - [x] Templates
      - [x] Deducible
      - [x] Non-deducible
      - [ ] Non-type templates
      - [ ] Mixed type and non-type templates
    - [x] Operators
      - [x] Unary
      - [x] Binary
      - [x] Assignment
    - [ ] Constructors
    - [x] Qualifiers
      - [x] Non-const member-functions
      - [ ] const member-functions
      - [ ] rvalue member-functions
      - [ ] const rvalue member-functions
  - [ ] Data
    - [ ] Basic support for data members
    - [ ] Opt-in zero-overhead declaration
- [ ] Free function support
  - [ ] Operators (non-members)
- [x] Reflection
  - [x] Class specific reflection
  - [x] Generic reflection
- [ ] Various
  - [ ] Conflict-resolution
  - [x] Reference-leaking control (e.g. operator=() currently returns int & for int-wrappers), and maybe a more general control mechanism

# The following features are planned to be implemented:
- [ ] Member-types
    - [ ] STL
        - [x] All of `std::vector`
        - [ ] Full support of the STL
    - [x] User-defined types
        - [x] Non-intrusive discovery of member-functions using the `REFLECTABLE` macro
- [ ] Member-fields
    - [ ] STL
        - [ ] Proof-of-concept member-fields: `first`, `second`
        - [ ] Full support of STL
    - [ ] User-defined types
        - [ ] Explicit definition of member-fields
        - [ ] Non-intrusive discovery of member-fields using the `REFLECTABLE` macro
- [ ] More test coverage
    - [ ] Implicit conversions while passing to function
    - [ ] Performance tests
        - [ ] Compile-time overhead
        - [ ] Runtime overhead
        - [ ] Memory overhead
- [ ] More examples
    - [ ] Proxy
    - [ ] Poly (value-based ClonablePtr)
    - [ ] Strong typedef
    - [ ] Perfect pimpl
    - [ ] Observable
    - [ ] Uniform call syntax
- [ ] Documentation
    - [ ] User
    - [ ] Source code
- [ ] Compiler support
    - [x] Clang (4.0.1)
    - [ ] GCC
    - [ ] MSVC
    - [ ] Intel
