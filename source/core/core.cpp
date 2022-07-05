#include <bloomCG/core/core.hpp>

namespace bloom {
  GLFWwindow* gl::window;

  void gl::clearError() {
    while (glGetError() != GL_NO_ERROR)
      ;
  }

  bool gl::logCall() {
    while (GLenum error = glGetError()) {
      fmt::print("OpenGL error: 0x{:04x}\n", error);
      return false;
    }

    return true;
  }

  void gl::setWindow(GLFWwindow* window) { gl::window = window; }
  GLFWwindow* gl::getWindow() { return gl::window; }
}  // namespace bloom
