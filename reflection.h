#define REFLECT(name) \
  __reflection_tag_##name() \
  { \
    throw std::logic_error{"__reflection_tag should never be called"}; \
  } \
  \
  struct __reflection_##name \
  { \
  }; \

namespace reflection {

template<typename T, size_t counter>
struct reflected_member
{
    using type = void;
};

template<typename T, size_t counter>
using reflected_member_t = typename reflected_member<T, counter>::type;

template<typename T, size_t index = 0, typename = reflected_member_t<T, 0>>
struct reflected_member_count_lowerbound
{
    static constexpr auto value = index;
};

template<typename T, size_t index>
struct reflected_member_count_lowerbound<T, index, void>
{
    static constexpr auto value = reflected_member_count_lowerbound<T, index, reflected_member_t<T, index + 1>>::value;
};

template<typename T, size_t index = 0, typename = reflected_member_t<T, 0>>
struct reflected_member_count_upperbound
{
    static constexpr auto value = reflected_member_count_upperbound<T, index + 1, reflected_member_t<T, index + 1>>::value;
};

template<typename T, size_t index>
struct reflected_member_count_upperbound<T, index, void>
{
    static constexpr auto value = index;
};

template<typename T>
constexpr auto reflected_member_count_v = reflected_member_count_upperbound<T>::value - reflected_member_count_lowerbound<T>::value;

template<typename T, size_t index>
constexpr auto is_reflected_member = std::is_void_v<reflected_member_t<T, index>>;

template<typename T>
constexpr auto reflected_member_count()
{
    const auto max = 5;

    auto count = size_t{};
    for (size_t index = 0; index != max; ++index)
        if (is_reflected_member<T, index>)
            ++count;

    return count;
}

struct Foo {};
struct Bar {};
struct Baz {};
struct Bat {};

template<> struct reflected_member<Bar, 0> {using type = int;};
template<> struct reflected_member<Baz, 0> {using type = int;};
template<> struct reflected_member<Baz, 1> {using type = int;};
template<> struct reflected_member<Bat, 1> {using type = int;};

// static_assert(reflected_member_count_v<Foo> == 0);
static_assert(reflected_member_count_v<Bar> == 1);
static_assert(reflected_member_count_v<Baz> == 2);
// static_assert(reflected_member_count_v<Bat> == 1);
// static_assert(reflected_member_count<Bar>() == 1);
// static_assert(reflected_member_count<Baz>() == 2);

auto _ = []{
    // std::cout << "Foo" << std::endl;
    // std::cout << reflected_member_count_lowerbound<Foo>::value << std::endl;
    // std::cout << reflected_member_count_upperbound<Foo>::value << std::endl;
    // std::cout << reflected_member_count_v<Foo> << std::endl;

    std::cout << "Bar" << std::endl;
    std::cout << reflected_member_count_lowerbound<Bar>::value << std::endl;
    std::cout << reflected_member_count_upperbound<Bar>::value << std::endl;
    std::cout << reflected_member_count_v<Bar> << std::endl;

    std::cout << "Baz" << std::endl;
    std::cout << reflected_member_count_lowerbound<Baz>::value << std::endl;
    std::cout << reflected_member_count_upperbound<Baz>::value << std::endl;
    std::cout << reflected_member_count_v<Baz> << std::endl;

    std::cout << "Bat" << std::endl;
    std::cout << reflected_member_count_lowerbound<Bat>::value << std::endl;
    std::cout << reflected_member_count_upperbound<Bat>::value << std::endl;
    std::cout << reflected_member_count_v<Bat> << std::endl;
    return 0;
}();

} // namespace reflection
