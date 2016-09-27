
#include "scope_exit.h"
#include "gtest/gtest.h"

#include <functional>
#include <type_traits>
#include <vector>
#include <stdexcept>

using namespace pp_qq;
using std::vector;

TEST(ScopeGuardImplBaseTest,DismissTest)
{
    pp_qq::rrid_detail::ScopeGuardImplBase test_object;
    EXPECT_FALSE(test_object.dismissed());

    test_object.Dismiss();
    EXPECT_TRUE(test_object.dismissed());
}

double ReturnDouble()
{
    return 0.0;
}

class MyFunctor {
public:
    explicit MyFunctor(int* ptr) : ptr_(ptr) {}

    void operator ()()
    {
        ++*ptr_;
    }

private:
    int* ptr_;
};

TEST(ScopeGuardImplTest, DifferentWaysToBind)
{
    {
        auto g = MakeScopeGuard(ReturnDouble);
    }

    vector<int> v;
    void (vector<int>::*push_back)(int const&) = &vector<int>::push_back;

    v.push_back(1);
    {
        // binding to member function.
        auto g = MakeScopeGuard(std::bind(&vector<int>::pop_back, &v));
    }
    EXPECT_EQ((size_t)0, v.size());

    {
        // bind member function with args. v is passed-by-value!
        auto g = MakeScopeGuard(std::bind(push_back, v, 2));
    }
    EXPECT_EQ((size_t)0, v.size()); // push_back happened on a copy of v... fail!

    {
        // pass in an argument by pointer so to avoid copy.
        auto g = MakeScopeGuard(std::bind(push_back, &v, 4));
    }
    EXPECT_EQ((size_t)1, v.size());

    {
        // pass in an argument by reference so to avoid copy.
        auto g = MakeScopeGuard(std::bind(push_back, std::ref(v), 4));
    }
    EXPECT_EQ((size_t)2, v.size());

    {
        // lambda with a reference to v
        auto g = MakeScopeGuard([&] { v.push_back(5); });
    }
    EXPECT_EQ((size_t)3, v.size());

    {
        // lambda with a copy of v
        auto g = MakeScopeGuard([v] () mutable { v.push_back(6); });
    }
    EXPECT_EQ(3U, v.size());

    // functor object
    int n = 0;
    {
        MyFunctor f(&n);
        auto g = MakeScopeGuard(f);
    }
    EXPECT_EQ(1, n);

    // temporary functor object
    n = 0;
    {
        auto g = MakeScopeGuard(MyFunctor(&n));
    }
    EXPECT_EQ(1, n);

    // Use auto instead of ScopeGuard
    n = 2;
    {
        auto g = MakeScopeGuard(MyFunctor(&n));
    }
    EXPECT_EQ(3, n);

    // Use const auto& instead of ScopeGuard
    n = 10;
    {
        const auto& g = MakeScopeGuard(MyFunctor(&n));
    }
    EXPECT_EQ(11, n);
}

TEST(ScopeGuardImplTest, GuardException)
{
    EXPECT_DEATH(
    {
        auto g = MakeScopeGuard([&] {
            throw std::runtime_error("destructors should never throw!");
        });
    },
    "destructors should never throw!"
    );
}

/**
 * Add an integer to a vector iff it was inserted into the
 * db successfuly. Here is a schematic of how you would accomplish
 * this with scope guard.
 */
void TestUndoAction(bool failure)
{
    vector<int64_t> v;
    { // defines a "mini" scope

        // be optimistic and insert this into memory
        v.push_back(1);

        // The guard is triggered to undo the insertion unless dismiss() is called.
        auto guard = MakeScopeGuard([&] { v.pop_back(); });

        // Do some action; Use the failure argument to pretend
        // if it failed or succeeded.

        // if there was no failure, dismiss the undo guard action.
        if (!failure)
            guard.Dismiss();
    } // all stack allocated in the mini-scope will be destroyed here.

    if (failure)
        EXPECT_EQ(0U, v.size()); // the action failed => undo insertion
    else
        EXPECT_EQ(1U, v.size()); // the action succeeded => keep insertion
}

TEST(ScopeGuardImplTest, UndoAction)
{
    TestUndoAction(true);
    TestUndoAction(false);
}

/**
 * Sometimes in a try catch block we want to execute a piece of code
 * regardless if an exception happened or not. For example, you want
 * to close a db connection regardless if an exception was thrown during
 * insertion. In Java and other languages there is a finally clause that
 * helps accomplish this:
 *
 *   try {
 *     dbConn.doInsert(sql);
 *   } catch (const DbException& dbe) {
 *     dbConn.recordFailure(dbe);
 *   } catch (const CriticalException& e) {
 *     throw e; // re-throw the exception
 *   } finally {
 *     dbConn.closeConnection(); // executes no matter what!
 *   }
 *
 * We can approximate this behavior in C++ with ScopeGuard.
 */
