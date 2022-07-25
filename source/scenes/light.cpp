#include <bloomCG/core/camera.hpp>
#include <bloomCG/core/core.hpp>
#include <bloomCG/core/renderer.hpp>
#include <bloomCG/models/light.hpp>
#include <bloomCG/scenes/light.hpp>
#include <bloomCG/structures/hierarchy.hpp>
#include <bloomCG/structures/shader.hpp>
#include <bloomCG/utils/imgui.hpp>
#include <bloomCG/utils/polymorphism.hpp>

#include "ImGuizmo.h"

namespace bloom {
  namespace scene {
    int8_t selected = -1;

    bool m_wireframe = false;
    bool m_depthBuffer = true;
    bool m_orbitLights = true;

    // clang-format off
    // +++++++++++++++++++ MODAL +++++++++++++++++++++++++
    /*                                                  */
    /**/ std::size_t bufferSize = sizeof(char) * 30;
    /*                                                  */
    /*                     SPHERE                       */
    /**/ bool m_modalSphere = false;                    /**/
    /**/ bool m_editingSphere = false;                  /**/
    /**/ char* namePtrSphere;                           /**/
    /**/ std::string errorMessageSphere = "";           /**/
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
    /**/ std::string errorMessageCube = "";             /**/
    /**/ glm::vec3 resultPositionCube;                  /**/
    /**/ glm::vec3 resultColorCube;                     /**/
    /**/ float resultSideCube;                          /**/
    /*                                                  */
    /*                      LIGHT                       */
    /**/ bool m_modalLight = false;                      /**/
    /**/ bool m_editingLight = false;                    /**/
    /**/ char* namePtrLight;                             /**/
    /**/ std::string errorMessageLight = "";             /**/
    /**/ glm::vec3 resultPositionLight;                  /**/
    /**/ glm::vec3 resultIntensityLight;                 /**/
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
    bool m_increaseWindow = false;
    bool m_decreaseWindow = false;

    std::array<double, 8> randomVelocities;
    std::array<double, 8> randomDistances;

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
      const double far = 1000.0f;

      cameraObject->setFieldOfView(fov)
          ->setAspectRatio(aspect)
          ->setNearPlane(near)
          ->setFarPlane(far)
          ->changeCameraType(CameraType::PERSPECTIVE)
          ->toggleMovement()
          ->toggleMouseMovement()
          ->setCameraSpeed(2.5f)
          ->setCameraSensitivity(.1);
      // ======================================================

      // ================ Setting up Floor ================
      hierarchyObjects.emplace_back(
          Objects{ObjectType::CUBE, "Floor", 0, {.cube = new bloom::Cube(glm::vec3{0})}});

      // Manipulate the floor
      bloom::Cube* floor = ((bloom::Cube*)getObjectByType<ObjectType::CUBE>(0).get());
      floor->setAppliedTransformation(glm::vec3{0, -8, 0});
      floor->setAppliedScale(glm::vec3{1000, 0.01, 1000});
      floor->setColor(glm::vec3{0.5f, 0.5f, 0.5f});

      // delete floor;
      // ======================================================

      // ================ Setting up Sphere ================
      hierarchyObjects.emplace_back(
          Objects{ObjectType::SPHERE,
                  "Sphere",
                  0,
                  {.sphere = new bloom::Sphere(glm::vec3{0, 0, 0}, glm::vec3{1, 0, 0}, 2,
                                               m_sectorCount, m_stackCount)}});
      // ======================================================

      // ================ Setting up Shaders ================
      // TODO: extract this to utils.
      // Get current directory
      const std::string cd = std::filesystem::current_path().string();
      auto at
          = [cd](const std::string& path) { return cd + "/../../../../assets/shaders/" + path; };

