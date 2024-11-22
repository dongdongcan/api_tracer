#include "trace.h"
#include "user_type.h"

void test_value(int vI, float vF, int *iPtr, float *fPtr, simpleEnum eNum) { API_TRACE(vI, vF, iPtr, fPtr, eNum); }

void test_carray(int iA[2], int iB[], int64_t iBLen, simpleStruct sA[2], simpleStruct sB[], int64_t sBLen) {
  auto trace0 = cArrayTracer<int>(iA, 2);
  auto trace1 = cArrayTracer<int>(iB, iBLen);
  auto trace2 = cArrayTracer<simpleStruct>(sA, 2);
  auto trace3 = cArrayTracer<simpleStruct>(sB, sBLen);
  API_TRACE(trace0, trace1, trace2, trace3);
}

void test_stl(std::vector<int> iVector, std::vector<simpleStruct> sVector, std::queue<std::string> sQueue,
              std::string str) {
  API_TRACE(iVector, sVector, sQueue, str);
}

void test_mstl(std::vector<std::vector<int>> ivVector) { API_TRACE(ivVector); }

int main() {
  // test simple data type
  int vI = 12;
  float vF = 12.34;
  int *iPtr = (int *)0x1234;
  float *fPtr = (float *)0xF123;
  simpleEnum eNum = simpleEnum::E1;
  test_value(vI, vF, iPtr, fPtr, eNum);

  // test c-style array
  int iA[2] = {1, 2};
  int iB[] = {1, 2, 3, 4, 5};
  auto vStruct0 = simpleStruct();
  auto vStruct1 = simpleStruct(11, (void *)0x999);
  simpleStruct sA[2] = {vStruct0, vStruct1};
  simpleStruct sB[] = {vStruct0, vStruct1, vStruct0, vStruct1};
  test_carray(iA, iB, sizeof(iB) / sizeof(iB[0]), sA, sB, sizeof(sB) / sizeof(sB[0]));

  // test stl
  std::vector<int> iVector{1, 2, 3, 4};
  std::vector<simpleStruct> sVector{vStruct0, vStruct0, vStruct0, vStruct0};
  std::queue<std::string> sQueue;
  sQueue.push("hello world");
  std::string str = "BigBang!";
  test_stl(iVector, sVector, sQueue, str);

  // test multi-stl
  std::vector<std::vector<int>> ivVector{
      {
          1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11,
          12, 13, 14, 15, 16, 1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 1,  2,  3,  4,  5,  6,
          7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
      },
      {5, 6, 7, 8}};
  test_mstl(ivVector);
  return 0;
}
