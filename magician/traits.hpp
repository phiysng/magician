#pragma once

#include <cstdlib>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace phi {
using namespace std;

namespace internal {
template<typename T, size_t... Is>
auto serialize_impl(const T &t, index_sequence<Is...>) {
  return vector<int>({get<Is>(t)...});
}
} // namespace internal

template<typename... Ts>
auto serialize(tuple<Ts...> &t) {
  return internal::serialize_impl(t, index_sequence_for<Ts...>{});
}

namespace internal {
template<typename T, size_t... Is>
void deserialize_impl(T &t, vector<int> v, index_sequence<Is...>) {
  ((get<Is>(t) = v[Is]), ...);
}
} // namespace internal

template<typename... Ts>
void deserialize(tuple<Ts...> &t, vector<int> in) {
  constexpr int size_tuple = sizeof...(Ts);
  int size = in.size();
  if (size != size_tuple) {
    abort();
  }
  internal::deserialize_impl(t, in, index_sequence_for<Ts...>{});
}

template<typename... Ts>
struct is_unique_type_list : true_type {
};

///
/// \brief The is_unique_type_list class
///
template<typename T, typename... Ts>
struct is_unique_type_list<T, Ts...> : std::conditional_t<(sizeof...(Ts) > 1),
                                                          conditional_t<(!is_same_v<T, Ts> && ...),
                                                                        is_unique_type_list<Ts...>,
                                                                        false_type>,
                                                          true_type> {
};

}