      shaders->registerShader<ShaderType::Object, LightModel::Flat>(at("object.flat.glsl"))
          ->registerShader<ShaderType::Object, LightModel::Gouraud>(at("object.gouraud.glsl"))
          ->registerShader<ShaderType::Object, LightModel::Phong>(at("object.phong.glsl"))
          ->registerShader<ShaderType::Light, LightModel::Phong>(at("light.glsl"));
      // ======================================================

      // =================== Lights in the scene ================
      auto ambientLight = new bloom::AmbientLight(glm::vec3{0.2f, 0.2f, 0.2f});

      hierarchyObjects.emplace_back(
          Objects{ObjectType::AMBIENT_LIGHT, "Ambient Light", 0, {.ambientLight = ambientLight}});

      glm::vec3 lightPositions[] = {
          glm::vec3(0.0f, 0.0f, 0.0f),
          glm::vec3(1.5f, 3.1f, -2.0f),
      };
      m_translation = glm::vec3(0.0f, 0.0f, 0.f);

      int lightCount = sizeof(lightPositions) / sizeof(lightPositions[0]);
      for (int i = 0; i < lightCount; i++) {
        hierarchyObjects.emplace_back(
            Objects{ObjectType::POINT_LIGHT,
                    fmt::format("Point Light{}", i == 0 ? "" : fmt::format(" ({})", i)),
                    i,
                    {.pointLight = new bloom::PointLight(lightPositions[i])}});
      }
      // ==========================================================

      cameraObject->setViewportU(glm::vec2{-1, 1});
      cameraObject->setViewportV(glm::vec2{-1, 1});
      cameraObject->setWindowSizeX(glm::vec2{-1, 1});
      cameraObject->setWindowSizeY(glm::vec2{-1, 1});

      // Generate 8 random values between
      for (int i = 0; i < 8; i++) {
        randomVelocities[i] = .5 + std::rand() / ((RAND_MAX + 1u) / 2.5);
        randomDistances[i] = 3.0 + std::rand() / ((RAND_MAX + 1u) / 2.);
      }
    }

    void Light::onUpdate(const float deltaTime) {
      if (m_isPaused) return;

      cameraObject->update(deltaTime);

      if (m_orbitLights) {
        // Get all point lights
        for (auto& object : getObjectByType<ObjectType::POINT_LIGHT>()) {
          auto pointLight = (bloom::PointLight*)object.get();
          auto tick = glfwGetTime();
          auto index = object.index;
          auto randomVelocity = randomVelocities[index];
          auto randomDistance = randomDistances[index];

          // Get the position of the light
          glm::vec3 position = pointLight->getAppliedTransformation();

          if (index % 4 == 0) {
            position.x = sin(randomVelocity * tick) * randomDistance;
            position.y = cos(randomVelocity * tick) * randomDistance;
            position.z = sin(randomVelocity * tick) * randomDistance;
          } else if (index % 4 == 1) {
            position.x = sin(randomVelocity * tick) * randomDistance;
            position.y = cos(randomVelocity * tick) * randomDistance;
            position.z = cos(randomVelocity * tick) * randomDistance;
          } else if (index % 4 == 2) {
            position.x = cos(randomVelocity * tick) * randomDistance;
            position.y = sin(randomVelocity * tick) * randomDistance;
            position.z = sin(randomVelocity * tick) * randomDistance;
          } else {
            position.x = cos(randomVelocity * tick) * randomDistance;
            position.y = sin(randomVelocity * tick) * randomDistance;
            position.z = cos(randomVelocity * tick) * randomDistance;
          }

          // Set the new position of the light.
          pointLight->setAppliedTransformation(position);
        }
      }
    }

