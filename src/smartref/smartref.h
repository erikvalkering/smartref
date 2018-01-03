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
    template<typename Forwarder, typename Derived>
    class reflect_member_function
     /*: public reflection::reflect_base<reflection::reflected_kind::member_function> */{
    private:
        struct F2
        {
            template<typename Obj, typename Arg>
            auto operator()(Obj &obj, Arg arg)
                -> decltype(obj = arg)
            {
                return obj = arg;
            };
        };

        template <typename Arg>
        auto indirect(Arg &&arg)
            -> decltype(Forwarder{}(*this, F2{}, arg))
        {
            return Forwarder{}(*this, F2{}, arg);
        }

    public:
        reflect_member_function() = default;
        reflect_member_function(const reflect_member_function &) = default;
        reflect_member_function(reflect_member_function &&) = default;
        reflect_member_function &operator=(const reflect_member_function &) = default;
        reflect_member_function &operator=(reflect_member_function &&) = default;

        template <typename Arg>
        auto operator=(Arg &&arg)
            -> decltype(indirect(arg))
        {
            return indirect(arg);
        }
    };

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
