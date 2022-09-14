#include <gtest/gtest.h>

#include "traits.hpp"
using namespace std;

TEST(HelloTest, TupleSerializatin)
{
    std::tuple<int, int, int, int> t{1, 2, 3, 4};

    vector<int> serialized_vec = phi::serialize(t);
    const vector expect{1, 2, 3, 4};
    EXPECT_EQ(serialized_vec, expect);
}

TEST(HelloTest, TupleDeserializatin)
{
    using phi_tuple = std::tuple<int, int, int, int>;
    const vector input{1, 2, 3, 4};
    
    phi_tuple expect{1, 2, 3, 4};
    phi_tuple out{};
    
    phi::deserialize(out,input);

    EXPECT_EQ(out, expect);
}
