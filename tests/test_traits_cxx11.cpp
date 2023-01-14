//
// Created by Wu Yuanshou on 2023/1/13.
//
#include <gtest/gtest.h>
#include "magician/traits_cxx11.hpp"
#include <iostream>

using namespace std;

TEST(OverLoadedCXX11, Simple) {
  auto h = phi::cxx11::make_overload([](int x) {
                                       cout << x << endl;
                                     },
                                     [](const string &s) {
                                       cout << "overloaded " << s << endl;
                                     }
  );

  h(110);
  h("Hello World");

}

/**
 * make_overloaded helper function
 */
TEST(Traits, MakeOverloaded) {
  int x = 0;
  constexpr int value = 10;
  string left = "<";
  string right = ">";
  string combined = left + right;
  auto add = [&x](int i) { x += i; };
  auto join = [&left](const string &s) { left += s; };

  auto h = phi::cxx11::make_overload(add, join);
  h(value);
  h(right);

  EXPECT_EQ(value, x);
  EXPECT_EQ(left, combined);
}

/**
 * CDAT implementation
 */
TEST(Traits, CDAT) {
  int x = 0;
  constexpr int value = 10;
  string left = "<";
  string right = ">";
  string combined = left + right;
  auto add = [&x](int i) { x += i; };
  auto join = [&left](const string &s) { left += s; };

  auto h = phi::cxx11::make_overload(add, join);
  h(value);
  h(right);

  EXPECT_EQ(value, x);
  EXPECT_EQ(left, combined);
}
