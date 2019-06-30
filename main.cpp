#include "LeakTester.h"
#include "gc_pointer.h"

template <int size = 0>
Pointer<int, size> testCopy(Pointer<int, size> p) {
  return p;
}

int main() {
  std::cout << "start to test Pointer" << std::endl;
  Pointer<int> p = new int(19);
  p = new int(21);
  p = new int(28);
  std::cout << "new Pointer passed" << std::endl;

  std::cout << "start to test array" << std::endl;
  Pointer<int, 2> p2 = new int[2];
  p2[1] = 1;
  if (p2[1] != 1) {
    std::cout << "the operator [] is not right" << std::endl;
    return 0;
  }
  p2 = new int[2];
  p2[0] = 2;
  if (p2[0] != 2) {
    std::cout << "the operator [] is not right" << std::endl;
    return 0;
  }
  std::cout << "test array passed" << std::endl;

  std::cout << "start to test operator=(Pointer)" << std::endl;
  Pointer<int> p3 = p;
  Pointer<int, 2> p4 = p2;

  p4[0] = 4;
  if (p4[0] != 4) {
    std::cout << "the operator [] is not right" << std::endl;
    return 0;
  }
  std::cout << "operator=(Pointer) passed" << std::endl;

  std::cout << "start to test copy constructor" << std::endl;
  Pointer<int> p5 = testCopy(p);
  Pointer<int, 2> p6 = testCopy(p2);
  p6[1] = 6;
  if (p6[1] != 6) {
    std::cout << "the operator [] is not right" << std::endl;
    return 0;
  }
  std::cout << "copy constructor passed" << std::endl;

  return 0;
}