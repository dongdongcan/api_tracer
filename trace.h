#pragma once

#include <cxxabi.h>

#include "stl_type.h"
#include "user_type.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <ostream>
#include <queue>
#include <string>
#include <typeinfo>
#include <vector>

#include <cxxabi.h>
#include <string>
#include <type_traits>
#include <typeinfo>

template <typename T>
static inline std::string getDemangleName() {
  auto processStdPrefix = [](const std::string &typeName) -> std::string {
    if (typeName.rfind("std", 0) == 0) {
      size_t commaPos = typeName.find(',');
      std::string result = (commaPos != std::string::npos) ? typeName.substr(0, commaPos) : typeName;
      int openCount = std::count(result.begin(), result.end(), '<');
      int closeCount = std::count(result.begin(), result.end(), '>');
      if (openCount > closeCount) {
        result.append(openCount - closeCount, '>');
      }
      return result;
    }
    return typeName;
  };

  if constexpr (std::is_same_v<T, std::string>) {
    return "std::string";
  } else {
    int status = 0;
    char *demangled = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
    std::string typeName = (status == 0) ? demangled : typeid(T).name();
    free(demangled);
    return processStdPrefix(typeName);
  }
}

// A struct used to trace a c-style array, such as `int v[3]` or `int v[]`.
// These array must have a `length` read data from address of `v`.
template <typename T>
struct cArrayTracer {
  std::vector<T> _data;
  cArrayTracer(T *data, int64_t len) {
    for (auto idx = 0; idx < len; ++idx) {
      _data.push_back(data[idx]);
    }
  }
  friend std::ostream &operator<<(std::ostream &o, const struct cArrayTracer &array) {
    o << "Wrapped from: array<" << getDemangleName<T>() << ", " << array._data.size() << ">:\n" << std::endl;
    if constexpr (is_fundamental_type<T>) {
      printFundamentalType<T>(array._data.data(), array._data.size(), o);
    } else {
      printSpecialType<T>(array._data.data(), array._data.size(), o);
    }
    return o;
  }
};

template <typename T>
static inline void printParam(int &index, T variableValue, std::string variableName) {
  ++index;
  std::replace(variableName.begin(), variableName.end(), ' ', '\0');
  std::cout << "param " << index << ": " << variableName << "(" << getDemangleName<T>() << ") = ";
  if constexpr (std::is_integral_v<T>) {
    std::cout << variableValue << "(0x" << std::hex << variableValue << ")" << std::dec << std::endl;
  } else if constexpr (std::is_floating_point_v<T> || std::is_enum_v<T> || std::is_class_v<T>) {
    std::cout << variableValue << std::endl;
  } else if constexpr (std::is_pointer_v<T>) {
    std::cout << reinterpret_cast<int64_t>(variableValue) << "(" << variableValue << ")" << std::endl;
  } else {
    std::cout << "Unkown type:" << variableValue << std::endl;
  }
}

template <typename T>
static inline void show_detail(std::queue<std::string> &paramNames, int &index, T paramValue) {
  printParam<T>(index, paramValue, paramNames.front());
  paramNames.pop();
}

template <typename T, typename... Args>
static void showParams(std::queue<std::string> &paramNames, int &index, T head, Args... args) {
  show_detail<T>(paramNames, index, head);
  if constexpr (sizeof...(args) > 0) {
    showParams(paramNames, index, args...);
  }
}

#define TOSTR(a) (#a)
#define API_TRACE(param...)                                                                                            \
  {                                                                                                                    \
    auto paramString = std::string(TOSTR((param)));                                                                    \
    const auto func = __PRETTY_FUNCTION__;                                                                             \
    std::cout << "\n========== Trace API Info Start ==========" << std::endl;                                          \
    std::cout << "[API Call]: " << func << ";\n" << std::endl;                                                         \
    std::queue<std::string> paramName;                                                                                 \
    auto pos = paramString.find("(", 0);                                                                               \
    paramString = paramString.substr(pos + 1);                                                                         \
    for (;;) {                                                                                                         \
      if (std::string::npos == paramString.find(",", 0)) {                                                             \
        break;                                                                                                         \
      } else {                                                                                                         \
        pos = paramString.find(",", 0);                                                                                \
        paramName.push(paramString.substr(0, pos));                                                                    \
        paramString = paramString.substr(pos + 1);                                                                     \
      }                                                                                                                \
    }                                                                                                                  \
    pos = paramString.find(")", 0);                                                                                    \
    paramName.push(paramString.substr(0, pos));                                                                        \
    int paramIndex = 0;                                                                                                \
    showParams(paramName, paramIndex, param);                                                                          \
    std::cout << "========== Trace API Info Done ==========\n" << std::endl;                                           \
  }
