# smartref [![Build Status](https://travis-ci.org/erikvalkering/smartref.svg?branch=master)](https://travis-ci.org/erikvalkering/smartref)
A modern header-only zero-overhead library for creating smart references

## Abstract

`smartref` is a *pure-library* solution for creating smart reference classes using C++11/14/17, and comprises two parts: the `using_` class-template, which acts as a *reusable* base-class for smart references, and a tiny reflection facility, which is used for supporting user-defined types. Together, they provide a new building block with which powerful zero-overhead abstractions can be created, today.

## Introduction

Smart references are a fundamental missing building block of the C++ language, and are a long-demanded feature [by the C++ community](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/1990/WG21%201990/X3J16_90%20WG21%20Request%20for%20Consideration%20-%20Overloadable%20Unary%20operator.pdf). They allow for creating objects that act as if they are other objects, like proxies, rebindable references, properties, strong typedefs, etc.

There currently exist several proposals for adding language support for smart references, ([p0416r1](https://wg21.link/p0416r1), [p0060r0](https://wg21.link/p0060r0), [p0352r1](https://wg21.link/p0352r1)). Despite the demand for language support, it does seem there is currently no consensus yet which direction to pursue.

The `smartref` library is an attempt to provide this missing building block, but instead as a pure-library solution. Furthermore, its syntax is based on the smart reference proposal from p0352r1 - "Smart References through Delegation", which arguably leads to a very intuitive way of thinking about how to design smart references.

## Overview

The core of the `smartref` library is the `using_` class-template, from which _smart reference_-classes can derive.

##### The `using_` class-template:
* Parameterizes over the *delegate type*.
* Obtains the *delegate object* from the derived class.
* Defines members, corresponding to those found in the delegate type.
* Member-*functions* forward the call to the delegate object.
* Member-*types* are aliases for the types defined in the delegate type.

##### The smart reference class:
* Defines a conversion function which returns the delegate object.
* Derives from the `using_` class, and thereby inherits the interface from the delegate type.

## Example: a proxy class for on-demand loading from disk

For example, it becomes reasonably easy to implement a `proxy` class, for on-demand loading from disk of some wrapped container class (e.g. `std::vector`), while providing exactly the same interface as the wrapped class:

```c++
template<typename T>
class proxy : public using_<T>
{
    T data_;

public:
    operator T &()
    {
        // ...lazy-load data_
        return data_;
    }
};
```

Now, `proxy` provides exactly the same interface as the wrapped `std::vector<double>`, while every function call is 'intercepted' by the conversion function.

It can be used as follows:
```c++
proxy<vector<double>> v = read_from_file(...);

v.push_back(3.141592654);

for (auto &x : v)
{
    x *= 2;
}
```

## Batteries included

Out of the box, the `using_` class-template defines member-functions and member-types corresponding to all those found in the data types defined by the STL. More importantly, this support is generic: any type satisfying (part of) the interface of an STL type, is also supported out of the box.

## User-defined types

In order to support user-defined types, their members need to be explicitly registered.

For this, the `smartref` library comes with a tiny reflection facility, which provides a non-intrusive `REFLECTABLE` macro. By annotating the name of a member using this macro, this member will be picked up automatically by the `using_` class-template:

```c++
class Person
{
public:
    auto first_name() {...}
    auto last_name() {...}
};

REFLECTABLE(first_name);
REFLECTABLE(last_name);
```

which could be used as follows:
```c++
template<typename T>
auto greet(const T &person)
{
    cout << "Hi, " << person.first_name() + " " + person.last_name() << endl;
}

auto real_person  = Person{...};
auto proxy_person = proxy<Person>{...};

greet(real_person);
greet(proxy_person);
```

## Completion Status

The implementation has been tested using the following compilers:
- Clang 4.0.1

## Features
- [x] Zero-overhead
- [x] Header-only
- [x] Non-intrusive support for user-defined types
- [x] Generic support
- [x] Support for typedefs / nested classes
- [x] Support for member-functions
  - [x] Non-templates
  - [x] Overloads
  - [x] Templates
    - [x] Deducible
    - [x] Non-deducible
  - [x] Operators
    - [x] Unary
    - [x] Binary
    - [x] Assignment
  - [x] Qualifiers
    - [x] Non-const member-functions
    - [ ] const member-functions
    - [ ] rvalue member-functions
    - [ ] const rvalue member-functions
- [ ] Support for data members

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
