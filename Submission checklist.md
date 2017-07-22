Submission checklist
====================

-----------------
Ideas:
- While working on an experimental property library (TODO: check what Herb Sutter has to say about the usefulness of properties), I was thinking about how the property-access should be designed. Although assignment to the properties was trivial to implement transparently (i.e. as-if you're assigning to a data-member of the underlying type), simply by overloading the assignment operator, as well as passing the properties to existing code that expect the underlying type, accessing members of the underlying type would then also something that the user would expect from a property.
In a previous project I had already tackled a very similar problem, namely creating a proxy-class wrapping an std::vector. Upon accessing the begin or end member functions, it would lazily load the data from disk, and simply delegate the call to the underlying vector. Using this proxy-class, it then becomes possible to keep a very large data model in memory, that contains many of those proxied vectors, while exposing a very natural interface to the developer (i.e. as-if no proxies were used).
An important detail from this proxy-class, was that it wasn't merely a class dedicated to std::vector. Instead, it was templatized over the _proxied_ type. By conditionally enabling member functions, depending on whether the underlying type supports them, we can add support for a whole range of classes of the STL, such that we can also proxy for example std::map, and std::set

The realization came that it might be possible to lift this functionality out of the proxy-class into a reusable library, instead of duplicating it in the property library.
By putting all of the possible STL member functions in a separate class, we can simply derive from this class in order to inherit all of them. This way, we get support for every possible STL class to be proxied, as well as being able to create properties that behave just like the STL classes themselves.

- As a side effect, I had to implement a little reflection library, which turned out useful on its own, for example to generate serialization code, automatic scripting interface, Delphi/C++ communication.
- 
Conclusion:
- We heavily need compiler support for overloading operator-dot, in whatever form in will arrive (p0416r1/p0252r2, )
