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
#include <regex>
#include <string>
#include <type_traits>
#include <typeinfo>

#include <iostream>
#include <string>

std::string adjustBrackets(const std::string &input) {
  int leftCount = 0;
  int rightCount = 0;

  for (char c : input) {
    if (c == '<') {
      leftCount++;
    } else if (c == '>') {
      rightCount++;
    }
  }

  std::string result = input;
  if (rightCount > leftCount) {
    int extra = rightCount - leftCount;
    for (auto it = result.rbegin(); it != result.rend() && extra > 0; ++it) {
      if (*it == '>') {
        *it = '\0';
        extra--;
      }
    }
    result.erase(std::remove(result.begin(), result.end(), '\0'), result.end());
  }
  return result;
}

template <typename T>
static inline std::string getDemangleName() {
  auto processStdPrefix = [](const std::string &typeName) -> std::string {
    std::string result = typeName;
    if (typeName.rfind("std", 0) == 0) {
      // delete " "
      result = std::regex_replace(result, std::regex(" "), "");
      // delete std::__cxx11
      result = std::regex_replace(result, std::regex("std::__cxx.*?::"), "std::");
      // delete std::allocator<>
      result = std::regex_replace(result, std::regex(",std::allocator<.*?>"), "");
      // delete "std::char_traits"
      result = std::regex_replace(result, std::regex(",std::char_traits<.*?>"), "");
      // delete std::deque<*>
      result = std::regex_replace(result, std::regex(",std::deque<.*?>"), "");
      // delete ul, UL used in std::array
      result = std::regex_replace(result, std::regex("ul"), "");
      result = std::regex_replace(result, std::regex("UL"), "");
      // basic_string -> string
      result = std::regex_replace(result, std::regex("basic_string"), "string");
      // std::string<char> -> std::string
      result = std::regex_replace(result, std::regex("std::string<.*?>"), "std::string");

      // "," -> ", "
      result = std::regex_replace(result, std::regex(","), ", ");
    }
    return adjustBrackets(result);
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
