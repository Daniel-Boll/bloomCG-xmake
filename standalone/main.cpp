#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <bloomCG/core/core.hpp>
#include <bloomCG/core/input.hpp>
#include <bloomCG/core/renderer.hpp>
#include <bloomCG/scenes/light.hpp>
#include <bloomCG/scenes/scene.hpp>
#include <cstdint>

#include "GLFW/glfw3.h"
#include "ImGuizmo.h"

const double ratio = 1.3;
const GLuint WIDTH = 1920 / ratio, HEIGHT = 1080 / ratio;

// Forward declaration
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void errorCallback(int error, const char* description);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

int main(void) {
  GLFWwindow* window;

  glfwSetErrorCallback(errorCallback);
  if (!glfwInit()) return -1;

  // Get monitors
  int count;
  GLFWmonitor** monitors = glfwGetMonitors(&count);

  // Create map for monitor name and index
  std::unordered_map<std::string, int> monitor_map;

  // Loop through monitors
  for (int i = 0; i < count; i++) {
    // Get monitor name
    const char* name = glfwGetMonitorName(monitors[i]);

    // Get monitor resolution
    int width, height;
    glfwGetMonitorPhysicalSize(monitors[i], &width, &height);

    // Get monitor refresh rate
    int refreshRate;
    float _width, _height;
    glfwGetMonitorContentScale(monitors[i], &_width, &_height);

    // Get monitor video mode
    const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);

    // Print monitor info
    fmt::print("Monitor {}: {}\n", i, name);
    fmt::print("Resolution: {}x{}\n", width, height);
    fmt::print("Refresh rate: {}\n", mode->refreshRate);
    fmt::print("Content scale: {}x{}\n", _width, _height);
    fmt::print("\n");

    // Add monitor to map
    monitor_map[name] = i;
  }

  // Attach window to monitor "eDP"
  window = glfwCreateWindow(WIDTH, HEIGHT, "BloomCG", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfwTerminate();
    return -1;
  }

  GLCall(glad_glViewport(0, 0, WIDTH, HEIGHT));
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSwapInterval(1);

  glfwSetKeyCallback(window, keyCallback);
  glfwSetCursorPosCallback(window, mouseCallback);
  glfwSetMouseButtonCallback(window, mouseButtonCallback);

  bloom::gl::setWindow(window);
  // ==============================================================================================

  std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

  bloom::Renderer renderer;
  // ====== Extract this ======
  // Render opengl within the imgui window
  uint32_t fbo;
  GLCall(glGenFramebuffers(1, &fbo));
  GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));

  uint32_t tex;
  GLCall(glGenTextures(1, &tex));
  GLCall(glBindTexture(GL_TEXTURE_2D, tex));
  GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

  unsigned int rbo;
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(
      GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH,
      HEIGHT);  // use a single renderbuffer object for both a depth AND stencil buffer.
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                            rbo);  // now actually attach it

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    fmt::print("Framebuffer not complete!\n");
  // ==========================

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  const char* glsl_version = "#version 330";
  ImGui_ImplOpenGL3_Init(glsl_version);

  bloom::scene::Scene* currentScene = nullptr;
  bloom::scene::Menu* menu = new bloom::scene::Menu(currentScene);
  currentScene = menu;
  menu->registerScene<bloom::scene::Light>("Testing lights");

  GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
  while (!glfwWindowShouldClose(window)) {
    renderer.clear();
    glfwPollEvents();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

    ImGui::Begin("ViewPort");
    {
      // Using a Child allow to fill all the space of the window.
      // It also allows customization
      ImGui::BeginChild("GameRender");
      {
        // Get the size of the child (i.e. the whole draw size of the windows).
        ImVec2 wsize = ImGui::GetWindowSize();
        // Because I use the texture from OpenGL, I need to invert the V from the UV.
        ImGui::Image((void*)(intptr_t)tex, wsize, ImVec2(0, 1), ImVec2(1, 0));

        // Get this viewport in ImGui
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImDrawList* viewportDrawList = ImGui::GetWindowDrawList();

        // Get the position of the viewport
        ImVec2 viewportPos = ImGui::GetWindowPos();

        // Get this window width and height.
        bloom::Renderer::setViewportDrawList(viewportDrawList);
        bloom::Renderer::setViewportSize(wsize.x, wsize.y);
        bloom::Renderer::setViewportPosition(viewportPos.x, viewportPos.y);
      }
      ImGui::EndChild();
    }
    ImGui::End();

    if (currentScene) {
      GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
      GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0));
      currentScene->onSceneRender();

      ImGui::Begin("BloomGL");
      {
        if (currentScene != menu && ImGui::ArrowButton("##left", ImGuiDir_Left)) {
          delete currentScene;
          currentScene = menu;
        }

        GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        currentScene->onImGuiRender();
        renderer.clear();
      }
      ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDeleteFramebuffers(1, &fbo);

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
  action == GLFW_PRESS ? bloom::Input::setKeyPressed(key) : bloom::Input::setKeyPressed(-1);

  if (action == GLFW_PRESS)
    bloom::Input::setKeyPressed(key);
  else if (action == GLFW_REPEAT)
    bloom::Input::setKeyPressed(key);
  else
    bloom::Input::setKeyPressed(-1);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
  bloom::Input::setMousePosition(xpos, ypos);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_RIGHT)
    bloom::Input::setMouseButtonPressed(bloom::Input::MouseButton::RIGHT, action);
  else if (button == GLFW_MOUSE_BUTTON_LEFT)
    bloom::Input::setMouseButtonPressed(bloom::Input::MouseButton::LEFT, action);
  else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    bloom::Input::setMouseButtonPressed(bloom::Input::MouseButton::MIDDLE, action);
}

void errorCallback(int error, const char* description) { fmt::print("Error: {}\n", description); }

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  GLCall(glad_glViewport(0, 0, width, height));
}
