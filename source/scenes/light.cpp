#include <fmt/core.h>
#include <fmt/ostream.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <bloomCG/core/camera.hpp>
#include <bloomCG/core/core.hpp>
#include <bloomCG/core/renderer.hpp>
#include <bloomCG/scenes/light.hpp>
#include <cstddef>

#include "ImGuizmo.h"

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

// Each key represents the type of the constructor of the class
enum class ObjectType { CUBE, SPHERE, LIGHT, CAMERA };
struct Objects {
  ObjectType type;
  std::string name;
  int32_t index;

  union Object {
    bloom::Sphere* sphere;
    bloom::Cube* cube;
    bloom::Light* light;
    bloom::Camera* camera;
  } object;

  bloom::Model* get() {
    // Check for the current type of the struct, then return the object
    switch (type) {
      case ObjectType::CUBE:
        return object.cube;
      case ObjectType::SPHERE:
        return object.sphere;
      case ObjectType::LIGHT:
        return object.light;
      case ObjectType::CAMERA:
        return object.camera;
    }
  }
};

std::ostream& operator<<(std::ostream& os, const Objects& object) {
  os << "Object: " << object.name << " (" << object.index << ")";
  return os;
}

namespace bloom {
  namespace scene {
    // Hierarchy
    std::vector<Objects> hierarchyObjects;
    int8_t selected = -1;

    // Get reference of the object by type
    template <ObjectType T> Objects& getObjectByTypeRef(int32_t index) {
      // Retrieve the index of the object in the hierarchy that matches the type and index
      std::for_each(hierarchyObjects.begin(), hierarchyObjects.end(), [&index](Objects& object) {
        if (object.type == T && object.index == index) {
          index = object.index;
        }
      });

      // Return the object
      return hierarchyObjects[index];
    }

    // Get all objects of the hierarchy by type, if index is informed, it will get the object at the
    // index of the vector
    template <ObjectType T> std::vector<Objects> getObjectByType() {
      std::vector<Objects> objects;
      std::copy_if(hierarchyObjects.begin(), hierarchyObjects.end(), std::back_inserter(objects),
                   [](const Objects& obj) { return obj.type == T; });
      return objects;
    }

    template <ObjectType T> Objects getObjectByType(int32_t index) {
      std::vector<Objects> objects = getObjectByType<T>();
      return objects[index];
    }

    bool m_wireframe = false;
    bool m_depthBuffer = true;

    // MODAL
    std::size_t bufferSize = sizeof(char) * 30;
    // ==== Sphere ====
    bool m_modalSphere = false;
    bool m_editing = false;
    char* namePtr;
    glm::vec3 resultPosition;
    float resultRadius;
    // ===============

    // ==== Camera ====
    bloom::Camera* cameraObject;
    // ================

    // ==== Inspector ====
    bool m_editingInspector = false;

    Light::Light() : m_translation(0.0f, 0.0f, 0.0f) {
      GLCall(glad_glEnable(GL_BLEND));
      GLCall(glad_glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

      // ================ Setting up Camera ================
      glm::vec3 position = glm::vec3(0.5f, 3.0f, 17.0f);

      hierarchyObjects.emplace_back(
          Objects{ObjectType::CAMERA, "Camera", 0, {.camera = new bloom::Camera(position)}});

      cameraObject = (bloom::Camera*)getObjectByTypeRef<ObjectType::CAMERA>(0).get();

      // Get window width
      // int width, height;
      // glfwGetWindowSize(bloom::gl::getWindow(), &width, &height);
      int width = 1920 / 2, height = 1080 / 2;

      const double fov = 45.0f;
      const double aspect = (float)width / (float)height;
      const double near = 0.1f;
      const double far = 100.0f;

      cameraObject->changeCameraType(CameraType::PERSPECTIVE, new double[]{fov, aspect, near, far})
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

      // ================ Setting up Sphere ================
      hierarchyObjects.emplace_back(Objects{
          ObjectType::SPHERE,
          "Sphere",
          0,
          {.sphere = new bloom::Sphere(glm::vec3{0, 0, 0}, .5, m_sectorCount, m_stackCount)}});
      // m_spheres.push_back(
      //     std::make_unique<bloom::Sphere>(glm::vec3{0, 0, 0}, .5, m_sectorCount, m_stackCount));
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
          glm::vec3(1.5f, 3.1f, -2.0f),
      };
      m_translation = glm::vec3(0.5f, 2.1f, -1.f);

      // Get the vector size
      int lightCount = sizeof(lightPositions) / sizeof(lightPositions[0]);
      for (int i = 0; i < lightCount; i++)
        hierarchyObjects.emplace_back(Objects{ObjectType::LIGHT,
                                              fmt::format("Light_{}", i),
                                              i,
                                              {.light = new bloom::Light(lightPositions[i])}});

      m_lightShader = std::make_unique<bloom::Shader>(
          "/home/danielboll/dev/Unioeste/2022/CG/bloomCG-xmake/assets/shaders/light.shader.glsl");
      m_lightShader->bind();
      m_lightShader->setUniform4f("uColor", lightColor);
      m_lightShader->unbind();
      // ==========================================================
    }

