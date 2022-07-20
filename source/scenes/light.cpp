#include <bloomCG/core/camera.hpp>
#include <bloomCG/core/core.hpp>
#include <bloomCG/core/renderer.hpp>
#include <bloomCG/scenes/light.hpp>
#include <bloomCG/structures/hierarchy.hpp>
#include <bloomCG/structures/shader.hpp>
#include <bloomCG/utils/imgui.hpp>
#include <bloomCG/utils/polymorphism.hpp>

namespace bloom {
  namespace scene {
    int8_t selected = -1;

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
    /**/ int resultStackSphere;                         /**/
    /**/ int resultSectorSphere;                        /**/
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

    // ==== Scene controllers ====
    bool m_canMove = true;
    bool m_isPaused = false;

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

      // ==========================================================

      // =================== Lights in the scene ================
      glm::vec3 lightPositions[] = {
          glm::vec3(0.0f, 0.0f, 0.0f),
          glm::vec3(1.5f, 3.1f, -2.0f),
      };
      m_translation = glm::vec3(0.0f, 0.0f, 0.f);

      int lightCount = sizeof(lightPositions) / sizeof(lightPositions[0]);
      for (int i = 0; i < lightCount; i++) {
        auto light = (bloom::Object*)new bloom::Light(lightPositions[i]);
        light->setKa(glm::vec3(0.2));
        light->setKd(glm::vec3(0.2));
        light->setKs(glm::vec3(0.8));

        hierarchyObjects.emplace_back(Objects{
            ObjectType::LIGHT, fmt::format("Light_{}", i), i, {.light = (bloom::Light*)light}});
      }
      // ==========================================================
    }

    void Light::onUpdate(const float deltaTime) {
      if (m_isPaused) return;

      cameraObject->update(deltaTime);
      // Update light position based on m_translation
      // ((bloom::Light*)getObjectByType<ObjectType::LIGHT>(0).get())
      //     ->setAppliedTransformation(m_translation);
      // ((bloom::Light*)getObjectByType<ObjectType::LIGHT>(1).get())
      //     ->setAppliedTransformation(-m_translation);
    }

