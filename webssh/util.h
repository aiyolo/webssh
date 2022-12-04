#pragma once
#include <iostream>

#define LOG (std::cout)

class copyable {
 protected:
  copyable() = default;
  ~copyable() = default;
};

