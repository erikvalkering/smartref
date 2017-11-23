#pragma once

#include "explicit.h"
#include "implicit.h"
#include "stl.h"

#include <utils/utils.h>

namespace smartref {

template<typename Delegate, class Derived>
struct using_base;
// {
//     operator Delegate &()
//     {
//         auto &derived = static_cast<Derived &>(*this);
//         return static_cast<Delegate &>(derived);
//     }
// };

// template<typename Delegate>
template<>
struct using_base<int, void>
{
    // TODO: Add some checks to see whether this doesn't add too much overhead
    //       for this specific Delegate type.
    virtual operator int &() = 0;
};

// struct ASDF {
    template <typename Derived>
    class reflect_member_function
     /*: public reflection::reflect_base<reflection::reflected_kind::member_function> */{
    private:
        template <typename Arg>
        // decltype(auto) indirect(Arg&& arg)
        // decltype(auto) indirect(Arg arg)
        // auto indirect(Arg arg)
        auto indirect(Arg arg)
        {
            //! Downcast to the derived class
            auto &derived = static_cast<utils::Delayed<Derived, Arg> &>(*this);

            // //! Now invoke the conversion operator
            // auto &delegate = static_cast<int &>(derived);

            // return delegate = arg;

            // // auto f = [](auto&& obj, auto&& arg2) -> decltype(auto)
            // auto f = [](auto &obj, auto arg2) -> decltype(auto)
            // {
            //     // return 0;
            //     return obj = arg2;
            //     // return obj = std::forward<Arg>(arg);
            //     // if constexpr (sizeof...(ExplicitArgs) == 0)
            //     //     return obj.operator=(std::forward<Args>(args)...);
            //     // else if constexpr (utils::always_true<Args...>)
            //     //     return obj.template operator=<ExplicitArgs...>(std::forward<Args>(args)...);
            // };

            // // auto FF = F{};
            // // return f(8, 8);
            // // return FF(*this, f);
            // return F{}(*this, f, arg);
            // // return F{}(*this, f, std::forward<Arg>(arg));
        }

    public:
        // reflect_member_function(int) {}

        // auto operator=(int) {}
        template <typename Arg>
        // decltype(auto)
        auto
         // operator=(Arg&& arg)
         operator=(Arg arg)
         // operator=(int arg)
            // -> decltype(indirect(std::forward<Arg>(arg)))
            -> decltype(indirect(arg))
        {
            // return 0;
            return indirect(arg);
            // return indirect(std::forward<Arg>(arg));
        }

        // auto operator=(int) {}
        // reflect_member_function() = default;
        // reflect_member_function(const reflect_member_function &) = default;
        reflect_member_function &operator=(const reflect_member_function &) = default;
        // reflect_member_function(reflect_member_function &&) = default;
        // reflect_member_function &operator=(const reflect_member_function &) = default;
        // reflect_member_function &operator=(reflect_member_function &&) = default;
    };
// };

// template<class Derived>
// using MemberForInt = using_member_t<
//     int,
//     Derived,
//     // reflection::reflected_member_t<int, 0>
//     ASDF
// >;

template<class Derived>
using MemberForInt = reflect_member_function<Derived>;
// using MemberForInt = reflect_member_function<Forwarder<int, Derived>>;
// using MemberForInt = decltype(
//     typename Reflection::template reflect_member_function<F>{}
//     // reflection::reify(
//     //     ASDF{},
//     //     Forwarder<int, Derived>{}
//     // )
// );

// static_assert(reflection::reflect<reflection::reflected_member_t<int, 0>> != reflection::reflect<void>);

// TODO: Unify the non-generic and generic ReflectedMembers - how they are registered is kind of an 'implementation detail'
template<typename Delegate, class Derived = void>
class using_ : public using_base<Delegate, Derived>
             // , public Members<Delegate, using_<Delegate, Derived>>
             , public MemberForInt<using_<Delegate, Derived>>
             // , public ReflectedMembers<Delegate, using_<Delegate, Derived>>
             // , public ReflectedMembers<Delegate, using_<Delegate, Derived>, utils::Delayed<reflection::auto_, Delegate>>
{
public:
    using MemberForInt<using_<Delegate, Derived>>::MemberForInt;
    using MemberForInt<using_<Delegate, Derived>>::operator=;

    using_() = default;
    using_(const using_ &) = default;
    using_ &operator=(const using_ &) = default;
    // using_(using_ &&) = default;
    // using_ &operator=(using_ &&) = default;

    // using ReflectedMembers<Delegate, using_<Delegate, Derived>>::operator=...;
    // using ReflectedMembers<Delegate, using_<Delegate, Derived>, utils::Delayed<reflection::auto_, Delegate>>::operator=...;
};

} // namespace smartref
