#pragma once

#include <array>
#include <ostream>
#include <queue>
#include <vector>

#define PRE_INDENT std::string("          ")

template <typename T>
inline constexpr bool is_fundamental_type = std::is_integral_v<T> || std::is_floating_point_v<T>;

template <typename T>
static inline void printFundamentalType(const T *data, int64_t len, std::ostream &o) {
  static_assert(is_fundamental_type<T>);
  static constexpr int64_t numPerLine = 64;
  auto ceil_div = [](int64_t a, int64_t b) { return (a + b - 1) / b; };
  auto loops = ceil_div(len, numPerLine);
  for (int64_t loopIdx = 0; loopIdx < loops; loopIdx++) {
    auto num = loopIdx == loops - 1 ? len - loopIdx * numPerLine : numPerLine;
    o << PRE_INDENT;
    for (int64_t idx = 0; idx < num; ++idx) {
      o << data[idx] << " ";
    }
    o << std::endl;
  }
}

template <typename T>
static inline void printSpecialType(const T *data, int64_t len, std::ostream &o) {
  static_assert(!is_fundamental_type<T>);
  for (auto i = 0; i < len; i++) {
    o << PRE_INDENT << data[i] << std::endl;
  }
}

template <typename T>
static inline std::ostream &operator<<(std::ostream &o, const std::vector<T> &v) {
  o << "\n";
  if constexpr (is_fundamental_type<T>) {
    printFundamentalType<T>(v.data(), v.size(), o);
  } else {
    printSpecialType<T>(v.data(), v.size(), o);
  }
  return o;
}

template <typename T>
static inline std::ostream &operator<<(std::ostream &o, const std::queue<T> &v) {
  o << "\n";
  auto temp = v;
  while (!temp.empty()) {
    o << PRE_INDENT << temp.front() << std::endl;
    temp.pop();
  }
  return o;
}
