#pragma once

#include <imgui.h>

namespace bloom {
  class Renderer {
    // STATIC METHODS
  private:
    static ImDrawList* s_viewportDrawList;
    static float s_viewportWidth;
    static float s_viewportHeight;
    static float s_viewportX;
    static float s_viewportY;

  public:
    // Draw list
    static void setViewportDrawList(ImDrawList* drawList) {
      Renderer::s_viewportDrawList = drawList;
    }
    static ImDrawList* getViewportDrawList() { return Renderer::s_viewportDrawList; }

    // Viewport width and height
    static void setViewportSize(float width, float height) {
      Renderer::s_viewportWidth = width;
      Renderer::s_viewportHeight = height;
    }
    static float getViewportWidth() { return Renderer::s_viewportWidth; }
    static float getViewportHeight() { return Renderer::s_viewportHeight; }

    // Viewport position
    static void setViewportPosition(float x, float y) {
      Renderer::s_viewportX = x;
      Renderer::s_viewportY = y;
    }
    static float getViewportX() { return Renderer::s_viewportX; }
    static float getViewportY() { return Renderer::s_viewportY; }

    // METHODS
  public:
    Renderer() = default;
    ~Renderer() = default;

    // Clear
    void clear() const;
  };
}  // namespace bloom
