#include <iostream>

#include "ipega.h"

/******************************************************************************/
int main() {
  Ipega ipega("/dev/input/js0");

  while (1) {
    ipega.printState();
    std::this_thread::sleep_for(std::chrono::milliseconds(40));
  }

  return 0;
}
/******************************************************************************/
