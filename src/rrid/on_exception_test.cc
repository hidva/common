
#include "rrid/on_exception.h"
#include "rrid/scope_exit.h"
#include "gtest/gtest.h"

void ThrowException()
{
    throw 22;
}

TEST(GetUncaughtExceptionsNumTest,test1)
{
    EXPECT_EQ(0,::pp_qq::rrid_detail::GetUncaughtExceptionsNum());
    try {
        ON_SCOPE_EXIT(test1) { EXPECT_EQ(1,::pp_qq::rrid_detail::GetUncaughtExceptionsNum()); };
        ThrowException();
    } catch (...) {
    }
}

namespace {

int obj_for_foo = 0;

void Foo()
{
    obj_for_foo = 33;
    return ;
}

void FooTest(bool throw_exception)
{
    obj_for_foo = 0;
    try {
        auto on_exception_obj = pp_qq::MakeOnException(Foo);
        EXPECT_EQ(0,obj_for_foo);
        if (throw_exception)
            throw 33;
    } catch (...) {}
    if (throw_exception)
        EXPECT_EQ(33,obj_for_foo);
    else
        EXPECT_EQ(0,obj_for_foo);
    return ;
}

} // namespace

TEST(OnExceptionTest,NormalFunction)
{
    FooTest(true);
    FooTest(false);
}

namespace {

struct FooFunctor {

    FooFunctor(int &obj):
        obj_(&obj)
    {
    }

    FooFunctor(const FooFunctor &other) noexcept(true):
        obj_(other.obj_)
    {
    }

    void operator() ()
    {
        *obj_ = 33;
        return ;
    }

private:
    int *obj_;
};

template <typename FunctorType>
void FunctorTestFunction(bool throw_exception)
{
    int obj = 0;
    FunctorType fc(obj);
    try {
        auto on_exception_obj = pp_qq::MakeOnException(fc);
        EXPECT_EQ(0,obj);
        if (throw_exception)
            throw 33;
    } catch (...) {}
    if (throw_exception)
        EXPECT_EQ(33,obj);
    else
        EXPECT_EQ(0,obj);
    return ;
}

}


namespace {

struct FooFunctor01 {

    FooFunctor01(int &obj):
        obj_(&obj)
    {
    }

    FooFunctor01(const FooFunctor01 &other) noexcept(true):
        obj_(other.obj_)
    {
    }

    FooFunctor01(FooFunctor01 &&other) noexcept(false):
        obj_(other.obj_)
    {
        throw 33;
    }

    void operator() ()
    {
        *obj_ = 33;
        return ;
    }

private:
    int *obj_;
};

struct FooFunctor10 {

    FooFunctor10(int &obj):
        obj_(&obj)
    {
    }

    FooFunctor10(const FooFunctor10 &other) noexcept(false):
        obj_(other.obj_)
    {
        throw 33;
    }

    FooFunctor10(FooFunctor10 &&other) noexcept(true):
        obj_(other.obj_)
    {
    }

    void operator() ()
    {
        *obj_ = 33;
        return ;
    }

private:
    int *obj_;
};

struct FooMoveNoexcept {
    FooMoveNoexcept(int &obj):
        obj_(&obj)
    {
    }

    FooMoveNoexcept(const FooMoveNoexcept&) = delete;

    FooMoveNoexcept(FooMoveNoexcept &&other) noexcept(true):
        obj_(other.obj_)
    {
    }

    void operator() ()
    {
        *obj_ = 33;
        return ;
    }

private:
    int *obj_ ;
};

struct FooExcept {
    FooExcept(int &obj):
        obj_(&obj)
    {
    }

    FooExcept(const FooExcept&) = delete;

    FooExcept(FooExcept &&other) noexcept(false):
        obj_(other.obj_)
    {
        throw 33;
    }

    void operator() ()
    {
        *obj_ = 33;
        return ;
    }

private:
    int *obj_ ;
};

}

TEST(OnExceptionTest,FunctorTest)
{
    FunctorTestFunction<FooFunctor>(true);
    FunctorTestFunction<FooFunctor>(false);

    {
        int obj = 0;
        bool have_exception = false;
        FooFunctor01 foo_obj{obj};
        try {
            auto on_exception_obj = pp_qq::MakeOnException(foo_obj);
        } catch(...) {
            EXPECT_EQ(33,obj);
            have_exception = true;
        }
        if (!have_exception)
            EXPECT_EQ(0,obj);
    }

    {
        int obj = 0;
        bool have_exception = false;
        FooFunctor10 foo_obj{obj};
        try {
            auto on_exception_obj = pp_qq::MakeOnException(foo_obj);
        } catch(...) {
            EXPECT_EQ(33,obj);
            have_exception = true;
        }
        if (!have_exception)
            EXPECT_EQ(0,obj);
    }

    {
        int obj = 0;
        auto throw_exception = true;
        FooMoveNoexcept fc(obj);
        try {
            auto on_exception_obj = pp_qq::MakeOnException(std::move(fc));
            EXPECT_EQ(0,obj);
            if (throw_exception)
                throw 33;
        } catch (...) {}
        if (throw_exception)
            EXPECT_EQ(33,obj);
        else
            EXPECT_EQ(0,obj);
        return ;
    }
    {
        int obj = 0;
        auto throw_exception = false;
        FooMoveNoexcept fc(obj);
        try {
            auto on_exception_obj = pp_qq::MakeOnException(std::move(fc));
            EXPECT_EQ(0,obj);
            if (throw_exception)
                throw 33;
        } catch (...) {}
        if (throw_exception)
            EXPECT_EQ(33,obj);
        else
            EXPECT_EQ(0,obj);
        return ;
    }
    {
        int obj = 0;
        auto throw_exception = true;
        FooExcept fc(obj);
        try {
            auto on_exception_obj = pp_qq::MakeOnException(std::move(fc));
            EXPECT_EQ(0,obj);
            if (throw_exception)
                throw 33;
        } catch (...) {}
        if (throw_exception)
            EXPECT_EQ(33,obj);
        else
            EXPECT_EQ(0,obj);
        return ;
    }
    {
        int obj = 0;
        auto throw_exception = false;
        FooExcept fc(obj);
        try {
            auto on_exception_obj = pp_qq::MakeOnException(std::move(fc));
            EXPECT_EQ(0,obj);
            if (throw_exception)
                throw 33;
        } catch (...) {}
        if (throw_exception)
            EXPECT_EQ(33,obj);
        else
            EXPECT_EQ(0,obj);
        return ;
    }
}

void LambdaTestFunction(bool throw_exception)
{
    int obj = 0;
    try {
        ON_EXCEPTIN {
            obj = 33;
        };
        if (throw_exception)
            throw 33;
    } catch (...) {}
    if (throw_exception)
        EXPECT_EQ(33,obj);
    else
        EXPECT_EQ(0,obj);
}

TEST(OnExceptionTest,LambdaTest)
{
    LambdaTestFunction(true);
    LambdaTestFunction(false);
}

