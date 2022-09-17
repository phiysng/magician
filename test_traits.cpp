#include <gtest/gtest.h>

#include <magician/traits.hpp>

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
  // type list with all type different.
  auto v1 = phi::is_unique_type_list<int, string, double>::value;
  // type list with duplicate type.
  auto v2 = phi::is_unique_type_list<string, string, double>::value;
  // single type list is always unique.
  bool v3 = phi::is_unique_type_list<int>::value;
  EXPECT_TRUE(v1);
  EXPECT_FALSE(v2);
  EXPECT_TRUE(v3);
}
