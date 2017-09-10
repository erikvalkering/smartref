#include <reflection/reflection.h>
#include <smartref/smartref.h>

#include <vector>

namespace tests_containers {

template<typename T>
struct Ref : smartref::using_<T>
{
  T ref;

  operator T &()
  {
    return ref;
  }
};

using T = std::vector<int>;

using reflection::reflect;

static_assert(reflect<Ref<T>::value_type>             == reflect<T::value_type>);
static_assert(reflect<Ref<T>::allocator_type>         == reflect<T::allocator_type>);
static_assert(reflect<Ref<T>::size_type>              == reflect<T::size_type>);
static_assert(reflect<Ref<T>::difference_type>        == reflect<T::difference_type>);
static_assert(reflect<Ref<T>::reference>              == reflect<T::reference>);
static_assert(reflect<Ref<T>::const_reference>        == reflect<T::const_reference>);
static_assert(reflect<Ref<T>::pointer>                == reflect<T::pointer>);
static_assert(reflect<Ref<T>::const_pointer>          == reflect<T::const_pointer>);
static_assert(reflect<Ref<T>::iterator>               == reflect<T::iterator>);
static_assert(reflect<Ref<T>::const_iterator>         == reflect<T::const_iterator>);
static_assert(reflect<Ref<T>::reverse_iterator>       == reflect<T::reverse_iterator>);
static_assert(reflect<Ref<T>::const_reverse_iterator> == reflect<T::const_reverse_iterator>);

} // namespace tests_containers
