#include <gtest/gtest.h>
#include <tuple>
#include <list>
#include "magician/traits.hpp"

using namespace std;

TEST(Tuple, TupleSerializatin) {
  std::tuple<int, int, int, int> t{1, 2, 3, 4};

  vector<int> serialized_vec = phi::serialize(t);
  const vector expect{1, 2, 3, 4};
  EXPECT_EQ(serialized_vec, expect);
}

TEST(Tuple, TupleDeserializatin) {
  using phi_tuple = std::tuple<int, int, int, int>;
  const vector input{1, 2, 3, 4};

  phi_tuple expect{1, 2, 3, 4};
  phi_tuple out{};

  phi::deserialize(out, input);

  EXPECT_EQ(out, expect);
  EXPECT_EQ(out, expect);
  EXPECT_EQ(out, expect);
}

TEST(Tuple, UniqueType) {
  auto v0 = phi::is_unique_type_list<>::value;
  // type list with all type different.
  auto v1 = phi::is_unique_type_list<int, string, double>::value;
  // type list with duplicate type.
  auto v2 = phi::is_unique_type_list<string, string, double>::value;
  // single type list is always unique.
  bool v3 = phi::is_unique_type_list<int>::value;
  EXPECT_TRUE(v0);
  EXPECT_TRUE(v1);
  EXPECT_FALSE(v2);
  EXPECT_TRUE(v3);
}


TEST(Tuple,ToAndFromJSON){
  using pair_of_4_int = std::tuple<int, int, int, int>;
  pair_of_4_int t{1, 2, 3, 4};

  auto str = phi::experimental::serialize(t);

  pair_of_4_int out;
  phi::experimental::deserialize(out,str);

  EXPECT_EQ(get<0>(out),1);
  EXPECT_EQ(get<1>(out),2);
  EXPECT_EQ(get<2>(out),3);
  EXPECT_EQ(get<3>(out),4);
}


TEST(Traits, IsVector) {
  using std::list;

  EXPECT_TRUE(phi::vector_traits::is_vector<vector<int>>::value);
  EXPECT_TRUE(phi::vector_traits::is_vector<vector<int&>>::value);
  EXPECT_FALSE(phi::vector_traits::is_vector<list<int>>::value);
}

/**
 * basic usage
 */
TEST(Traits, Overloaded) {
  auto add = [](int x) { cout << x + 1 << endl; };
  auto join = [](const string &s) { cout << s << endl; };
  auto h = phi::overloaded<decltype(add), decltype(join)>{add, join};

  h(1);
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

  auto h = phi::deprecated::make_overloaded(add, join);
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

  auto h = phi::overloaded{add, join};
  h(value);
  h(right);

  EXPECT_EQ(value, x);
  EXPECT_EQ(left, combined);
}

enum class Status_A{
  A_0,
  A_1,
  A_2,
};

enum class Status_B{
  B_0,
  B_1,
  B_2,
};

enum class Status_C{
  C_0,
  C_1,
  C_2,
};

TEST(Status,Base){
  phi::StatusManager<Status_A,Status_B> manager;
  manager.addState(Status_A::A_0);
//  manager.addState(Status_C::C_0);
}

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
  int r = phi::util::apply(consumer, params);

  EXPECT_EQ(r, EXPECT_RET);
}
