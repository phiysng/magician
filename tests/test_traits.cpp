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