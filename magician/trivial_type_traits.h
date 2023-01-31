//
// Created by wyshou on 2023/1/31.
//

#ifndef MAGICIAN_MAGICIAN_TRIVIAL_TYPE_TRAITS_H
#define MAGICIAN_MAGICIAN_TRIVIAL_TYPE_TRAITS_H

#include <cstddef>
#include <utility>
#include <type_traits>
#include <tuple>

/**
 * trivial implementation of type traits in C++ standard.
 */
namespace phi {
using namespace std;

template<size_t... Is>
struct index_sequence {
  static constexpr size_t size = sizeof...(Is);
};
namespace impl {
template<size_t I, size_t... Is>
struct index_sequence_helper : index_sequence_helper<I - 1U, I - 1U, Is...> {
};

template<size_t... Is>
struct index_sequence_helper<0U, Is...> {
  using type = index_sequence<Is...>;
};
}

template<size_t N>
using make_index_sequence = typename impl::index_sequence_helper<N>::type;

namespace impl {
// helper function to expand the tuple
template<typename F, typename Tuple, size_t... Is>
auto apply_impl(F &&f, Tuple &&t, phi::index_sequence<Is...>) {
  return invoke(std::forward<F>(f), get<Is>(std::forward<Tuple>(t))...);
}
} // namespace impl

template<typename F, typename Tuple>
auto apply(F &&f, Tuple &&t) {
  return impl::apply_impl(
      std::forward<F>(f), std::forward<Tuple>(t),
      phi::make_index_sequence<
          tuple_size<typename remove_reference<Tuple>::type>::value>{});
}
}

#endif //MAGICIAN_MAGICIAN_TRIVIAL_TYPE_TRAITS_H
