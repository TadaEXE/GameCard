#pragma once

#include <cstddef>
#include <cstdint>

namespace util::span {

/// @brief Non-owning span over memory
template <typename T>
class Span {
 public:
  constexpr Span() noexcept : data(nullptr), size(0) {}
  constexpr Span(T* data, size_t size) : data(data), size(size) {}
  constexpr Span(T* first, T* last) : data(first), size(last - first) {}

  constexpr T& operator[](size_t i) const noexcept { return data[i]; }
  constexpr T& at(size_t i) const noexcept { return data[i]; }
  constexpr T& front() const noexcept { return data[0]; }
  constexpr T& back() const noexcept { return data[size - 1]; }

  constexpr T* begin() const noexcept { return data; }
  constexpr T* end() const noexcept { return data + size; }
  constexpr const T* cbegin() const noexcept { return data; }
  constexpr const T* cend() const noexcept { return data + size; }

  constexpr Span<T> subspan(size_t offset, size_t count) const noexcept {
    return {data + offset, count};
  }

 private:
  T* data;
  size_t size;
};

using ByteSpan = Span<uint8_t>;
using CByteSpan = Span<const uint8_t>;
}  // namespace util::span
