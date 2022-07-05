#include <imgui.h>

#include <bloomCG/core/core.hpp>
#include <bloomCG/scenes/light.hpp>
#include <cmath>
#include <memory>

namespace bloom {
  namespace scene {
    Light::Light() : m_translation(0.0f, 0.0f, 0.0f) {
      m_lastTime = 0;
      m_direction = 1;

      GLCall(glad_glEnable(GL_BLEND));
      GLCall(glad_glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
      GLCall(glad_glEnable(GL_DEPTH_TEST));

      // ================ Setting up Camera ================
      glm::vec3 position = glm::vec3(2.5f, 3.0f, 7.0f);

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
          ->setCameraSensitivity(.3);
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
          std::make_unique<bloom::Sphere>(glm::vec3{0, 0, 0}, 2., m_sectorCount, m_stackCount));
      // m_spheres.push_back(std::make_unique<bloom::Sphere>(glm::vec3{0, 0, 0}, 1., 7, 8));

      // m_cubes[0]->print();

      m_objectShader = std::make_unique<bloom::Shader>(
          "/home/danielboll/dev/learning/cpp/bloomCG-xmake/assets/shaders/cube.shader.glsl");
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
          "/home/danielboll/dev/learning/cpp/bloomCG-xmake/assets/shaders/light.shader.glsl");
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

    void Light::onImGuiRender() {
      ImGui::ShowDemoWindow();

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

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

  }  // namespace scene
}  // namespace bloom
