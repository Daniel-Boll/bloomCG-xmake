#include <fmt/core.h>
#include <fmt/ostream.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <bloomCG/core/camera.hpp>
#include <bloomCG/core/core.hpp>
#include <bloomCG/core/renderer.hpp>
#include <bloomCG/scenes/light.hpp>
#include <bloomCG/structures/shader.hpp>
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

static void TextCentered(std::string text) {
  auto windowWidth = ImGui::GetWindowSize().x;
  auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

  // Bold the text if it is the currently selected item
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));

  ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
  ImGui::Text("%s", text.c_str());

  ImGui::PopStyleColor(1);
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

  bloom::Entity* get() {
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

      if (objects.size() == 0) return Objects{};

      return objects[index];
    }

    bool m_wireframe = false;
    bool m_depthBuffer = true;

    // clang-format off
    // +++++++++++++++++++ MODAL +++++++++++++++++++++++++
    /*                                                  */
    /**/ std::size_t bufferSize = sizeof(char) * 30;
    /*                                                  */
    /*                     SPHERE                       */
    /**/ bool m_modalSphere = false;                    /**/
    /**/ bool m_editingSphere = false;                  /**/
    /**/ char* namePtrSphere;                           /**/
    /**/ glm::vec3 resultPositionSphere;                /**/
    /**/ glm::vec3 resultColorSphere;                   /**/
    /**/ float resultRadiusSphere;                      /**/
    /*                                                  */
    /*                      CUBE                        */
    /**/ bool m_modalCube = false;                      /**/
    /**/ bool m_editingCube = false;                    /**/
    /**/ char* namePtrCube;                             /**/
    /**/ glm::vec3 resultPositionCube;                  /**/
    /**/ glm::vec3 resultColorCube;                     /**/
    /**/ float resultSideCube;                          /**/
    /*                                                  */
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++
    // clang-format on

    // ==== Camera ====
    bloom::Camera* cameraObject;
    // ================

    // ==== Inspector ====
    bool m_editingInspector = false;

    // ==== Shaders ====
    ShaderMap* shaders;

    Light::Light() : m_translation(0.0f, 0.0f, 0.0f) {
      GLCall(glad_glEnable(GL_BLEND));
      GLCall(glad_glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

      // ================ Setting up Camera ================
      glm::vec3 position = glm::vec3(0.5f, 3.0f, 17.0f);

      hierarchyObjects.emplace_back(
          Objects{ObjectType::CAMERA, "Camera", 0, {.camera = new bloom::Camera(position)}});

      cameraObject = (bloom::Camera*)getObjectByTypeRef<ObjectType::CAMERA>(0).get();

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

      glm::vec4 lightColor = glm::vec4{1., 1., 1., 1.};

      // =================== Elements in the scene ================
      glm::vec3 cubePositions[] = {
          glm::vec3(0.0f, 0.0f, 0.0f),
      };

      // ================ Setting up Sphere ================
      hierarchyObjects.emplace_back(
          Objects{ObjectType::SPHERE,
                  "Sphere",
                  0,
                  {.sphere = new bloom::Sphere(glm::vec3{0, 0, 0}, glm::vec3{1, 0, 0}, 2,
                                               m_sectorCount, m_stackCount)}});

      // ================ Setting up Shaders ================
      // Get current directory
      const std::string cd = std::filesystem::current_path().string();
      auto at
          = [cd](const std::string& path) { return cd + "/../../../../assets/shaders/" + path; };

      shaders->registerShader<ShaderType::Object, LightModel::Phong>(at("object.shader.phong.glsl"))
          ->registerShader<ShaderType::Object, LightModel::Gouraud>(
              at("object.shader.gouraud.glsl"))
          ->registerShader<ShaderType::Light, LightModel::Phong>(at("light.shader.glsl"));

      // m_objectShader->setUniform1f("uLight.constant", 1.0f);
      // m_objectShader->setUniform1f("uLight.linear", .09f);
      // m_objectShader->setUniform1f("uLight.quadratic", .032f);
      // m_lightShader->setUniform4f("uColor", lightColor);
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

      // m_lightShader->setUniform4f("uColor", lightColor);
      // ==========================================================
    }

    void Light::onUpdate(const float deltaTime) { cameraObject->update(deltaTime); }

    void Light::onRender(const float deltaTime) {
      GLCall(glad_glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
      GLCall(glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
      GLCall(m_depthBuffer ? glad_glEnable(GL_DEPTH_TEST) : glad_glDisable(GL_DEPTH_TEST));

      auto objectShader = shaders->get<ShaderType::Object, LightModel::Phong>();
      objectShader->bind()
          ->setUniformMat4f("uView", cameraObject->getViewMatrix())
          ->setUniformMat4f("uProjection", cameraObject->getProjectionMatrix())
          ->setUniform3f("uCameraPosition", cameraObject->getPosition())
          ->setUniform3f("uLight.ambient", m_lightAmbient)
          ->setUniform3f("uLight.diffuse", m_lightDiffuse)
          ->setUniform3f("uLight.specular", m_lightSpecular);
      objectShader->unbind();

      auto lightShader = shaders->get<ShaderType::Light, LightModel::Phong>();
      lightShader->bind()
          ->setUniformMat4f("uView", cameraObject->getViewMatrix())
          ->setUniformMat4f("uProjection", cameraObject->getProjectionMatrix());
      lightShader->unbind();

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
          case ObjectType::CUBE:
          case ObjectType::SPHERE: {
            auto _object = (bloom::Object*)object.get();
            glm::vec3 position = _object->getPosition();

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, position);

            auto appliedRotation = _object->getAppliedRotation();
            model
                = glm::rotate(model, glm::radians(appliedRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model
                = glm::rotate(model, glm::radians(appliedRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model
                = glm::rotate(model, glm::radians(appliedRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

            auto appliedScale = _object->getAppliedScale();
            model = glm::scale(model, appliedScale);

            objectShader->bind()
                ->setUniform3f("uMaterial.ambient", _object->getKa())
                ->setUniform3f("uMaterial.diffuse", _object->getKd())
                ->setUniform3f("uMaterial.specular", _object->getKs())
                ->setUniform1f("uMaterial.shininess", _object->getShininess())
                ->setUniform1f("uLight.constant", 1.0f)
                ->setUniform1f("uLight.linear", .09f)
                ->setUniform1f("uLight.quadratic", .032f)
                // ->setUniform3f("uObjectColor", glm::vec3(1.0f, 0.0f, 0.0f)) // Just turn this on
                // when on Gouraud
                ->setUniformMat4f("uModel", model);

            // m_objectShader->setUniform1f("uLight.constant", 1.0f);
            // m_objectShader->setUniform1f("uLight.linear", .09f);
            // m_objectShader->setUniform1f("uLight.quadratic", .032f);
            // m_lightShader->setUniform4f("uColor", lightColor);

            auto light = (bloom::Light*)getObjectByType<ObjectType::LIGHT>(0).get();
            if (light != nullptr)
              objectShader->setUniform3f("uLightPosition", light->getPosition());
            objectShader->setUniform1i("uUseLighting", light ? 1 : 0);

            _object->draw();
            objectShader->unbind();
            break;
          }
          case ObjectType::LIGHT: {
            auto light = (bloom::Light*)object.get();
            glm::vec3 position = object.get()->getPosition();
            glm::mat4 model = glm::mat4(1.0f);
            // reduce 1 of each coordinate of the position
            // position.x -= .5f;
            // position.y -= 2.1f;
            // position.z += 1.0f;
            // 0.5f, 2.1f, -1.0f

            model = glm::translate(model, position);
            lightShader->bind()
                ->setUniformMat4f("uModel", model)
                ->setUniform4f("uColor", glm::vec4{1});
            light->draw();
            lightShader->unbind();

            break;
          }
          case ObjectType::CAMERA:
            break;
        }
      }
    }

    void Light::inspector() {
      ImGui::Begin("Inspector");

      if (selected == -1) {
        ImGui::Text("Nothing is selected");
        ImGui::End();
        return;
      }

      Objects currentSelected = hierarchyObjects[selected];

      TextCentered(currentSelected.name);

      ImGui::Separator();
      ImGui::Spacing();
      ImGui::Text("Transform");

      auto object = (bloom::Object*)currentSelected.get();
      {  // Transform
        glm::vec3 position = object->getPosition();

        ImGui::SliderFloat3("Position", glm::value_ptr(position), -100, 100);

        // Apply position change
        // If the position has changed, we update the object's position
        if (position != object->getPosition()) {
          object->setPosition(position);
        }

        glm::vec3 appliedRotation = object->getAppliedRotation();
        glm::vec3 appliedScale = object->getAppliedScale();

        ImGui::SliderFloat3("Rotation", glm::value_ptr(appliedRotation), 0, 360.0f);
        ImGui::SliderFloat3("Scale", glm::value_ptr(appliedScale), 0, 10.0f);

        if (appliedRotation != object->getAppliedRotation()) {
          object->setAppliedRotation(glm::vec3(appliedRotation));
        }
        if (appliedScale != object->getAppliedScale()) {
          object->setAppliedScale(glm::vec3(appliedScale));
        }

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("Material");

        glm::vec3 ka = object->getKa();
        glm::vec3 kd = object->getKd();
        glm::vec3 ks = object->getKs();
        int32_t shininess = object->getShininess();

        ImGui::ColorEdit3("Light ambient", glm::value_ptr(ka));
        ImGui::ColorEdit3("Light diffuse", glm::value_ptr(kd));
        ImGui::ColorEdit3("Light specular", glm::value_ptr(ks));
        ImGui::InputInt("Shininess", &shininess);

        if (ka != object->getKa() || kd != object->getKd() || ks != object->getKs()
            || shininess != object->getShininess()) {
          object->setKa(ka);
          object->setKd(kd);
          object->setKs(ks);
          object->setShininess(shininess);
        }
      }

      switch (currentSelected.type) {
        case ObjectType::CUBE: {
          auto cube = (bloom::Cube*)object;
          ImGui::Separator();
          ImGui::Spacing();
          ImGui::Text("Object");

          float side = cube->getSide();

          // In the same line three inputs for: radius, sectors and stacks
          ImGui::SliderFloat("Side", &side, 0.0f, 100.0f);

          if (side != cube->getSide()) {
            cube->setSide(side);
          }
          break;
        }
        case ObjectType::SPHERE: {
          auto sphere = (bloom::Sphere*)object;

          ImGui::Separator();
          ImGui::Spacing();
          ImGui::Text("Object");

          float radius = sphere->getRadius();
          int32_t sectors = sphere->getSectorCount();
          int32_t stacks = sphere->getStackCount();

          // In the same line three inputs for: radius, sectors and stacks
          ImGui::SliderFloat("Radius", &radius, 0.0f, 100.0f);
          ImGui::SliderInt("Sectors", &sectors, 3, 100);
          ImGui::SliderInt("Stacks", &stacks, 2, 100);

          if (radius != sphere->getRadius() || sectors != sphere->getSectorCount()
              || stacks != sphere->getStackCount()) {
            sphere->setRadius(radius);
            sphere->setSectorCount(sectors);
            sphere->setStackCount(stacks);
          }
          break;
        }
        case ObjectType::LIGHT: {
          break;
        }
        case ObjectType::CAMERA: {
          break;
        }
      }

      ImGui::End();
    }

    void Light::hierarchy() {
      ImGui::Begin("Hierarchy");

      if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::BeginMenu("Create element")) {
          if (ImGui::MenuItem("Sphere")) m_modalSphere = true;

          if (ImGui::MenuItem("Cube")) m_modalCube = true;

          if (ImGui::MenuItem("Light")) {
            // hierarchyObjects.emplace_back(
            //     Objects{ObjectType::LIGHT, "Light", (int32_t)m_lights.size()});
          }

          ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scene controllers")) {
          ImGui::Checkbox("Wireframe", &m_wireframe);
          ImGui::Checkbox("Depth buffer", &m_depthBuffer);
          ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Load .element", "CTRL+L", false, false)) {
        }

        if (ImGui::MenuItem("Save .element", "CTRL+S", false, false)) {
        }

        if (ImGui::MenuItem("Clear scene")) {
          // Clear everything except for the camera
          auto camera = getObjectByType<ObjectType::CAMERA>(0);

          std::vector<Objects>().swap(hierarchyObjects);
          hierarchyObjects.emplace_back(camera);

          // cameraObject = (bloom::Camera*)getObjectByTypeRef<ObjectType::CAMERA>(0).get();
          ImGui::EndMenu();
        }
        ImGui::EndPopup();
      }

      {  // Modals
        if (m_modalSphere == true) ImGui::OpenPopup("add_sphere");
        if (m_modalCube == true) ImGui::OpenPopup("add_cube");

        // Always center this window when appearing
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("add_sphere", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
          addSphere();
          m_modalSphere = false;
        }

        if (ImGui::BeginPopupModal("add_cube", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
          addCube();
          m_modalCube = false;
        }
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
      const std::string sphereName = fmt::format("Sphere{}", size >= 1 ? repeated : "");

      if (!m_editingSphere) {
        std::string resultName = name ? *name : sphereName;
        // std::string to char*
        namePtrSphere = new char[resultName.size() + 1];
        std::copy(resultName.begin(), resultName.end(), namePtrSphere);
        // Create buffer
        resultPositionSphere = position ? *position : glm::vec3(0.0f);
        resultRadiusSphere = radius ? *radius : 1.0f;
        resultColorCube = glm::vec3(1.0f);
        m_editingSphere = true;
      }

      ImGui::InputText("Name", namePtrSphere, 64);
      ImGui::Separator();

      ImGui::InputFloat3("Position", glm::value_ptr(resultPositionSphere));
      ImGui::Separator();

      ImGui::InputFloat("Radius", &resultRadiusSphere);
      ImGui::Spacing();

      ImGui::ColorEdit3("Color", glm::value_ptr(resultColorSphere));
      ImGui::Spacing();

      if (ImGui::Button("OK", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
        m_editingSphere = false;

        hierarchyObjects.emplace_back(
            Objects{ObjectType::SPHERE,
                    namePtrSphere,
                    (int32_t)getObjectByType<ObjectType::SPHERE>().size(),
                    {.sphere = new bloom::Sphere(resultPositionSphere, resultColorSphere,
                                                 resultRadiusSphere, 30, 30)}});
      }
      ImGui::SetItemDefaultFocus();
      ImGui::SameLine();
      if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();

      m_modalSphere = false;
    }

    void Light::addCube(std::string* name, glm::vec3* position, float* side) {
      const auto size = getObjectByType<ObjectType::CUBE>().size();
      const std::string repeated = fmt::format("({})", size);
      const std::string cubeName = fmt::format("Cube{}", size >= 1 ? repeated : "");

      if (!m_editingSphere) {
        std::string resultName = name ? *name : cubeName;
        // std::string to char*
        namePtrCube = new char[resultName.size()];
        std::copy(resultName.begin(), resultName.end(), namePtrCube);
        // Create buffer
        resultPositionCube = position ? *position : glm::vec3(0);
        resultSideCube = side ? *side : .5f;
        resultColorCube = glm::vec3(1.0f);
        m_editingCube = true;
      }

      ImGui::InputText("Name", namePtrCube, 64);
      ImGui::Separator();

      ImGui::InputFloat3("Position", glm::value_ptr(resultPositionCube));
      ImGui::Separator();

      ImGui::InputFloat("Side", &resultSideCube);
      ImGui::Spacing();

      ImGui::ColorEdit3("Color", glm::value_ptr(resultColorCube));
      ImGui::Spacing();

      if (ImGui::Button("OK", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
        m_editingCube = false;

        hierarchyObjects.emplace_back(Objects{
            ObjectType::CUBE,
            namePtrCube,
            (int32_t)getObjectByType<ObjectType::CUBE>().size(),
            {.cube = new bloom::Cube(resultPositionCube, resultSideCube, resultColorCube)}});
      }
      ImGui::SetItemDefaultFocus();
      ImGui::SameLine();

      if (ImGui::Button("Cancel", ImVec2(120, 0))) ImGui::CloseCurrentPopup();

      ImGui::EndPopup();

      m_modalCube = false;
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

      ImGui::ShowDemoWindow();

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
  }  // namespace scene
}  // namespace bloom
