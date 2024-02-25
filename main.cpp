#include <iostream>
#include <thread>

#include "ipega.h"

/******************************************************************************/
int main() {
  Ipega ipega("/dev/input/js0");

  std::thread tele_thread([&] {
    while (1) {
      ipega.printState();
      std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
  });

  while (1) {
    ipega.spin();
  }

  return 0;
}
/******************************************************************************/
