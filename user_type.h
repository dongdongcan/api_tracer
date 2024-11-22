#pragma once
#include <vector>

#define USER_CLASS_PRE std::string("          ")

// Put your own class type below, and you should override `operator<<` for your class

////////////////////////////////  struct exmple
// simpleStruct
struct simpleStruct {
  int a = 1;
  void *ptr = (void *)0x1234;

  simpleStruct(){};
  simpleStruct(int ia, void *p) : a(ia), ptr(p) {}

  friend std::ostream &operator<<(std::ostream &o, const struct simpleStruct &s) {
    o << "\n";
    o << USER_CLASS_PRE << "simpleStruct {" << std::endl;
    o << USER_CLASS_PRE << "  int a = " << s.a << ";" << std::endl;
    o << USER_CLASS_PRE << "  void *ptr = " << s.ptr << ";" << std::endl;
    o << USER_CLASS_PRE << "}" << std::endl;
    return o;
  }
};

////////////////////////////////  enum exmple
// simpleEnum
enum simpleEnum {
  E0 = 0,
  E1 = 1,
  E2 = 2,
};

static inline std::ostream &operator<<(std::ostream &o, simpleEnum eNum) {
  const auto pre = "simpleEnum::";
  switch (eNum) {
  case E0:
    o << pre << "E0" << std::endl;
    break;
  case E1:
    o << pre << "E1" << std::endl;
    break;
  case E2:
    o << pre << "E2" << std::endl;
    break;
  default:
    o << pre << "Unknown" << std::endl;
    break;
  }
  return o;
}

////////////////////////////////  struct enum