    void Light::onRender(const float deltaTime) {
      GLCall(glad_glClearColor(0.0, 0.0, 0.0, 1.0f));
      // Add a sky color
      // GLCall(glad_glClearColor(0.529f, 0.808f, 0.922f, 1.0f));
      GLCall(glad_glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
      GLCall(m_depthBuffer ? glad_glEnable(GL_DEPTH_TEST) : glad_glDisable(GL_DEPTH_TEST));

      if (m_isPaused) return;

      auto lightShader = shaders->get<ShaderType::Light, LightModel::Phong>();
      lightShader->bind()
          ->setUniformMat4f("uView", cameraObject->getViewMatrix())
          ->setUniformMat4f("uProjection", cameraObject->getProjectionMatrix());
      lightShader->unbind();

      // Move the light in a orbit around the center
      // m_translation.x = sin(glfwGetTime() * 3) * 3.0f;
      // m_translation.z = cos(glfwGetTime() * 3) * 3.0f;
      // m_translation.y = sin(glfwGetTime() * 3) * 3.0f;

      if (m_wireframe) {
        GLCall(glad_glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
      } else {
        GLCall(glad_glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
      }

      // Loop through the hierarchyObjects and draw them
      for (auto& object : hierarchyObjects) {
        if (!object.visible) continue;

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

            auto ambientLight
                = (bloom::AmbientLight*)getObjectByType<ObjectType::AMBIENT_LIGHT>(0).get();

            shader->bind()
                ->setUniformMat4f("uW2V", cameraObject->getViewportMatrix())
                ->setUniformMat4f("uProjection", cameraObject->getProjectionMatrix())
                ->setUniformMat4f("uView", cameraObject->getViewMatrix())
                ->setUniformMat4f("uModel", model)
                ->setUniform3f("uCameraPosition", cameraObject->getPosition())
                ->setUniform3f("uMaterial.ambient", _object->getKa())
                ->setUniform3f("uMaterial.diffuse", _object->getKd())
                ->setUniform3f("uMaterial.specular", _object->getKs())
                ->setUniform1f("uMaterial.shininess", _object->getShininess())
                ->setUniform3f("uAmbientLight.intensity", ambientLight->getIntensity());

            auto lights = getObjectByType<ObjectType::POINT_LIGHT>();
            if (!lights.empty()) {
              for (auto& light : lights) {
                std::string prefix = fmt::format("uPointLights[{}].", light.index);
                auto _light = (bloom::PointLight*)light.get();

                if (!light.visible) {
                  shader->setUniform3f(prefix + "intensity", glm::vec3{0});
                  continue;
                }

                shader->setUniform3f(prefix + "position", _light->getAppliedTransformation())
                    ->setUniform3f(prefix + "intensity", _light->getIntensity())
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
          case ObjectType::POINT_LIGHT: {
            auto light = (bloom::PointLight*)object.get();
            glm::vec3 position = object.get()->getPosition();
            glm::mat4 model = glm::mat4(1.0f);

            // Translation
            auto appliedTranslation = light->getAppliedTransformation();
            model = glm::translate(model, appliedTranslation);

            lightShader->bind()
                ->setUniformMat4f("uView", cameraObject->getViewMatrix())
                ->setUniformMat4f("uProjection", cameraObject->getProjectionMatrix())
                ->setUniformMat4f("uW2V", cameraObject->getViewportMatrix())
                ->setUniformMat4f("uModel", model)
                ->setUniform4f("uColor", glm::vec4{1});
            light->draw();
            lightShader->unbind();

            break;
          }
          case ObjectType::CAMERA:
          case ObjectType::AMBIENT_LIGHT:
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

      // Disable visibility checkbox
      if (currentSelected.type != ObjectType::CAMERA
          && currentSelected.type != ObjectType::AMBIENT_LIGHT) {
        bool visible = currentSelected.visible;

        ImGui::Checkbox(
            fmt::format("{} Object visibility", (visible ? (ICON_FA_EYE) : (ICON_FA_EYE_SLASH)))
                .c_str(),
            &visible);

        if (visible != currentSelected.visible) hierarchyObjects[selected].visible = visible;
      }

      auto object = (bloom::Object*)currentSelected.get();
      {  // Transform

        if (instanceof <bloom::AmbientLight, bloom::Camera>(currentSelected.get())) goto common_end;

        glm::vec3 position = object->getAppliedTransformation();

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("Transform");

        ImGui::SliderFloat3("Position", glm::value_ptr(position), -100, 100);

        // Apply position change
        // If the position has changed, we update the object's position
        if (position != object->getPosition()) {
          object->setAppliedTransformation(position);
        }

        glm::vec3 appliedRotation = object->getAppliedRotation();
        glm::vec3 appliedScale = object->getAppliedScale();

        if (! instanceof <bloom::Object>(currentSelected.get())) goto common_end;

        ImGui::SliderFloat3("Rotation", glm::value_ptr(appliedRotation), 0, 360.0f);
        ImGui::SliderFloat3("Scale", glm::value_ptr(appliedScale), 0, 1000.0f);

        if (appliedRotation != object->getAppliedRotation()) {
          object->setAppliedRotation(glm::vec3(appliedRotation));
        }
        if (appliedScale != object->getAppliedScale()) {
          object->setAppliedScale(glm::vec3(appliedScale));
        }

        if (instanceof <bloom::PointLight>(currentSelected.get())) goto common_end;

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("Material");
        ImGui::SameLine();
        HelpMarker("Ambient, diffuse and specular are reflection components of the material");

        glm::vec3 ka = object->getKa();
        glm::vec3 kd = object->getKd();
        glm::vec3 ks = object->getKs();
        int32_t shininess = object->getShininess();

        ImGui::ColorEdit3("Ambient", glm::value_ptr(ka));
        ImGui::ColorEdit3("Diffuse", glm::value_ptr(kd));
        ImGui::ColorEdit3("Specular", glm::value_ptr(ks));
        ImGui::InputInt("Shininess", &shininess);

        if (ka != object->getKa() || kd != object->getKd() || ks != object->getKs()
            || shininess != object->getShininess()) {
          object->setKa(ka);
          object->setKd(kd);
          object->setKs(ks);
          object->setShininess(shininess);
        }

        if (instanceof <bloom::AmbientLight>(currentSelected.get())) goto common_end;
        if (instanceof <bloom::PointLight>(currentSelected.get())) goto common_end;

        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("Lighting model");

        int32_t shading = (int32_t)object->getShading();

        // Create a input list for the shading options
        const char* shadingNames[] = {"Flat", "Gouraud", "Phong"};

        ImGui::Combo("Shading", &shading, shadingNames, IM_ARRAYSIZE(shadingNames),
                     IM_ARRAYSIZE(shadingNames));

        if (shading != (int32_t)object->getShading()) {
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
        case ObjectType::AMBIENT_LIGHT: {
          ImGui::Separator();
          ImGui::Spacing();
          ImGui::Text("Intensity");

          auto light = (bloom::AmbientLight*)object;
          glm::vec3 intensity = light->getIntensity();
          ImGui::ColorEdit3("Intensity", glm::value_ptr(intensity));
          if (intensity != light->getIntensity()) {
            light->setIntensity(intensity);
          }

          break;
        }
        case ObjectType::POINT_LIGHT: {
          ImGui::Separator();
          ImGui::Spacing();
          ImGui::Text("Intensity");

          auto light = (bloom::PointLight*)object;
          glm::vec3 intensity = light->getIntensity();
          ImGui::ColorEdit3("Intensity", glm::value_ptr(intensity));
          if (intensity != light->getIntensity()) {
            light->setIntensity(intensity);
          }

          ImGui::Separator();
          ImGui::Spacing();
          ImGui::Text("Attenuation");

          float constant = light->getConstant();
          float linear = light->getLinear();
          float quadratic = light->getQuadratic();

          ImGui::SliderFloat("Constant", &constant, 0.0f, 1.0f, "%.2f");
          ImGui::SliderFloat("Linear", &linear, 0.0f, 1.0f, "%.3f");
          ImGui::SliderFloat("Quadratic", &quadratic, 0.0f, 1.0f, "%.4f");

          if (constant != light->getConstant() || linear != light->getLinear()
              || quadratic != light->getQuadratic()) {
            light->setConstant(constant);
            light->setLinear(linear);
            light->setQuadratic(quadratic);
          }

          break;
        }
        case ObjectType::CAMERA: {
          ImGui::Separator();
          ImGui::Spacing();
          ImGui::Text("Transform");

          auto position = cameraObject->getPosition();

          ImGui::SliderFloat3("Position", glm::value_ptr(position), -100.0f, 100.0f);

          if (position != cameraObject->getPosition()) {
            cameraObject->setPosition(position);
          }

          ImGui::Separator();

          auto sensibility = cameraObject->getCameraSensitivity();
          auto speed = (float)cameraObject->getCameraSpeed();

          ImGui::SliderFloat("Sensibility", &sensibility, 0.0f, 2.0f);
          ImGui::SliderFloat("Speed", &speed, 0.0f, 30.0f);

          if (sensibility != cameraObject->getCameraSensitivity()
              || speed != cameraObject->getCameraSpeed()) {
            cameraObject->setCameraSensitivity(sensibility);
            cameraObject->setCameraSpeed(speed);
          }

          ImGui::Spacing();
          ImGui::Spacing();
          ImGui::Text("Pipeline");

          // Create a input list for the shading options
          static const char* cameraType[] = {"Perspective", "Axonometric"};

          auto currentType = (int32_t)cameraObject->getCameraType();

          ImGui::Combo("Projection", &currentType, cameraType, IM_ARRAYSIZE(cameraType),
                       IM_ARRAYSIZE(cameraType));

          if (currentType != (int32_t)cameraObject->getCameraType()) {
            cameraObject->changeCameraType((bloom::CameraType)currentType);
          }

          ImGui::Spacing();
          ImGui::Text("Perspective information");

          auto fov = cameraObject->getFieldOfView();
          auto aspect = cameraObject->getAspectRatio();
          auto near = cameraObject->getNearPlane();
          auto far = cameraObject->getFarPlane();
          auto up = cameraObject->getUp();

          ImGui::SliderFloat("Field of View", &fov, 0.0f, 180.0f);
          ImGui::SliderFloat("Aspect Ratio", &aspect, 0.0f, 3.0f);
          ImGui::SliderFloat("Near Plane", &near, 0.0f, 10.0f);
          ImGui::SliderFloat("Far Plane", &far, 0.0f, 2000.0f);
          ImGui::SliderFloat3("Up", glm::value_ptr(up), -1.0f, 1.0f);

          if (fov != cameraObject->getFieldOfView() || aspect != cameraObject->getAspectRatio()
              || near != cameraObject->getNearPlane() || far != cameraObject->getFarPlane()
              || up != cameraObject->getUp()) {
            cameraObject->setFieldOfView(fov);
            cameraObject->setAspectRatio(aspect);
            cameraObject->setNearPlane(near);
            cameraObject->setFarPlane(far);
            cameraObject->setUp(up);
          }

          ImGui::Spacing();
          ImGui::Text("Viewport information");

          auto viewportU = cameraObject->getViewportU();
          auto viewportV = cameraObject->getViewportV();
          auto windowX = cameraObject->getWindowSizeX();
          auto windowY = cameraObject->getWindowSizeY();

          glm::vec2 editableViewportU = glm::vec2(viewportU);
          glm::vec2 editableViewportV = glm::vec2(viewportV);
          glm::vec2 editableWindowX = glm::vec2(windowX);
          glm::vec2 editableWindowY = glm::vec2(windowY);

          ImGui::SliderFloat2("Viewport U", glm::value_ptr(editableViewportU), -1000.0f, 1000.0f);
          ImGui::SliderFloat2("Viewport V", glm::value_ptr(editableViewportV), -1000.0f, 1000.0f);
          ImGui::SliderFloat2("Window X", glm::value_ptr(editableWindowX), -1000.0f, 1000.0f);
          ImGui::SliderFloat2("Window Y", glm::value_ptr(editableWindowY), -1000.0f, 1000.0f);

          if (editableViewportU != viewportU || editableViewportV != viewportV
              || editableWindowX != windowX || editableWindowY != windowY) {
            cameraObject->setViewportU(editableViewportU);
            cameraObject->setViewportV(editableViewportV);
            cameraObject->setWindowSizeX(editableWindowX);
            cameraObject->setWindowSizeY(editableWindowY);
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

          ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ICON_FA_LIGHTBULB);
          ImGui::SameLine();

          if (ImGui::MenuItem(" Light")) m_modalLight = true;

          ImGui::EndMenu();
        }

        if (ImGui::BeginMenu(ICON_FA_GAMEPAD " Scene controllers")) {
          ImGui::Checkbox("Wireframe", &m_wireframe);
          ImGui::Checkbox("Depth buffer", &m_depthBuffer);
          ImGui::Checkbox("Orbit lights", &m_orbitLights);
          ImGui::EndMenu();
        }

        if (ImGui::MenuItem(ICON_FA_FILE_UPLOAD " Load .element", "CTRL+L", false, false)) {
        }

        if (ImGui::MenuItem(ICON_FA_SAVE " Save .element", "CTRL+S", false, false)) {
        }

        if (ImGui::MenuItem(ICON_FA_PAINT_ROLLER
                            " Clear scene")) {  // Clear everything except for the camera
          auto camera = getObjectByType<ObjectType::CAMERA>(0);
          auto ambienteLight = getObjectByType<ObjectType::AMBIENT_LIGHT>(0);
          auto pointLight = getObjectByType<ObjectType::POINT_LIGHT>(0);

          std::vector<Objects>().swap(hierarchyObjects);
          hierarchyObjects.emplace_back(camera);
          hierarchyObjects.emplace_back(ambienteLight);
          hierarchyObjects.emplace_back(pointLight);

          ImGui::EndMenu();
        }
        ImGui::EndPopup();
      }

      {  // Modals
        if (m_modalSphere == true) ImGui::OpenPopup("add_sphere");
        if (m_modalCube == true) ImGui::OpenPopup("add_cube");
        if (m_modalLight == true) ImGui::OpenPopup("add_light");

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

        if (ImGui::BeginPopupModal("add_light", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
          addLight();
          m_modalLight = false;
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

          if (!object.visible) ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

          if (ImGui::Selectable(object.name.c_str(), selected == i)) {
            selected = i;
          }
          if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Delete")) {
              hierarchyObjects.erase(hierarchyObjects.begin() + i);
              selected = -1;
            }
            ImGui::EndPopup();
          }

          if (!object.visible) ImGui::PopStyleColor();
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

      if (!errorMessageSphere.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", errorMessageSphere.c_str());
      }

      ImGui::InputText("Name", namePtrSphere, 64);
      ImGui::SameLine();
      HelpMarker("The name of the sphere *MUST* be unique.\n");
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
        m_editingSphere = false;

        // Check if the name is unique
        std::string name = namePtrSphere;
        if (std::any_of(hierarchyObjects.begin(), hierarchyObjects.end(),
                        [name](const Objects& object) { return object.name == name; })) {
          errorMessageSphere = fmt::format("The name \"{}\" is already in use", name);
          goto not_adding;
        }

        hierarchyObjects.emplace_back(Objects{
            ObjectType::SPHERE,
            namePtrSphere,
            (int32_t)getObjectByType<ObjectType::SPHERE>().size(),
            {.sphere
             = new bloom::Sphere(resultPositionSphere, resultColorSphere, resultRadiusSphere,
                                 resultSectorSphere, resultStackSphere)}});
        ImGui::CloseCurrentPopup();
      }
    not_adding:
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

      if (!m_editingCube) {
        std::string resultName = name ? *name : cubeName;
        namePtrCube = new char[resultName.size() + (size >= 1)];
        std::copy(resultName.begin(), resultName.end(), namePtrCube);

        resultPositionCube = position ? *position : glm::vec3(0.0f);
        resultSideCube = side ? *side : 1.0f;
        resultColorCube = glm::vec3(1.0f, 1.0f, 1.0f);

        m_editingCube = true;
      }

      if (!errorMessageCube.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", errorMessageCube.c_str());
      }

      ImGui::InputText("Name", namePtrCube, 64);
      ImGui::SameLine();
      HelpMarker("The name of the cube *MUST* be unique.\n");
      ImGui::Separator();

      ImGui::InputFloat3("Position", glm::value_ptr(resultPositionCube));
      ImGui::Separator();

      ImGui::InputFloat("Side", &resultSideCube);
      ImGui::Spacing();

      ImGui::ColorEdit3("Color", glm::value_ptr(resultColorCube));
      ImGui::SameLine();
      HelpMarker(
          "\"Color\" this parameter is deceiving, the color of the cube isn't real"
          " it's simpled mapped to Ka and Kd coeficientes of the Material.\n");
      ImGui::Spacing();

      if (ImGui::Button("OK", ImVec2(120, 0))) {
        m_editingCube = false;

        // Check if the name is unique
        std::string name = namePtrCube;
        if (std::any_of(hierarchyObjects.begin(), hierarchyObjects.end(),
                        [name](const Objects& object) { return object.name == name; })) {
          errorMessageCube = fmt::format("The name \"{}\" is already in use", name);
          goto not_adding;
        }

        hierarchyObjects.emplace_back(Objects{
            ObjectType::CUBE,
            namePtrCube,
            (int32_t)getObjectByType<ObjectType::CUBE>().size(),
            {.cube = new bloom::Cube(resultPositionCube, resultSideCube, resultColorCube)}});
        ImGui::CloseCurrentPopup();
      }
    not_adding:
      ImGui::SetItemDefaultFocus();
      ImGui::SameLine();
      if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();

      m_modalCube = false;
    }

    void Light::addLight(std::string* name, glm::vec3* position) {
      const auto size = getObjectByType<ObjectType::POINT_LIGHT>().size();
      const std::string repeated = fmt::format("({})", size);
      const std::string cubeName = fmt::format("Point Light {}", size >= 1 ? repeated : "");

      if (!m_editingLight) {
        std::string resultName = name ? *name : cubeName;
        namePtrLight = new char[resultName.size() + (size >= 1)];
        std::copy(resultName.begin(), resultName.end(), namePtrLight);

        resultPositionLight = position ? *position : glm::vec3(0.0f);
        resultIntensityLight = glm::vec3(1.0f, 1.0f, 1.0f);

        m_editingLight = true;
      }

      if (!errorMessageLight.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", errorMessageLight.c_str());
      }

      ImGui::InputText("Name", namePtrLight, 64);
      ImGui::SameLine();
      HelpMarker("The name of the cube *MUST* be unique.\n");
      ImGui::Separator();

      ImGui::InputFloat3("Position", glm::value_ptr(resultPositionLight));
      ImGui::Separator();

      ImGui::ColorEdit3("Intensity", glm::value_ptr(resultIntensityLight));
      ImGui::Spacing();

      if (ImGui::Button("OK", ImVec2(120, 0))) {
        m_editingLight = false;

        // Check if the name is unique
        std::string name = namePtrLight;
        if (std::any_of(hierarchyObjects.begin(), hierarchyObjects.end(),
                        [name](const Objects& object) { return object.name == name; })) {
          errorMessageLight = fmt::format("The name \"{}\" is already in use", name);
          goto not_adding;
        }

        // Check if there's less than 8 lights
        if (getObjectByType<ObjectType::POINT_LIGHT>().size() >= 8) {
          errorMessageLight = "There's already 8 lights";
          goto not_adding;
        }

        hierarchyObjects.emplace_back(Objects{
            ObjectType::POINT_LIGHT,
            namePtrLight,
            (int32_t)getObjectByType<ObjectType::POINT_LIGHT>().size(),
            {.pointLight = new bloom::PointLight(resultPositionLight, resultIntensityLight)}});
        ImGui::CloseCurrentPopup();
      }
    not_adding:
      ImGui::SetItemDefaultFocus();
      ImGui::SameLine();
      if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();

      m_modalLight = false;
    }

    void Light::enableGuizmo() {
      if (selected == -1) return;

      if (instanceof <bloom::Camera, bloom::AmbientLight>(hierarchyObjects[selected].get())) return;
      if (!hierarchyObjects[selected].visible) return;

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

      ImGuizmo::Manipulate(
          glm::value_ptr(cameraObject->getViewMatrix()),
          glm::value_ptr(cameraObject->getViewportMatrix() * cameraObject->getProjectionMatrix()),
          ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::WORLD, glm::value_ptr(model));

      if (ImGuizmo::IsUsing())
        ((bloom::Object*)selectedModel)->setAppliedTransformation(glm::vec3(model[3]));
    }

    void Light::guizmoController() {
      // Gray background
      ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.00f, 1.00f, 0.13f, 1.00f));
      ImGui::Begin("Controllers");
      {
        // Center buttons in the window
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f);
        bool decoy = false;

        selectableButton(ICON_FA_ARROWS_ALT, &m_canMove);
        selectableButton(m_isPaused ? ICON_FA_PLAY : ICON_FA_PAUSE, &m_isPaused);
        selectableButton(ICON_FA_SEARCH_PLUS, &m_increaseWindow);
        selectableButton(ICON_FA_SEARCH_MINUS, &m_decreaseWindow);
        selectableButton(ICON_FA_PAINT_ROLLER, &decoy);

        if (m_decreaseWindow) {
          auto windowX = cameraObject->getWindowSizeX();
          auto windowY = cameraObject->getWindowSizeY();

          cameraObject->setWindowSizeX(glm::vec2{windowX.x - 1.0, windowX.y + 1.0});
          cameraObject->setWindowSizeY(glm::vec2{windowY.x - 1.0, windowY.y + 1.0});
          m_decreaseWindow = false;
        } else if (m_increaseWindow) {
          auto windowX = cameraObject->getWindowSizeX();
          auto windowY = cameraObject->getWindowSizeY();

          cameraObject->setWindowSizeX(glm::vec2{windowX.x + 1.0, windowX.y - 1.0});
          cameraObject->setWindowSizeY(glm::vec2{windowY.x + 1.0, windowY.y - 1.0});
          m_increaseWindow = false;
        }
      }
      ImGui::End();
      ImGui::PopStyleColor(1);

      ImGuizmo::SetDrawlist(bloom::Renderer::getViewportDrawList());
      ImGuizmo::SetOrthographic(false);

      float windowPosX = bloom::Renderer::getViewportX();
      float windowPosY = bloom::Renderer::getViewportY();

      float windowWidth = bloom::Renderer::getViewportWidth();
      float windowHeight = bloom::Renderer::getViewportHeight();

      ImGuizmo::SetRect(windowPosX, windowPosY, windowWidth, windowHeight);

      float viewManipulateRight = windowPosX + windowWidth;
      float viewManipulateTop = windowPosY;

      auto viewMatrix = cameraObject->getViewMatrix();
      ImGuizmo::ViewManipulate(glm::value_ptr(viewMatrix), -4.f,
                               ImVec2(viewManipulateRight - 128, viewManipulateTop),
                               ImVec2(128, 128), 0x11111110);
    }

    void Light::onImGuiRender() {
      hierarchy();
      inspector();
      if (m_canMove) enableGuizmo();
      guizmoController();

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
  }  // namespace scene
}  // namespace bloom
