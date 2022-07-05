#pragma once

#include <bloomCG/core/common.hpp>

#define ASSERT(x)                                         \
  if (!(x)) {                                             \
    fmt::print("[{}]:{} @ {}\n", #x, __LINE__, __FILE__); \
    asm volatile("int3;");                                \
  }

#define GLCall(x)          \
  bloom::gl::clearError(); \
  x;                       \
  ASSERT(bloom::gl::logCall());

namespace bloom {
  class gl {
  private:
    static GLFWwindow* window;

  public:
    static void clearError();
    static bool logCall();

    static void setWindow(GLFWwindow* window);
    static GLFWwindow* getWindow();
  };
}  // namespace bloom
