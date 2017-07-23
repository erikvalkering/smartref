Emulating Smart References in C++11/14/17
=========================================

# Introduction

There exist several proposals for adding language support for _smart references_, a long-demanded feature by the C++ community ()

This poster will present the `smartref` library that emulates the operator dot proposal as proposed in [p0352r1 - Smart References through Delegation](https://wg21.link/p0352r1).


# Relevance

> Why the work is interesting, and what problems it addresses.

- No consensus yet in the different proposals (e.g. p0700)
- Availability will depend on standardization process (at least C++20)
- After inclusion into standard, it still requires adoption from the compilers.
- After adoption, it requires developers to upgrade their compiler toolchain, which is not always possible (it might break code).
- The lack of the above, results in people writing their own boilerplate code to support all the delegation, or simply ignore the problem, and go for an inferior solution.

# Discussion

- The core of the `smartref` library is the `using_` class-template, from which _smart reference_-classes can inherit. The template defines a set of member-functions and member-types, corresponding to those defined in the template parameter. These member
Usage:
```c++
// using_ library                               // p0352r1
template<typename T>                            template<typename T>
class proxy : public using_<T>                  class proxy : public using T
{                                               {
public:                                         public:
    operator T &()                                  operator T &()
    {                                               {
        // lazy-load data_                              // lazy-load data_

        return data_;                                   return data_;
    }                                               }
    
private:                                        private:
    T data_;                                        T data_;
};                                              };
```

Limitations:
- Member-variables are currently not supported. I don't currently know of a way to add support for them without breaking the zero-overhead principle. Nevertheless, this might be some future research topic.

# Completion Status

A proof-of-concept library has been developed, demonstrating the feasibility of implementing a smart reference library (based on the intuitive syntax of [p0352r1](https://wg21.link/p0352r1)). `STL` types are supported out of the box, whereas user-defined types require only a minimal amount of annotating using the `REFLECT` macro (either intrusively or non-intrusively).

Currently, this implementation supports the following:
- Delegation to non-const member-functions
- Predefined STL member-functions: `begin()`, `end()`, `push_back()`
- Explicit definition of member-functions for user-defined types
- Non-intrusive discovery of member-functions using the REFLECT macro
- Intrusive discovery of member-functions using the REFLECT_MEMBER macro

The implementation has been tested using the following compilers:
- Clang 4.0.1

What is not yet supported, but is expected to be completed before the poster presentation at CppCon, is:
- Delegation to const member-functions
- Delegation to member-types
- Delegation to member-variables
- Support for the all STL types
- Support for fundamental types (e.g. size_t, bool, double).
- More test coverage
- More examples
- Compiler support: GCC and MSVC

# Supporting Material

A working implementation of the prototype can be found at my github account: <http://github.com/erikvalkering/smartref>

# Contact information

Name: Erik Valkering
email: <eejv@plaxis.com>

# Biography

