/**
UNDERSCORE CPP Project
Unit tests.
*/

#include "underscore_test.h"
#include "old_syntax_test.h"

int main() {
  std::cout << "Start" << std::endl;
  run_underscore_test();
  std::cout << "Stop" << std::endl;
  old_syntax_test();
  std::cout << std::endl << "Finished!" << std::endl;
}

