#pragma once
#include <atomic>
#include <iostream>

#define LOG (std::cerr)

class copyable {
protected:
  copyable() = default;
  ~copyable() = default;
};

// 定义一个类，用于在析构函数中打印函数名
class PrintFuncName {
public:
  PrintFuncName(const std::string &name) : m_name(name) {
    // 构造函数中不需要做任何事
    counter++;
    std::string nPlus = std::string(counter.load(), '+');
    std::string nMinus = std::string(level - counter.load(), '-');
    std::cout << nPlus << nMinus << "Entering【" << m_name << " " << counter << "】" << std::endl;
  }

  ~PrintFuncName() {
    // 析构函数中打印函数名
    std::string nPlus = std::string(counter.load(), '+');
    std::string nMinus = std::string(level - counter.load(), '-');
    std::cout << nPlus << nMinus<< "Leaving【" << m_name << " " << counter << "】" << std::endl << std::endl;
    counter--;
  }

  static std::atomic<int> counter;

private:
  std::string m_name; // 函数名
  const int level = 5;
};
