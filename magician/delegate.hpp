#pragma once

#include <type_traits>

namespace phi {
using namespace std;

///
/// \brief The Delegate class
/// TODO:
/// 1. support free function , static member function , member function,
/// functor(lambda)
///
/// 2. support rvaule
///
/// 3. async function call
///
/// 4. smart pointer support
///
/// Design Decision:
/// 1. support functor mutable?
/// TODO: loose signature for reference.
///
template<typename R, typename... Args>
struct delegate {

  using callback_type = function<R(Args...)>;
  using free_function_type = R(Args...);
  vector<callback_type> callbacks;

  vector<tuple<Args...>> Params;

  // introduce a new typename to bring in type deduction, for which we can call int(*)(const int a, const vector<int>& b)
  // with signature of int(int,vector<int>);
  template<typename F>
  void bind(F func_type) {
    callback_type t = [=](Args... args) -> R { return func_type(args...); };
    callbacks.push_back(t);
  }

  template<typename CtxType, typename free_function_type_with_ctx = R(CtxType, Args...)>
  void bind(int x, free_function_type_with_ctx func_type) {
    callback_type t = [=](Args... args) -> R { return func_type(x, args...); };
    callbacks.push_back(t);
  }

  template<typename T, typename callback_member_type = R (T::*)(Args...)>
  void bind(T *ptr, callback_member_type mem_type) {
    callback_type t = [=](Args... args) -> R {
      return invoke(mem_type, ptr, args...);
    };
    callbacks.push_back((t));
  }

  template<typename T, typename callback_member_type = R (T::*)(Args...)>
  void bind(weak_ptr<T> ptr, callback_member_type mem_type) {

    callback_type t = [=](Args... args) -> R {
      auto real_ptr = ptr.lock();
      // TODO: signature change to void(T::*)(Args...)
      if (!real_ptr)
        return R();
      return invoke(mem_type, real_ptr, args...);
    };
    callbacks.push_back((t));
  }

  template<typename T, typename callback_member_type = R (T::*)(Args...)>
  void bind(shared_ptr<T> ptr, callback_member_type mem_type) {
    // closure capture and increase the reference count , so we dont need to
    // check the whether pointer is valid.
    callback_type t = [=](Args... args) -> R {
      return invoke(mem_type, ptr, args...);
    };
    callbacks.push_back((t));
  }

  // functor or lambda
  template<typename T,
      typename = enable_if_t<disjunction_v<
          is_same<decltype(&T::operator()), R (T::*)(Args...)>,
          is_same<decltype(&T::operator()), R (T::*)(Args...) const>>>>
  void bind(T functor) {
    callback_type t = [=](Args... args) mutable -> R {
      return (functor)(args...);
    };
    callbacks.push_back((t));
  }

  void emit(Args &&... args) {
    for (auto &cb : callbacks) {
      cb(std::forward<Args>(args)...);
    }
  }

  void emit(const Args &... args) {
    for (auto &cb : callbacks) {
      cb((args)...);
    }
  }

  // copy all param for async emit.
  // WARNING: use with caution , may trigger heavy copy operation.
  void emit_async(Args... args) { Params.emplace_back(args...); }

  void process() {
    for (auto &param : Params) {
      for (auto &cb : callbacks) {
        apply(cb, param);
      }
    }
  }
};

}
