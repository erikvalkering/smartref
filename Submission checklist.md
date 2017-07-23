Submission checklist
====================

-----------------
While working on an experimental property library (TODO: check what Herb Sutter has to say about the usefulness of properties), I was thinking about how the property-access should be designed.
Although assignment to the properties was trivial to implement transparently (i.e. as-if you're assigning to a data-member of the underlying type), simply by overloading the assignment operator, as well as passing the properties to existing code that expect the underlying type, accessing members of the underlying type would then also be something that the user would expect from a property.

In a previous project I had already tackled a very similar problem, namely creating a proxy-class wrapping an std::vector. Upon accessing the begin or end member functions, it would lazily load the data from disk, and simply delegate the call to the underlying vector. Using this proxy-class, it then becomes possible to keep a very large data model in memory, that contains many of those proxied vectors, while exposing a very natural interface to the developer (i.e. as-if no proxies were used).
An important detail from this proxy-class, was that it wasn't merely a class dedicated to std::vector. Instead, it was templatized over the _proxied_ type. By conditionally enabling member functions, depending on whether the underlying type supports them, we can add support for a whole range of classes of the STL, such that we can also proxy for example std::map, and std::set

The realization came that it might be possible to lift this functionality out of the proxy-class into a reusable library, instead of duplicating it in the property library.
By putting all of the possible STL member functions in a separate class, we can simply derive from this class in order to inherit all of them. This way, we get support for every possible STL class to be proxied, as well as being able to create properties that behave just like the STL classes themselves.
Additionally, by moving this functionality into a separate library, it would become easier to test the proxy library, as well as the property library, because of the reduced complexity of both of them.
There was one problem, though. While in the proxy-class, the member functions had direct access to the proxied object, moving them into a separate class, more specifically a _base-class_, makes it more difficult to get access to this object. I could of course use a virtual member-function for this, but then I would loose the zero-overhead principle of C++. Fortunately, there is a better solution for this: the _Curiously Recurring Template Pattern_ (CRTP). By passing the derived class as a template parameter to the base class, we can call a function on the derived class from within the base class, that returns a reference to this object. The most obvious function to call, is the conversion operator (TODO: Find correct name).
Having come up with this design, I came to the conclusion that in fact I'd discovered a way to emulate one of the operator-dot proposals (p0352r1). Therefore, I decided to name my library's class, "using_", in order to have a syntax that looks as closely as possible to p0352r1:

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

So the actual difference is **only 12 characters**:
```c++
class proxy : public using_<proxy<T>, T>
class proxy : public using T
```

Now, this class can be used like this:
```c++
proxy<std::vector<int>> v;

for (auto &x : v)
{
    x *= 2;
}
```

However, although exposing all of the STL member functions would be sufficient for the general use case that the proxy class was designed for (i.e. wrapping STL containers), user-defined types would not be supported. The latter is even more important to have support for, for a property library. Therefore, a mechanism had to be designed such that user-defined types can also be supported.

TODO: Also check the commit history
An initial (naive) attempt I tried, was to specialize the using_ class for every user-defined class that was going to be used. However, that would result in a lot of code duplication.

- As a side effect, I had to implement a little reflection library, which turned out useful on its own, for example to generate serialization code, automatic scripting interface, Delphi/C++ communication.
- 
Conclusion:
- We heavily need compiler support for overloading operator-dot, in whatever form in will arrive ([p0352r1](http://wg21.link/p0352r1), [p0416r1](http://wg21.link/p0416r1)/[p0252r2](http://wg21.link/p0252r2), or the even more ambitious )

References
----------
- 2017-02-21 - [p0700r0 - Alternatives to operator dot](http://wg21.link/p0700r0)
- 2017-02-06 - [p0352r1 - Smart References through Delegation (2nd revision)](http://wg21.link/p0352r1)
- 2016-10-16 - [p0416r1 - Operator Dot (R3)](http://wg21.link/p0416r1)
- 2016-10-16 - [p0252r2 - Operator Dot Wording](http://wg21.link/p0252r2)
- 2015-09-18 - [p0060r0 - Function Object-Based Overloading of Operator Dot](http://wg21.link/p0060r0)

- Long discussion about p0700: https://groups.google.com/a/isocpp.org/forum/#!topic/std-proposals/ykmS3pPu91Y
- A bit of background for the operator dot proposal—Bjarne Stroustrup: https://isocpp.org/blog/2016/02/a-bit-of-background-for-the-operator-dot-proposal-bjarne-stroustrup
