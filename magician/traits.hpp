#pragma once

#include <cstdlib>
#include <nlohmann/json.hpp>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
namespace phi {
using namespace std;

namespace internal {
template <typename T,typename Fn , size_t... Is>
auto traverse_impl(const T &t,Fn& fn ,index_sequence<Is...>) {
  return vector<int>({get<Is>(t)...});
}
} // namespace internal

template <typename Fn , typename... Ts>
auto traverse(tuple<Ts...> &t,Fn& fn) {
  return internal::traverse_impl(t, fn, index_sequence_for<Ts...>{});
}

namespace internal {
template <typename T, size_t... Is>
auto serialize_impl(const T &t, index_sequence<Is...>) {
  return vector<int>({get<Is>(t)...});
}
} // namespace internal

template <typename... Ts> auto serialize(tuple<Ts...> &t) {
  return internal::serialize_impl(t, index_sequence_for<Ts...>{});
}

namespace internal {
template <typename T, size_t... Is>
void deserialize_impl(T &t, vector<int> v, index_sequence<Is...>) {
  ((get<Is>(t) = v[Is]), ...);
}
} // namespace internal

template <typename... Ts> void deserialize(tuple<Ts...> &t, vector<int> in) {
  constexpr int size_tuple = sizeof...(Ts);
  int size = in.size();
  if (size != size_tuple) {
    abort();
  }
  internal::deserialize_impl(t, in, index_sequence_for<Ts...>{});
}

namespace experimental {
template<typename T, size_t... Is>
void serialize_impl(T &t, nlohmann::json &object, index_sequence<Is...>) {
  ((object[to_string(Is)] = get<Is>(t)), ...);
}

template<typename... Ts>
string serialize(tuple<Ts...> &t) {
  nlohmann::json object;
  serialize_impl(t, object, index_sequence_for<Ts...>{});
  return object.dump();
}


template<typename T, size_t... Is>
void deserialize_impl(T &t, string_view sw, index_sequence<Is...>) {
  auto object = nlohmann::json::parse(sw);
  ((get<Is>(t) = object[to_string(Is)]), ...);
}

template<typename... Ts>
void deserialize(tuple<Ts...> &t, string_view sw) {
  constexpr int size_tuple = sizeof...(Ts);
  deserialize_impl(t, sw, index_sequence_for<Ts...>{});
}
}  // namespace experimental

/// define a primary template to handle the default situation.
template <typename... Ts> struct is_unique_type_list : true_type {};

/// specialized version to handle type list with at least one type.
template <typename T, typename... Ts>
struct is_unique_type_list<T, Ts...>
    : std::conditional_t<(sizeof...(Ts) > 1),
                         conditional_t<(!is_same_v<T, Ts> && ...),
                                       is_unique_type_list<Ts...>, false_type>,
                         true_type> {};

namespace vector_traits {

template<typename T>
struct remove_vector {
};

template<typename T>
struct remove_vector<vector<T>> {
  using type = T;
};

template<typename T, typename = void>
struct is_vector : false_type {};

template<typename T>
struct is_vector<T, void_t<typename remove_vector<T>::type>> : true_type {
  using type = typename remove_vector<T>::type;
};

template<typename T>
constexpr bool is_vector_v = is_vector<T>::value;

}

template<typename ...Ts>
struct overloaded : Ts ... {
  using Ts::operator()...;
};

// non-optimal solution
namespace deprecated {
template<typename ...Ts>
auto make_overloaded(Ts... ts) {
  return overloaded<Ts...>{ts...};
}
}

// see https://en.cppreference.com/w/cpp/language/class_template_argument_deduction for more details.
template<typename ...Ts>
overloaded(Ts...) -> overloaded<Ts...>;

template<typename T>
struct StatusNode{
  T t;

protected:
  void _addState(T _t){
    t = _t;
  }
};

template<typename ...Ts>
struct StatusManager : StatusNode<Ts> ... {

  template<typename T>
  void addState(T t) {
    StatusNode<T>::_addState(t);
  }
};

namespace util {

namespace impl {
// helper function to expand the tuple
template <typename F, typename Tuple, size_t... Is>
auto apply_impl(F &&f, Tuple &&t, index_sequence<Is...>) {
  return invoke(std::forward<F>(f), get<Is>(t)...);
}
} // namespace impl

template <typename F, typename Tuple>
auto apply(F &&f, Tuple &&t) {
  return impl::apply_impl(
      std::forward<F>(f), std::forward<Tuple>(t),
      make_index_sequence<
          tuple_size<typename remove_reference<Tuple>::type>::value>{});
}
} // namespace util
} // namespace phi
