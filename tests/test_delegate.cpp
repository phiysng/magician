#include <gtest/gtest.h>
#include <magician/delegate.hpp>

namespace phi::test {
static int cnt_foo = 0;

int delegate_foo(const int &a, const vector<int> &vec) {
  ++cnt_foo;

  vector<int> expectVec{1, 2, 3};

  EXPECT_EQ(a, 9);
  EXPECT_EQ(vec, expectVec);
  return vec.size();
}
}

using namespace std;
TEST(Delegate, FreeFunction) {
  phi::delegate<int, int, vector<int>> test_delegate;
  test_delegate.bind(&phi::test::delegate_foo);
  test_delegate.emit(9, vector{1, 2, 3});
  EXPECT_EQ(phi::test::cnt_foo, 1);
}

TEST(Delegate, Lambda) {
  phi::delegate<int, int, vector<int>> test_delegate;
  int cnt = 0;
  test_delegate.bind([&cnt](int x, vector<int> vec) {
    cnt++;
    return x;
  });
  test_delegate.emit(9, vector{1, 2, 3});
  EXPECT_EQ(cnt, 1);
}

TEST(Delegate, Functor) {
  struct TestDelegate {
    explicit TestDelegate(int &in_cnt) : cnt(in_cnt) {}
    int operator()(int &x, const vector<int> &vec) const {
      cnt++;
      return x;
    }
    int &cnt;
  };

  phi::delegate<int, int, vector<int>> test_delegate;
  int cnt = 0;
  test_delegate.bind(TestDelegate(cnt));
  test_delegate.emit(9, vector{1, 2, 3});
  EXPECT_EQ(cnt, 1);

}