enum class ErrorBehavior {
    kSUCCESS,
    kHANDLED_ERROR,
    kUNHANDLED_ERROR
};

void TestFinally(ErrorBehavior error)
{
    bool cleanupOccurred = false;

    try {
        auto guard = MakeScopeGuard([&] { cleanupOccurred = true; });

        try {
            if (error == ErrorBehavior::kHANDLED_ERROR)
                throw std::runtime_error("throwing an expected error");
            else if (error == ErrorBehavior::kUNHANDLED_ERROR)
                throw "never throw raw strings";
        } catch (const std::runtime_error&) {
        }
    } catch (...) {
        // Outer catch to swallow the error for the UNHANDLED_ERROR behavior
    }

    EXPECT_TRUE(cleanupOccurred);
}

TEST(ScopeGuardImplTest, TryCatchFinally)
{
    TestFinally(ErrorBehavior::kSUCCESS);
    TestFinally(ErrorBehavior::kHANDLED_ERROR);
    TestFinally(ErrorBehavior::kUNHANDLED_ERROR);
}

TEST(ScopeGuardImplTest, ON_SCOPE_EXIT_TEST)
{
    int x = 0;
    {
        ON_SCOPE_EXIT (AddX) { ++x; };
        EXPECT_EQ(0, x);
    }
    EXPECT_EQ(1, x);
}

class Foo {
public:
    Foo() {}
    ~Foo()
    {
        try {
            auto e = std::current_exception();
            int test = 0;
            {
                ON_SCOPE_EXIT (AddTest) { ++test; };
                EXPECT_EQ(0, test);
            }
            EXPECT_EQ(1, test);
        } catch (const std::exception& ex) {
        }
    }
};

TEST(ScopeGuardImplTest, TEST_SCOPE_FAILURE2)
{
    try {
        Foo f;
        throw std::runtime_error("test");
    } catch (...) {
    }
}

#if 0
void testScopeFailAndScopeSuccess(ErrorBehavior error, bool expectFail) {
  bool scopeFailExecuted = false;
  bool scopeSuccessExecuted = false;

  try {
    SCOPE_FAIL { scopeFailExecuted = true; };
    SCOPE_SUCCESS { scopeSuccessExecuted = true; };

    try {
      if (error == ErrorBehavior::HANDLED_ERROR) {
        throw std::runtime_error("throwing an expected error");
      } else if (error == ErrorBehavior::UNHANDLED_ERROR) {
        throw "never throw raw strings";
      }
    } catch (const std::runtime_error&) {
    }
  } catch (...) {
    // Outer catch to swallow the error for the UNHANDLED_ERROR behavior
  }

  EXPECT_EQ(expectFail, scopeFailExecuted);
  EXPECT_EQ(!expectFail, scopeSuccessExecuted);
}

TEST(ScopeGuard, TEST_SCOPE_FAIL_AND_SCOPE_SUCCESS) {
  testScopeFailAndScopeSuccess(ErrorBehavior::kSUCCESS, false);
  testScopeFailAndScopeSuccess(ErrorBehavior::kHANDLED_ERROR, false);
  testScopeFailAndScopeSuccess(ErrorBehavior::kUNHANDLED_ERROR, true);
}

TEST(ScopeGuard, TEST_SCOPE_SUCCESS_THROW) {
  auto lambda = []() {
    SCOPE_SUCCESS { throw std::runtime_error("ehm"); };
  };
  EXPECT_THROW(lambda(), std::runtime_error);
}

TEST(ScopeGuard, TEST_THROWING_CLEANUP_ACTION) {
  struct ThrowingCleanupAction {
    explicit ThrowingCleanupAction(int& scopeExitExecuted)
        : scopeExitExecuted_(scopeExitExecuted) {}
    ThrowingCleanupAction(const ThrowingCleanupAction& other)
        : scopeExitExecuted_(other.scopeExitExecuted_) {
      throw std::runtime_error("whoa");
    }
    void operator()() { ++scopeExitExecuted_; }

   private:
    int& scopeExitExecuted_;
  };
  int scopeExitExecuted = 0;
  ThrowingCleanupAction onExit(scopeExitExecuted);
  EXPECT_THROW(MakeScopeGuard(onExit), std::runtime_error);
  EXPECT_EQ(scopeExitExecuted, 1);
}
#endif
