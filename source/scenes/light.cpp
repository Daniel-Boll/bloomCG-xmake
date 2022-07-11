#include <fmt/core.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <bloomCG/core/core.hpp>
#include <bloomCG/core/renderer.hpp>
#include <bloomCG/scenes/light.hpp>
#include <cstddef>

#include "ImGuizmo.h"
#include "bloomCG/core/camera.hpp"

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see
// docs/FONTS.md)
static void HelpMarker(const char* desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

namespace bloom {
  namespace scene {
    enum class ObjectType {
      CUBE,
      SPHERE,
      LIGHT,
    };
    struct Objects {
      ObjectType type;
      std::string name;
      int32_t index;
    };

    float m_lastTime;
    int m_direction;

    bool m_wireframe = false;
    bool m_depthBuffer = true;

    // MODAL
    std::size_t bufferSize = sizeof(char) * 30;
    // Sphere=========
    bool m_modalSphere = false;
    bool m_editing = false;
    char* namePtr;
    glm::vec3 resultPosition;
    float resultRadius;
    // ===============

    // Hierarchy
    std::vector<Objects> hierarchyObjects;
    int8_t selected = -1;

    // TODO: Refactor all objects to be contained within hierarchyObjects.

    Light::Light() : m_translation(0.0f, 0.0f, 0.0f) {
      m_lastTime = 0;
      m_direction = 1;

      GLCall(glad_glEnable(GL_BLEND));
      GLCall(glad_glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

      // ================ Setting up Camera ================
      glm::vec3 position = glm::vec3(0.5f, 3.0f, 17.0f);

      m_camera = std::make_unique<bloom::Camera>(position);

      // Get window width
      // int width, height;
      // glfwGetWindowSize(bloom::gl::getWindow(), &width, &height);
      int width = 1920 / 2, height = 1080 / 2;

      const double fov = 45.0f;
      const double aspect = (float)width / (float)height;
      const double near = 0.1f;
      const double far = 100.0f;

      m_camera->changeCameraType(CameraType::PERSPECTIVE, new double[]{fov, aspect, near, far})
          ->toggleMovement()
          ->toggleMouseMovement()
          ->setCameraSpeed(2.5f)
          ->setCameraSensitivity(.1);
      // ======================================================

      glm::vec3 objectKa = glm::vec3{1.0, 0.0, .0};
      glm::vec3 objectKd = glm::vec3{1.0, 0.0, .0};
      glm::vec3 objectKs = glm::vec3{0.5, 0.5, .5};

      glm::vec4 lightColor = glm::vec4{1., 1., 1., 1.};

      // =================== Elements in the scene ================
      glm::vec3 cubePositions[] = {
          glm::vec3(0.0f, 0.0f, 0.0f),
          // glm::vec3(2.0f, 5.0f, -15.0f),
      };

      // For each cube position create a cube object
      int cubeCount = sizeof(cubePositions) / sizeof(cubePositions[0]);
      for (int i = 0; i < cubeCount; i++)
        m_cubes.push_back(std::make_unique<bloom::Cube>(2.f, cubePositions[i], CubeType::REPEATED));

      m_spheres.push_back(
          std::make_unique<bloom::Sphere>(glm::vec3{0, 0, 0}, .5, m_sectorCount, m_stackCount));
      // m_spheres.push_back(std::make_unique<bloom::Sphere>(glm::vec3{0, 0, 0}, 1., 7, 8));

      // m_cubes[0]->print();

      // TODO: Change this to get CORRECTLY
      m_objectShader = std::make_unique<bloom::Shader>(
          "/home/danielboll/dev/Unioeste/2022/CG/bloomCG-xmake/assets/shaders/cube.shader.glsl");
      m_objectShader->bind();
      m_objectShader->setUniform3f("uMaterial.ambient", objectKa);
      m_objectShader->setUniform3f("uMaterial.diffuse", objectKd);
      m_objectShader->setUniform3f("uMaterial.specular", objectKs);
      m_objectShader->setUniform1f("uMaterial.shininess", 32.f);

      m_objectShader->setUniform1f("uLight.constant", 1.0f);
      m_objectShader->setUniform1f("uLight.linear", .09f);
      m_objectShader->setUniform1f("uLight.quadratic", .032f);

      m_objectShader->unbind();
      // ==========================================================

      // =================== Lights in the scene ================
      glm::vec3 lightPositions[] = {
          glm::vec3(0.5f, 2.1f, -1.0f),
      };
      m_translation = glm::vec3(0.5f, 2.1f, -1.f);

      // Get the vector size
      int lightCount = sizeof(lightPositions) / sizeof(lightPositions[0]);
      for (int i = 0; i < lightCount; i++)
        m_lights.push_back(
            std::make_unique<bloom::Light>(.3f, lightPositions[i], CubeType::REPEATED));

      m_lightShader = std::make_unique<bloom::Shader>(
          "/home/danielboll/dev/Unioeste/2022/CG/bloomCG-xmake/assets/shaders/light.shader.glsl");
      m_lightShader->bind();
      m_lightShader->setUniform4f("uColor", lightColor);
      m_lightShader->unbind();
      // ==========================================================
    }

    void Light::onUpdate(const float deltaTime) {
      m_camera->update(deltaTime);

      for (auto& light : m_lights) light.get()->setPosition(m_translation);

      m_spheres[0]->setSectorCount(m_sectorCount);
      m_spheres[0]->setStackCount(m_stackCount);
    }

    void Light::onRender(const float deltaTime) {
      GLCall(glad_glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
      GLCall(glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
      GLCall(m_depthBuffer ? glad_glEnable(GL_DEPTH_TEST) : glad_glDisable(GL_DEPTH_TEST));

      m_objectShader->bind();
      m_objectShader->setUniformMat4f("uView", m_camera->getViewMatrix());
      m_objectShader->setUniformMat4f("uProjection", m_camera->getProjectionMatrix());
      m_objectShader->setUniform3f("uCameraPosition", m_camera->getPosition());

      m_objectShader->setUniform3f("uLight.ambient", m_lightAmbient);
      m_objectShader->setUniform3f("uLight.diffuse", m_lightDiffuse);
      m_objectShader->setUniform3f("uLight.specular", m_lightSpecular);

      // Print light specular
      uint32_t offset = 0;
      // for (auto& cube : m_cubes) {
      //   glm::vec3 position = cube->getPosition();
      //   glm::mat4 model = glm::mat4(1.0f);
      //   float angle = 20.0f * offset++;
      //
      //   model = glm::translate(model, position);
      //   model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      //   m_objectShader->setUniformMat4f("uModel", model);
      //   m_objectShader->setUniform3f("uLightPosition", m_lights[0].get()->getPosition());
      //   cube->draw();
      // }

      for (auto& sphere : m_spheres) {
        glm::vec3 position = glm::vec3{.0, .0, .0};
        glm::mat4 model = glm::mat4(1.0f);
        float angle = 20.0f * offset++;

        model = glm::translate(model, position);
        m_objectShader->setUniformMat4f("uModel", model);
        m_objectShader->setUniform3f("uLightPosition", m_lights[0].get()->getPosition());
        sphere->draw();
      }

      m_objectShader->unbind();

      m_lightShader->bind();
      m_lightShader->setUniformMat4f("uView", m_camera->getViewMatrix());
      m_lightShader->setUniformMat4f("uProjection", m_camera->getProjectionMatrix());

      // Move the light in a orbit around the center
      m_translation.x = sin(glfwGetTime()) * 5.0f;
      m_translation.z = cos(glfwGetTime()) * 5.0f;
      m_translation.y = sin(glfwGetTime()) * 5.0f;

      // Do the bellow snippet of code every .1 seconds
      if (glfwGetTime() > m_lastTime + .1f) {
        m_lastTime = glfwGetTime();
        // Up until 50 then reverse the direction
        if (m_direction == 1) {
          if (m_sectorCount < 50) {
            m_sectorCount++;
          } else {
            m_direction = -1;
          }
        } else {
          if (m_sectorCount > 1) {
            m_sectorCount--;
          } else {
            m_direction = 1;
          }
        }
      }

      if (m_wireframe) {
        GLCall(glad_glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
      } else {
        GLCall(glad_glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
      }

      for (auto& light : m_lights) {
        light->setPosition(m_translation);
        glm::vec3 position = light->getPosition();
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(model, position);
        m_lightShader->setUniformMat4f("uModel", model);
        light->draw();
      }

      m_lightShader->unbind();
    }

    void Light::inspector() {
      ImGui::Begin("Inspector");

      if (selected == -1) {
        ImGui::Text("Nothing is selected");
        ImGui::End();
        return;
      }

      // Display a input to change the name of the current selected element
      ImGui::Text("%s", hierarchyObjects.at(selected).name.c_str());

      ImGui::End();
    }

    void Light::hierarchy() {
      ImGui::Begin("Hierarchy");

      if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::BeginMenu("Create element")) {
          if (ImGui::MenuItem("Sphere")) {
            m_modalSphere = true;
          }
          if (ImGui::MenuItem("Cube")) {
            hierarchyObjects.emplace_back(
                Objects{ObjectType::CUBE, "Cube", (int32_t)m_cubes.size()});
          }
          if (ImGui::MenuItem("Light")) {
            hierarchyObjects.emplace_back(
                Objects{ObjectType::LIGHT, "Light", (int32_t)m_lights.size()});
          }

          ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scene controllers")) {
          // Checkbox for wireframe
          ImGui::Checkbox("Wireframe", &m_wireframe);
          ImGui::Checkbox("Depth buffer", &m_depthBuffer);
          ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Load .element", "CTRL+L", false, false)) {
        }

        if (ImGui::MenuItem("Save .element", "CTRL+S", false, false)) {
        }

        ImGui::EndPopup();
      }

      if (m_modalSphere == true) ImGui::OpenPopup("add_sphere");

      // Always center this window when appearing
      ImVec2 center = ImGui::GetMainViewport()->GetCenter();
      ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

      if (ImGui::BeginPopupModal("add_sphere", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        addSphere();
        m_modalSphere = false;
      }

      ImGui::Text("Hierarchy");
      ImGui::SameLine();
      HelpMarker(
          "\"Hierarchy\" is a list of all the objects in the scene. Press Right click to add a new "
          "object\n");
      ImGui::Separator();

      {
        for (int32_t i = 0; i < hierarchyObjects.size(); i++) {
          const auto object = hierarchyObjects[i];
          // Get the index of the object in the std::vector

          if (ImGui::Selectable(object.name.c_str(), selected == i)) {
            selected = i;
          }
          if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete")) {
              m_spheres.erase(m_spheres.begin() + hierarchyObjects[i].index);
              hierarchyObjects.erase(hierarchyObjects.begin() + i);
            }
            ImGui::EndPopup();
          }
        }
      }

      ImGui::End();
    }

    void Light::addSphere(std::string* name, glm::vec3* position, float* radius) {
      const std::string repeated = fmt::format("({})", m_spheres.size());
      const std::string sphereName = fmt::format("Sphere{}", m_spheres.size() > 1 ? repeated : "");

      if (!m_editing) {
        std::string resultName = name ? *name : sphereName;
        // std::string to char*
        namePtr = new char[resultName.size() + 1];
        std::copy(resultName.begin(), resultName.end(), namePtr);
        // Create buffer
        resultPosition = position ? *position : glm::vec3(0.0f);
        resultRadius = radius ? *radius : 1.0f;
        m_editing = true;
      }

      ImGui::InputText("Name", namePtr, 64);
      ImGui::Separator();

      ImGui::InputFloat3("Position", &resultPosition[0]);
      ImGui::Separator();

      ImGui::InputFloat("Radius", &resultRadius);
      ImGui::Spacing();

      if (ImGui::Button("OK", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
        m_editing = false;

        hierarchyObjects.emplace_back(
            Objects{ObjectType::SPHERE, namePtr, (int32_t)m_spheres.size()});
        m_spheres.emplace_back(
            std::make_unique<bloom::Sphere>(resultPosition, resultRadius, 30, 30));
      }
      ImGui::SetItemDefaultFocus();
      ImGui::SameLine();
      if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();

      m_modalSphere = false;
    }

    void Light::enableGuizmo() {
      // TODO: add for every object. (only show to the selected one)
      // TODO: add rotation on R
      // TODO: add base ImGuizmo for coordinates
      ImGuizmo::SetOrthographic(false);
      ImGuizmo::SetDrawlist(bloom::Renderer::getViewportDrawList());

      float windowPosX = bloom::Renderer::getViewportX();
      float windowPosY = bloom::Renderer::getViewportY();

      float windowWidth = bloom::Renderer::getViewportWidth();
      float windowHeight = bloom::Renderer::getViewportHeight();

      ImGuizmo::SetRect(windowPosX, windowPosY, windowWidth, windowHeight);

      glm::mat4 model = glm::mat4(1.0f);
      model = glm::translate(model, m_spheres[0].get()->getPosition());

      ImGuizmo::Manipulate(glm::value_ptr(m_camera->getViewMatrix()),
                           glm::value_ptr(m_camera->getProjectionMatrix()),
                           ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::WORLD, glm::value_ptr(model));

      if (ImGuizmo::IsUsing()) {
        glm::vec3 position = glm::vec3(model[3]);
        m_spheres[0].get()->setPosition(position);
      }
    }

    void Light::onImGuiRender() {
      hierarchy();
      inspector();
      enableGuizmo();

      if (ImGui::CollapsingHeader("Object configurations")) {
        ImGui::Checkbox("Wireframe", &m_wireframe);
      }

      if (ImGui::CollapsingHeader("Light configuration")) {
        ImGui::SliderFloat3("Light position", &m_translation.x, -5.0f, 5.0f);

        ImGui::ColorEdit3("Light ambient", &m_lightAmbient.x);
        ImGui::ColorEdit3("Light diffuse", &m_lightDiffuse.x);
        ImGui::ColorEdit3("Light specular", &m_lightSpecular.x);
      }

      if (ImGui::CollapsingHeader("Sphere configuration")) {
        ImGui::InputInt("Sphere sector  count", &m_sectorCount);
        ImGui::InputInt("Sphere stack count", &m_stackCount);
      }

      if (ImGui::CollapsingHeader("Camera configuration")) {
        switch (m_camera->getCameraType()) {
          case bloom::CameraType::PERSPECTIVE: {
            if (ImGui::Button("Change to parallel")) {
              m_camera->changeCameraType(CameraType::AXONOMETRIC, new double[]{});
            }
            break;
          }
          case bloom::CameraType::AXONOMETRIC: {
            if (ImGui::Button("Change to perspective")) {
              int width = 1920 / 2, height = 1080 / 2;

              const double fov = 45.0f;
              const double aspect = (float)width / (float)height;
              const double near = 0.1f;
              const double far = 100.0f;

              m_camera->changeCameraType(CameraType::PERSPECTIVE,
                                         new double[]{fov, aspect, near, far});
            }
            break;
          }
        }
      }

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

      // ImGui::ShowDemoWindow();

      if (ImGui::TreeNode("Selection State: Single Selection")) {
        static int selected = -1;
        for (int n = 0; n < 5; n++) {
          char buf[32];
          sprintf(buf, "Object %d", n);
          if (ImGui::Selectable(buf, selected == n)) selected = n;
        }
        ImGui::TreePop();
      }
    }
  }  // namespace scene
}  // namespace bloom
