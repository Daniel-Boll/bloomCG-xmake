#pragma once

#include <bloomCG/core/common.hpp>
#include <tuple>

namespace bloom {
  class Input {
  public:
    // ======== Keys ========
    static int m_keyPressed;

    static bool isKeyPressed(const int &key) { return Input::m_keyPressed == key; }

    static void setKeyPressed(const int &key) { Input::m_keyPressed = key; }

    // ======== Mouse ========
    static double m_lastMouseX;
    static double m_lastMouseY;
    static double m_mouseX;
    static double m_mouseY;

    static bool getDiffMousePosition() {
      double diffX = (m_mouseX - m_lastMouseX);
      double diffY = (m_lastMouseY - m_mouseY);

      return ((diffX >= -1 && diffX <= 1) && (diffY >= -1 && diffY <= 1));
    }

    static std::tuple<double, double> getMousePosition() {
      if (getDiffMousePosition()) {
        return std::make_tuple(0, 0);
      }

      return std::make_tuple(Input::m_mouseX - Input::m_lastMouseX,
                             Input::m_lastMouseY - Input::m_mouseY);
    }

    static void setMousePosition(const double &x, const double &y) {
      Input::m_lastMouseX = Input::m_mouseX;
      Input::m_lastMouseY = Input::m_mouseY;

      Input::m_mouseX = x;
      Input::m_mouseY = y;
    }

    // ======== Mouse Buttons ========
    enum class MouseButton { LEFT, MIDDLE, RIGHT };

    static bool m_mouseButtonPressed[3];

    static bool isMouseButtonPressed(const MouseButton &button) {
      return Input::m_mouseButtonPressed[static_cast<int>(button)];
    }

    static void setMouseButtonPressed(const MouseButton &button, const bool &pressed) {
      Input::m_mouseButtonPressed[static_cast<int>(button)] = pressed;
    }
  };
}  // namespace bloom
