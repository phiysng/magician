#include <gtest/gtest.h>
#include <magician/delegate.hpp>

namespace phi::test {
static int cnt_foo = 0;

int delegate_foo(vector<int> vec) {
  ++cnt_foo;
  return vec.size();
}
}
using namespace std;
TEST(Delegate, FreeFunction) {
  phi::delegate<int, vector<int>> delegate;
  delegate.bind(&phi::test::delegate_foo);
  delegate.emit(vector{1, 2, 3});
  EXPECT_EQ(phi::test::cnt_foo, 1);
}