Emulating Smart References in C++11/14/17
=========================================

# Introduction

There exist several proposals for adding language support for 
This poster will present a library that emulates the operator dot proposal as proposed in [p0352r1 - Smart References through Delegation](https://wg21.link/p0352r1).


# Relevance

> Why the work is interesting, and what problems it addresses.

- No consensus yet in the different proposals (e.g. p0700)
- Availability will depend on standardization process (at least C++20)
- After inclusion into standard, it still requires adoption from the compilers.
- After adoption, it requires developers to upgrade their compiler toolchain, which is not always possible (it might break code).
- The lack of the above, results in people writing their own boilerplate code to support all the delegation, or simply ignore the problem, and go for an inferior solution.
- 
# Discussion

Usage:
```c++
// using_ library                               // p0352r1
template<typename T>                            template<typename T>
class proxy : public using_<T, proxy<T>>        class proxy : public using T
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

# Completion Status

A proof-of-concept has been developed, demonstrating the feasibility of the approach of creating a very natural syntax. Currently, this implementation supports the following:
- Delegation of member-functions
- Predefined STL member-functions: `begin()`, `end()`, `push_back()`
- Explicit definition of member-functions for user-defined types
- Non-intrusive discovery of member-functions using the REFLECT macro
- Intrusive discovery of member-functions using the REFLECT_MEMBER macro

The implementation has been tested using the following compilers:
- Clang 4.0.1
- 
What is not yet supported, but is expected to be completed before the poster presentation at CppCon, is:
- Delegation of member-types
- Delegation of member-fields
- Full support for the STL

# Supporting Material

> If applicable, results and references to work, for example, GitHub.

# Contact information

# Biography

> brief biography.