    void Light::onUpdate(const float deltaTime) {
      cameraObject->update(deltaTime);

      // for (auto& light : m_lights) light.get()->setPosition(m_translation);

      // m_spheres[0]->setSectorCount(m_sectorCount);
      // m_spheres[0]->setStackCount(m_stackCount);
    }

    void Light::onRender(const float deltaTime) {
      GLCall(glad_glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
      GLCall(glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
      GLCall(m_depthBuffer ? glad_glEnable(GL_DEPTH_TEST) : glad_glDisable(GL_DEPTH_TEST));

      m_objectShader->bind();
      m_objectShader->setUniformMat4f("uView", cameraObject->getViewMatrix());
      m_objectShader->setUniformMat4f("uProjection", cameraObject->getProjectionMatrix());
      m_objectShader->setUniform3f("uCameraPosition", cameraObject->getPosition());

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

      // for (auto& sphere : m_spheres) {
      //   glm::vec3 position = glm::vec3{.0, .0, .0};
      //   glm::mat4 model = glm::mat4(1.0f);
      //   float angle = 20.0f * offset++;
      //
      //   model = glm::translate(model, position);
      //   m_objectShader->setUniformMat4f("uModel", model);
      //   // m_objectShader->setUniform3f("uLightPosition", m_lights[0].get()->getPosition());
      //   m_objectShader->setUniform3f(
      //       "uLightPosition",
      //       ((bloom::Light*)getObjectByType<ObjectType::LIGHT>(0).get())->getPosition());
      //   sphere->draw();
      // }

      m_objectShader->unbind();

      m_lightShader->bind();
      m_lightShader->setUniformMat4f("uView", cameraObject->getViewMatrix());
      m_lightShader->setUniformMat4f("uProjection", cameraObject->getProjectionMatrix());

      // Move the light in a orbit around the center
      // m_translation.x = sin(glfwGetTime()) * 5.0f;
      // m_translation.z = cos(glfwGetTime()) * 5.0f;
      // m_translation.y = sin(glfwGetTime()) * 5.0f;

      if (m_wireframe) {
        GLCall(glad_glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
      } else {
        GLCall(glad_glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
      }

      // Loop through the hierarchyObjects and draw them
      for (auto& object : hierarchyObjects) {
        switch (object.type) {
          case ObjectType::CUBE: {
          }
          case ObjectType::SPHERE: {
            m_objectShader->bind();
            // glm::vec3 position = glm::vec3{.0, .0, .0};
            glm::vec3 position = object.get()->getPosition();
            glm::mat4 model = glm::mat4(1.0f);
            float angle = 20.0f * offset++;
            auto light = (bloom::Light*)getObjectByType<ObjectType::LIGHT>(0).get();

            model = glm::translate(model, position);
            m_objectShader->setUniformMat4f("uModel", model);
            // m_objectShader->setUniform3f("uLightPosition", m_lights[0].get()->getPosition());
            m_objectShader->setUniform3f("uLightPosition", light->getPosition());

            ((bloom::Sphere*)object.get())->draw();
            m_objectShader->unbind();
            break;
          }
          case ObjectType::LIGHT: {
            bloom::Light* light = (bloom::Light*)object.get();
            m_lightShader->bind();
            glm::vec3 position = object.get()->getPosition();
            glm::mat4 model = glm::mat4(1.0f);
            // reduce 1 of each coordinate of the position
            position.x -= .5f;
            position.y -= 2.1f;
            position.z += 1.0f;
            // 0.5f, 2.1f, -1.0f

            model = glm::translate(model, position);
            m_lightShader->setUniformMat4f("uModel", model);
            light->draw();
            m_lightShader->unbind();
            break;
          }
          case ObjectType::CAMERA:
            break;
        }
      }

      // for (auto& light : m_lights) {
      //   light->setPosition(m_translation);
      //   glm::vec3 position = light->getPosition();
      //   glm::mat4 model = glm::mat4(1.0f);
      //
      //   model = glm::translate(model, position);
      //   m_lightShader->setUniformMat4f("uModel", model);
      //   light->draw();
      // }

      m_lightShader->unbind();
    }

    void Light::inspector() {
      ImGui::Begin("Inspector");

      if (selected == -1) {
        ImGui::Text("Nothing is selected");
        ImGui::End();
        return;
      }

      Objects currentSelected = hierarchyObjects[selected];

      ImGui::TextDisabled("%s", currentSelected.name.c_str());

      switch (currentSelected.type) {
        case ObjectType::CUBE:
        case ObjectType::SPHERE: {
          // Position
          auto object = (bloom::Sphere*)currentSelected.get();
          glm::vec3 position = object->getPosition();

          ImGui::Text("Position");
          ImGui::InputFloat3("Position", glm::value_ptr(position));

          // Apply position change
          // If the position has changed, we update the object's position
          if (position != object->getPosition()) {
            object->setPosition(position);
          }
          break;
        }
        case ObjectType::LIGHT: {
          // Position
          auto object = (bloom::Light*)currentSelected.get();
          glm::vec3 position = object->getPosition();

          ImGui::Text("Position");
          ImGui::InputFloat3("Position", glm::value_ptr(position));

          // Apply position change
          if (position != object->getPosition()) {
            object->setPosition(position);
          }
          break;
        }
        case ObjectType::CAMERA: {
          // Position
          auto object = (bloom::Camera*)currentSelected.get();
          glm::vec3 position = object->getPosition();

          ImGui::Text("Position");
          ImGui::SliderFloat3("Position", glm::value_ptr(position), -100, 100);

          // Apply position change
          object->setPosition(position);
          break;
        }
      }

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
            // hierarchyObjects.emplace_back(
            //     Objects{ObjectType::LIGHT, "Light", (int32_t)m_lights.size()});
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
          "\"Hierarchy\" is a list of all the objects in the scene. Press Right click to add a "
          "new "
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
              hierarchyObjects.erase(hierarchyObjects.begin() + i);
            }
            ImGui::EndPopup();
          }
        }
      }

      ImGui::End();
    }