    void Light::onRender(const float deltaTime) {
      GLCall(glad_glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
      GLCall(glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
      GLCall(m_depthBuffer ? glad_glEnable(GL_DEPTH_TEST) : glad_glDisable(GL_DEPTH_TEST));

      if (m_isPaused) return;

      auto lightShader = shaders->get<ShaderType::Light, LightModel::Phong>();
      lightShader->bind()
          ->setUniformMat4f("uView", cameraObject->getViewMatrix())
          ->setUniformMat4f("uProjection", cameraObject->getProjectionMatrix());
      lightShader->unbind();

      // Move the light in a orbit around the center
      m_translation.x = sin(glfwGetTime() * 3) * 3.0f;
      m_translation.z = cos(glfwGetTime() * 3) * 3.0f;
      m_translation.y = sin(glfwGetTime() * 3) * 3.0f;

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

            // Translation
            auto appliedTranslation = _object->getAppliedTransformation();
            model = glm::translate(model, appliedTranslation);

            // Rotation
            auto appliedRotation = _object->getAppliedRotation();
            model
                = glm::rotate(model, glm::radians(appliedRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model
                = glm::rotate(model, glm::radians(appliedRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model
                = glm::rotate(model, glm::radians(appliedRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

            // Scale
            auto appliedScale = _object->getAppliedScale();
            model = glm::scale(model, appliedScale);

            bloom::Shader* shader = nullptr;

            switch ((LightModel)_object->getShading()) {
              case LightModel::Phong:
                shader = shaders->get<ShaderType::Object, LightModel::Phong>();
                break;
              case LightModel::Gouraud:
                shader = shaders->get<ShaderType::Object, LightModel::Gouraud>();
                break;
              case LightModel::Flat:
                shader = shaders->get<ShaderType::Object, LightModel::Flat>();
                break;
            }

            shader->bind()
                ->setUniformMat4f("uView", cameraObject->getViewMatrix())
                ->setUniformMat4f("uProjection", cameraObject->getProjectionMatrix())
                ->setUniformMat4f("uModel", model)
                ->setUniform3f("uCameraPosition", cameraObject->getPosition())
                ->setUniform3f("uMaterial.ambient", _object->getKa())
                ->setUniform3f("uMaterial.diffuse", _object->getKd())
                ->setUniform3f("uMaterial.specular", _object->getKs())
                ->setUniform1f("uMaterial.shininess", _object->getShininess())
                ->setUniform3f("uObjectColor", glm::vec3(1.0f, 0.0f, 0.0f));  // Gouraud's only

            auto lights = getObjectByType<ObjectType::LIGHT>();
            if (!lights.empty()) {
              for (auto& light : lights) {
                std::string prefix = fmt::format("uPointLights[{}].", light.index);
                auto _light = (bloom::Object*)light.get();

                shader->setUniform3f(prefix + "position", _light->getAppliedTransformation())
                    ->setUniform3f(prefix + "ambient", _light->getKa())
                    ->setUniform3f(prefix + "diffuse", _light->getKd())
                    ->setUniform3f(prefix + "specular", _light->getKs())
                    ->setUniform1f(prefix + "constant", 1.0f)
                    ->setUniform1f(prefix + "linear", .09f)
                    ->setUniform1f(prefix + "quadratic", .032f);
              }
            }

            // Set light's constraints
            shader->setUniform1i("uUseLighting", !lights.empty() ? 1 : 0);
            shader->setUniform1i("uPointLightCount", lights.size());

            _object->draw();
            shader->unbind();
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

            // Translation
            auto appliedTranslation = light->getAppliedTransformation();
            model = glm::translate(model, appliedTranslation);

            lightShader->bind()
                ->setUniformMat4f("uView", cameraObject->getViewMatrix())
                ->setUniformMat4f("uProjection", cameraObject->getProjectionMatrix())
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

        if (! instanceof <bloom::Object>(currentSelected.get())) goto common_end;

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

        if (instanceof <bloom::Light>(currentSelected.get())) goto common_end;

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("Lighting model");

        int32_t shading = (int32_t)object->getShading();

        // Create a input list for the shading options
        const char* shadingNames[] = {"Flat", "Gouraud", "Phong"};

        ImGui::Combo("Shading", &shading, shadingNames, IM_ARRAYSIZE(shadingNames),
                     IM_ARRAYSIZE(shadingNames));

        if (shading != (int32_t)object->getShading()) {
          fmt::print("Shading changed to {}\n", shadingNames[shading]);
          object->setShading((bloom::Object::Shading)shading);
        }
      }

    common_end:

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
          break;
        }
      }

      ImGui::End();
    }

    void Light::hierarchy() {
      ImGui::Begin("Hierarchy");

      if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::BeginMenu(ICON_FA_PLUS_CIRCLE " Create element")) {
          ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), ICON_FA_CIRCLE);
          ImGui::SameLine();
          if (ImGui::MenuItem("Sphere")) m_modalSphere = true;

          ImGui::TextColored(ImVec4(0.2f, 0.7f, 1.0f, 1.0f), ICON_FA_CUBE);
          ImGui::SameLine();
          if (ImGui::MenuItem("Cube")) m_modalCube = true;

          // Apply yellow text color for the next line
          ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ICON_FA_LIGHTBULB);
          ImGui::SameLine();
          if (ImGui::MenuItem("Light")) {
            // hierarchyObjects.emplace_back(
            //     Objects{ObjectType::LIGHT, "Light", (int32_t)m_lights.size()});
          }

          ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(ICON_FA_GAMEPAD " Scene controllers")) {
          ImGui::Checkbox("Wireframe", &m_wireframe);
          ImGui::Checkbox("Depth buffer", &m_depthBuffer);
          ImGui::EndMenu();
        }

        if (ImGui::MenuItem(ICON_FA_FILE_UPLOAD " Load .element", "CTRL+L", false, false)) {
        }

        if (ImGui::MenuItem(ICON_FA_SAVE " Save .element", "CTRL+S", false, false)) {
        }

        if (ImGui::MenuItem(ICON_FA_PAINT_ROLLER
                            " Clear scene")) {  // Clear everything except for the camera
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
          "new object\n");
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
        namePtrSphere = new char[resultName.size() + 1];
        std::copy(resultName.begin(), resultName.end(), namePtrSphere);

        resultPositionSphere = position ? *position : glm::vec3(0.0f);
        resultRadiusSphere = radius ? *radius : 1.0f;
        resultColorCube = glm::vec3(1.0f, 1.0f, 1.0f);

        resultStackSphere = 30;
        resultSectorSphere = 30;

        m_editingSphere = true;
      }

      ImGui::InputText("Name", namePtrSphere, 64);
      ImGui::Separator();

      ImGui::InputFloat3("Position", glm::value_ptr(resultPositionSphere));
      ImGui::Separator();

      ImGui::InputFloat("Radius", &resultRadiusSphere);
      ImGui::Spacing();

      ImGui::InputInt("Sector", &resultSectorSphere);
      ImGui::InputInt("Stack", &resultStackSphere);
      ImGui::Spacing();

      ImGui::ColorEdit3("Color", glm::value_ptr(resultColorSphere));
      ImGui::SameLine();
      HelpMarker(
          "\"Color\" this parameter is deceiving, the color of the sphere isn't real"
          " it's simpled mapped to Ka and Kd coeficientes of the Material.\n");
      ImGui::Spacing();

      if (ImGui::Button("OK", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
        m_editingSphere = false;

        hierarchyObjects.emplace_back(Objects{
            ObjectType::SPHERE,
            namePtrSphere,
            (int32_t)getObjectByType<ObjectType::SPHERE>().size(),
            {.sphere
             = new bloom::Sphere(resultPositionSphere, resultColorSphere, resultRadiusSphere,
                                 resultSectorSphere, resultStackSphere)}});
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

      HelpMarker(
          "\"Color\" is just a abstraction. \nIn reality what is happening is that the color that "
          "you are selecting is being set as the ambient and diffuse coeficients of the "
          "material\n");
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
      // ImGui::Begin("Guizmo");
      if (selected == -1) return;

      ImGuizmo::SetDrawlist(bloom::Renderer::getViewportDrawList());
      ImGuizmo::SetOrthographic(false);

      float windowPosX = bloom::Renderer::getViewportX();
      float windowPosY = bloom::Renderer::getViewportY();

      float windowWidth = bloom::Renderer::getViewportWidth();
      float windowHeight = bloom::Renderer::getViewportHeight();

      ImGuizmo::SetRect(windowPosX, windowPosY, windowWidth, windowHeight);

      glm::mat4 model = glm::mat4(1.0f);
      auto selectedModel = (bloom::Object*)hierarchyObjects[selected].get();

      model = glm::translate(model, selectedModel->getAppliedTransformation());

      ImGuizmo::Manipulate(glm::value_ptr(cameraObject->getViewMatrix()),
                           glm::value_ptr(cameraObject->getProjectionMatrix()),
                           ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(model));

      if (ImGuizmo::IsUsing()) {
        glm::vec3 position = glm::vec3(model[3]);
        // selectedModel->setPosition(position);
        ((bloom::Object*)selectedModel)->setAppliedTransformation(position);
      }
      // ImGui::End();
    }

    void Light::guizmoController() {
      // Gray background
      ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.00f, 1.00f, 0.13f, 1.00f));
      ImGui::Begin("Dear");
      {
        // Center buttons in the window
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f);
        bool decoy = false;

        selectableButton(ICON_FA_ARROWS_ALT, &m_canMove);
        selectableButton(m_isPaused ? ICON_FA_PLAY : ICON_FA_PAUSE, &m_isPaused);
        selectableButton(ICON_FA_SEARCH_PLUS, &decoy);
        selectableButton(ICON_FA_SEARCH_MINUS, &decoy);
        selectableButton(ICON_FA_PAINT_ROLLER, &decoy);
      }
      ImGui::End();
      ImGui::PopStyleColor(1);
    }

    void Light::onImGuiRender() {
      hierarchy();
      inspector();
      if (m_canMove) enableGuizmo();
      guizmoController();

      ImGui::ShowDemoWindow();

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
  }  // namespace scene
}  // namespace bloom
