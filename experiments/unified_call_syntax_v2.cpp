struct FreeFunctionInvoker
{
    template<typename Object>
    friend auto invoke(FreeFunctionInvoker, Object object)
    {
        return foo(object);
    }
};

struct MemberFunctionInvoker
{
    template<typename Object>
    friend auto invoke(MemberFunctionInvoker, Object object)
    {
        return object.foo();
    }
};

struct CombinedInvoker
{
    auto as_free_function() { return FreeFunctionInvoker{}; }
    auto as_member_function() { return MemberFunctionInvoker{}; }
};

template<typename Derived>
struct FreeFunctionExposer : FreeFunctionInvoker, CombinedInvoker
{
    friend auto foo(FreeFunctionExposer &)
    {
        return on_call(FreeFunctionExposer{}, Derived{});
    }
};

template<typename Derived>
struct MemberFunctionExposer : MemberFunctionInvoker, CombinedInvoker
{
    auto foo()
    {
        return on_call(MemberFunctionExposer{}, Derived{});
    }
};

template<typename Derived>
struct CombinedExposer : FreeFunctionExposer<Derived>, MemberFunctionExposer<Derived> {};

template<typename T>
struct SmartRef : CombinedExposer<SmartRef<T>>
{
    template<typename Invoker, typename Self>
    friend auto on_call(Invoker invoker, Self self)
    {
        return invoke(invoker.as_free_function(), T{});
    }
};

namespace foobar {
struct Foo
{
//    auto foo() { return 2; }
};

auto foo(Foo) { return 3; }
}

int main()
{
    auto ref = SmartRef<foobar::Foo>{};

    return ref.foo();
}
