#include <smartref/smartref.h>
#include <reflection/reflect.h>
#include <utils/utils.h>

using namespace std;
using namespace smartref;
using namespace reflection;

template<typename T>
struct RefVirtual : using_<T> {};

template<typename T>
struct RefCRTP : using_<T, RefCRTP<T>> {};

static_assert(reflect<DelegateType<decltype(declval<RefVirtual<int> &>())>> == reflect<int>);
static_assert(reflect<DelegateType<decltype(declval<RefCRTP<int> &>())>> == reflect<int>);
