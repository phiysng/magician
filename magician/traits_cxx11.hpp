//
// Created by Wu Yuanshou on 2023/1/13.
//

#ifndef MAGICIAN_MAGICIAN_TRAITS_CXX11_HPP
#define MAGICIAN_MAGICIAN_TRAITS_CXX11_HPP
#include <type_traits>
namespace phi {

namespace cxx11 {
using namespace std;

template<typename ...Ts>
struct overloaded;

template<typename Fn>
struct overloaded<Fn> : Fn {
  explicit overloaded(Fn fn) : Fn(fn) {
  }
  using Fn::operator();
};

template<typename Fn, typename ...Rest>
struct overloaded<Fn, Rest...> : Fn, overloaded<Rest...> {

  explicit overloaded(Fn fn, Rest ...rest) : Fn(fn), overloaded<Rest...>(rest...) {
  }

  using Fn::operator();
  using overloaded<Rest...>::operator();
};

template<typename ...Fn>
auto make_overload(Fn... fn) -> overloaded<Fn...> {
  return overloaded<Fn...>(fn...);
}
}
}

#endif //MAGICIAN_MAGICIAN_TRAITS_CXX11_HPP
