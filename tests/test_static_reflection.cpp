//
// Created by wyshou on 2022/12/17.
//
#include <gtest/gtest.h>
#include <string>
#include <magician/static_reflection.hpp>
#include <ostream>
#include <utility>

using namespace std;

struct IntValue {
  int int_;
  friend ostream &operator<<(ostream &Os, const IntValue &Value) {
    Os << "int_: " << Value.int_;
    return Os;
  }
};

DEFINE_STRUCT_SCHEMA(IntValue,
                     DEFINE_STRUCT_FIELD(int_, int)
)

struct SimpleStruct {
  friend ostream &operator<<(ostream &Os, const SimpleStruct &AStruct) {
    Os << "bool_: " << AStruct.bool_ << " int_: " << AStruct.int_ << " double_: " << AStruct.double_ << " string_: "
       << AStruct.string_ << " vec_: " << AStruct.vec_.size() << " intValue_: " << AStruct.intValue_;
    return Os;
  }
public:
  SimpleStruct() : bool_(false), int_(0), double_(0.0f), string_() {}
  SimpleStruct(bool Bool, int Int, double Double, string String)
      : bool_(Bool), int_(Int), double_(Double), string_(std::move(String)) {}

  bool bool_;
  int int_;
  double double_;
  std::string string_;
  vector<int> vec_;
  IntValue intValue_;
};

DEFINE_STRUCT_SCHEMA(SimpleStruct,
                     DEFINE_STRUCT_FIELD(bool_, bool),
                     DEFINE_STRUCT_FIELD(int_, int),
                     DEFINE_STRUCT_FIELD(double_, double),
                     DEFINE_STRUCT_FIELD(string_, string),
                     DEFINE_STRUCT_FIELD(intValue_, IntValue),
                     DEFINE_STRUCT_FIELD(vec_, vector<int>)
)

TEST(Reflection, Serialize) {
  nlohmann::json object = {

      {"bool_", true},
      {"int_", 1234},
      {"double_", 12.34f},
      {"string_", "hello world"},
      {"intValue_", {
          {"int_", 7890}
      }},
      {"vec_", {10000, 10001, 10002, 10003, 10004}}

  };

  SimpleStruct simpleStruct{};
  phi::serialize(object, simpleStruct);

  EXPECT_EQ(simpleStruct.bool_, true);
  EXPECT_EQ(simpleStruct.int_, 1234);
  EXPECT_FLOAT_EQ(simpleStruct.double_, 12.34);
  EXPECT_EQ(simpleStruct.string_, "hello world");
  EXPECT_EQ(simpleStruct.intValue_.int_, 7890);

  vector<int> expect_vec{10000, 10001, 10002, 10003, 10004};
  EXPECT_EQ(simpleStruct.vec_, expect_vec);
}

TEST(Reflection, Deserialize) {
  SimpleStruct simpleStruct{};
  simpleStruct.bool_=true;
  simpleStruct.int_=1234;
  simpleStruct.double_=12.34;
  simpleStruct.string_= "hello world";
  simpleStruct.intValue_.int_= 7890;

  vector<int> expect_vec{10000, 10001, 10002, 10003, 10004};
  simpleStruct.vec_= expect_vec;

  nlohmann::json object;
  phi::deserialize(simpleStruct, object);

  EXPECT_TRUE(object.contains("bool_"));
  EXPECT_EQ(object.at("bool_"),true);

  EXPECT_TRUE(object.contains("int_"));
  EXPECT_EQ(object.at("int_"),1234);

  EXPECT_TRUE(object.contains("double_"));
  EXPECT_FLOAT_EQ(object.at("double_"),12.34);

  EXPECT_TRUE(object.contains("string_"));
  EXPECT_EQ(object.at("string_"),"hello world");

  EXPECT_TRUE(object.contains("intValue_"));
  EXPECT_TRUE(object.at("intValue_").contains("int_"));
  EXPECT_EQ(object.at("intValue_").at("int_"),7890);

  EXPECT_TRUE(object.contains("vec_"));
  EXPECT_EQ(object.at("vec_"), expect_vec);
}