    void Light::addSphere(std::string* name, glm::vec3* position, float* radius) {
      const auto size = getObjectByType<ObjectType::SPHERE>().size();
      const std::string repeated = fmt::format("({})", size);
      const std::string sphereName = fmt::format("Sphere{}", size > 1 ? repeated : "");

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
            Objects{ObjectType::SPHERE,
                    namePtr,
                    (int32_t)getObjectByType<ObjectType::SPHERE>().size(),
                    {.sphere = new bloom::Sphere(resultPosition, resultRadius, 30, 30)}});
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
      // // TODO: add for every object. (only show to the selected one)
      // // TODO: add rotation on R
      // // TODO: add base ImGuizmo for coordinates
      if (selected == -1) return;

      ImGuizmo::SetOrthographic(false);
      ImGuizmo::SetDrawlist(bloom::Renderer::getViewportDrawList());

      float windowPosX = bloom::Renderer::getViewportX();
      float windowPosY = bloom::Renderer::getViewportY();

      float windowWidth = bloom::Renderer::getViewportWidth();
      float windowHeight = bloom::Renderer::getViewportHeight();

      ImGuizmo::SetRect(windowPosX, windowPosY, windowWidth, windowHeight);

      glm::mat4 model = glm::mat4(1.0f);
      auto* selectedModel = hierarchyObjects[selected].get();

      model = glm::translate(model, selectedModel->getPosition());

      ImGuizmo::Manipulate(glm::value_ptr(cameraObject->getViewMatrix()),
                           glm::value_ptr(cameraObject->getProjectionMatrix()),
                           ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(model));

      if (ImGuizmo::IsUsing()) {
        glm::vec3 position = glm::vec3(model[3]);
        selectedModel->setPosition(position);
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
        switch (cameraObject->getCameraType()) {
          case bloom::CameraType::PERSPECTIVE: {
            if (ImGui::Button("Change to parallel")) {
              cameraObject->changeCameraType(CameraType::AXONOMETRIC, new double[]{});
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

              cameraObject->changeCameraType(CameraType::PERSPECTIVE,
                                             new double[]{fov, aspect, near, far});
            }
            break;
          }
        }
      }

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

      ImGui::ShowDemoWindow();

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
