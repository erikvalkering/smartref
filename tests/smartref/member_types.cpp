#include <smartref.h>

#include <type_traits>
#include <vector>
#include <list>


namespace tests {

using smartref::using_;

//! These tests check for absence of the value_type member-type,
//! for using_<int>, which obviously doesn't have this member-type.
namespace test_absence {

template<typename T, typename = typename T::value_type>
constexpr auto has_value_type(int) {return true;}
template<typename T>
constexpr auto has_value_type(...) {return false;}

//! 1. Accessing the value_type member-type should be invalid
//! 2. Instantiating the class *template* should work,
//!    and should not result in an ill-formed class.
//!    (which is the case if value_type refers to an invalid type)
static_assert(!has_value_type<using_<int>>(0),
              "TEST FAILED: value_type should not be defined for using_<int>!");

} // namespace test_absence

//! A basic check to test support for std::vector<int>::value_type
static_assert(
    std::is_same<
        using_<std::vector<int>>::value_type,
               std::vector<int> ::value_type
    >::value);

//! Check whether we are not hard-coding support for std::vector
static_assert(
    std::is_same<
        using_<std::list<int>>::value_type,
               std::list<int> ::value_type
    >::value);

//! Check whether we are not hard-coding support for std::vector<int>
static_assert(
    std::is_same<
        using_<std::list<float>>::value_type,
               std::list<float> ::value_type
    >::value);

} // namespace tests
