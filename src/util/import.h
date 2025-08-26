#pragma once

#include <type_traits>
#include <utility>

namespace util {

template <typename, typename = int>
struct has_subscriptor : std::true_type {};

template <typename T>
struct has_subscriptor<T, decltype((void)T::operator[], 0)> : std::false_type {
};

template <typename T>
class Import {
 public:
  T& get();
  T& operator*() { return get(); }
  T* operator->() { return &get(); }

  template <
      typename = typename std::enable_if<has_subscriptor<T>::value, void>::type,
      typename... Args>
  auto operator[](Args&&... args) {
    return get().operator[](std::forward<Args>(args)...);
  }
};

template <typename T, typename Name>
class ImportNamed {
 public:
  T& get();
  T& operator*() { return get(); }
  T* operator->() { return &get(); }

  template <
      typename = typename std::enable_if<has_subscriptor<T>::value, void>::type,
      typename... Args>
  auto operator[](Args&&... args) {
    return get().operator[](std::forward<Args>(args)...);
  }
};

}  // namespace util
