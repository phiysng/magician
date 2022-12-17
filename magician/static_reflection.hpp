//
// Created by wyshou on 2022/12/17.
//

#pragma once

#include <tuple>
#include <type_traits>
#include <nlohmann/json.hpp>
#include <magician/traits.hpp>

namespace phi {

namespace detail {

template<typename Fn, typename Tuple, std::size_t... I>
inline constexpr void ForEachTuple(Tuple &&tuple,
                                   Fn &&fn,
                                   std::index_sequence<I...>) {
  (fn(std::get<I>(std::forward<Tuple>(tuple))), ...);
}

template<typename Fn, typename Tuple>
inline constexpr void ForEachTuple(Tuple &&tuple, Fn &&fn) {
  ForEachTuple(
      std::forward<Tuple>(tuple), std::forward<Fn>(fn),
      std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}

template<typename T>
struct is_field_pointer : std::false_type {};

template<typename C, typename T>
struct is_field_pointer<T C::*> : std::true_type {};

template<typename T>
constexpr auto is_field_pointer_v = is_field_pointer<T>::value;

}  // namespace detail

template<typename T>
inline constexpr auto StructSchema() {
  return std::make_tuple();
}

/**
 * namespcae issue, we are hard encoding namespcae in this macro (namespace phi)
 */
#define DEFINE_STRUCT_SCHEMA(Struct, ...)        \
  template <>                                    \
  inline constexpr auto phi::StructSchema<Struct>() { \
    using _Struct = Struct;                      \
    return std::make_tuple(__VA_ARGS__);         \
  }

#define DEFINE_STRUCT_FIELD(StructField, FieldName) \
  std::make_tuple(&_Struct::StructField, #FieldName, #StructField)

template<typename T, typename Fn>
inline constexpr void ForEachField(T &&value, Fn &&fn) {
  constexpr auto struct_schema = StructSchema<std::decay_t<T>>();
  static_assert(std::tuple_size<decltype(struct_schema)>::value != 0,
                "StructSchema<T>() for type T should be specialized to return "
                "FieldSchema tuples, like ((&T::field, field_name), ...)");

  detail::ForEachTuple(struct_schema, [&value, &fn](auto &&field_schema) {
    using FieldSchema = std::decay_t<decltype(field_schema)>;
    static_assert(
        std::tuple_size<FieldSchema>::value == 3 &&
            detail::is_field_pointer_v<std::tuple_element_t<0, FieldSchema>>,
        "FieldSchema tuple should be (&T::field, field_name)");

    // call member field by operator.*
    fn(value.*(std::get<0>(std::forward<decltype(field_schema)>(field_schema))),
       std::get<1>(std::forward<decltype(field_schema)>(field_schema)),
       std::get<2>(std::forward<decltype(field_schema)>(field_schema))
    );
  });
}

namespace internal {
template<typename T>
struct is_optional : std::false_type {};

template<typename T>
struct is_optional<std::unique_ptr<T>> : std::true_type {};

template<typename T>
constexpr bool is_optional_v = is_optional<std::decay_t<T>>::value;

template<typename T>
constexpr bool has_schema = std::tuple_size<decltype(StructSchema<T>())>::value;
}

template<typename T>
void serialize(nlohmann::json &source, T &target) {

  ForEachField(target, [&source](auto &field, auto &&type, auto &&varname/* 字段名 */) {

    // 不是数组,且json对象不包含对应kv,则不处理这个结构体字段
    if (!source.is_array() && !source.contains(varname))
      return;

    // 复合类型
    if constexpr (internal::has_schema<std::decay_t<decltype(field)>>) {
      constexpr auto struct_schema = StructSchema<std::decay_t<decltype(field)>>();
      // 支持的类型
      if (std::tuple_size<decltype(struct_schema)>::value > 0) {
        auto ptr = source.find(varname);
        if (ptr->is_object()) {
          serialize(*ptr, field);
        }
      }
      return;
    }

    auto &&ptr = source.find(varname);

    if constexpr (phi::vector_traits::is_vector_v<decay_t<decltype(field)>>) {
      if (ptr->is_array()) {
        auto size = ptr->size();
        field.resize(size);
        for (int i = 0; i < size; ++i) {
          auto arrayItem = (*ptr)[i];
          using type = typename phi::vector_traits::is_vector<decay_t<decltype(field)>>::type;
          if constexpr (is_same_v<int, decay_t<type>>) {
            type val = arrayItem;
            field[i] = val;
          }
        }
      }
    }

    if constexpr (is_same_v<bool, decay_t<decltype(field) >>) {
      if (ptr->is_boolean()) {
        field = *ptr;
      }
    }
    if constexpr (is_same_v<int, decay_t<decltype(field)>>) {
      if (ptr->is_number()) {
        field = *ptr;
      }
    }
    if constexpr (is_same_v<double, decay_t<decltype(field)>>) {
      if (ptr->is_number()) {
        field = *ptr;
      }
    }
    if constexpr (is_same_v<string, decay_t<decltype(field)>>) {
      if (ptr->is_string()) {
        field = *ptr;
      }
    }
  });
}

template<typename T>
void deserialize(T &source, nlohmann::json &target) {
  ForEachField(source, [&target](auto &&field, auto &&type, auto &&varname/* 字段名 */) {
    if constexpr (is_same_v<decay_t<decltype(field)>, int>) {
      target[varname] = field;
    } else if constexpr(is_same_v<decay_t<decltype(field)>, bool>) {
      target[varname] = field;
    } else if constexpr (is_same_v<decay_t<decltype(field)>, double>) {
      target[varname] = field;
    } else if constexpr(is_same_v<decay_t<decltype(field)>, string>) {
      target[varname] = field;
    } else if constexpr(phi::vector_traits::is_vector_v<decay_t<decltype(field)>>) { // 数组转换为JSON List
      target[varname] = {};
      auto &&array = target[varname];
      int size_of_list = field.size();
      for (int val : field) {
        array.push_back(val);
      }
    }
    else {
      // 复合类型
      constexpr auto struct_schema = StructSchema<std::decay_t<decltype(field)>>();
      if(std::tuple_size<decltype(struct_schema)>::value > 0){
        nlohmann::json j;
       // 递归调用解析这个键
        deserialize(field,j);
        target[varname] = j;
      }
    }
  });
}

}
