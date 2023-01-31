//
// Created by wyshou on 2023/1/31.
//
#include <gtest/gtest.h>
#include <magician/trivial_type_traits.h>

using namespace std;
using namespace phi;

TEST(TraisUtil, Apply) {
  constexpr int EXPECT_CODE = 200;
  constexpr int EXPECT_RET = 500;
  const string EXPECT_MSG = "SUCCESS";
  auto consumer = [&EXPECT_CODE, EXPECT_MSG](int &code, string &msg) {
    EXPECT_EQ(code, EXPECT_CODE);
    EXPECT_EQ(msg, EXPECT_MSG);

    return EXPECT_RET;
  };

  tuple<int, string> params = make_tuple(200, "SUCCESS");
  int r = phi::apply(consumer, params);

  EXPECT_EQ(r, EXPECT_RET);
  auto size = phi::make_index_sequence<1>::size;
}