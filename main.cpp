#include <iostream>

#include "ipega.h"

int main() {
  Ipega ipega("/dev/input/js0");
  while (1) {
    ipega.spin();
  }
  return 0;
}