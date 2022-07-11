#pragma once

#include <bloomCG/buffers/index_buffer.hpp>
#include <bloomCG/buffers/vertex_array.hpp>
#include <bloomCG/buffers/vertex_buffer.hpp>
#include <bloomCG/buffers/vertex_buffer_layout.hpp>
#include <bloomCG/core/camera.hpp>
#include <bloomCG/core/common.hpp>
#include <bloomCG/core/core.hpp>
#include <bloomCG/core/shader.hpp>
#include <bloomCG/models/cube.hpp>
#include <bloomCG/models/light.hpp>
#include <bloomCG/models/sphere.hpp>
#include <bloomCG/scenes/scene.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace bloom {
  namespace scene {
    class Light : public Scene {
    private:
      glm::vec3 m_translation;
      glm::vec3 m_rotationX, m_rotationY, m_rotationZ;

      std::vector<std::unique_ptr<bloom::Cube>> m_cubes;
      std::vector<std::unique_ptr<bloom::Sphere>> m_spheres;
      std::vector<std::unique_ptr<bloom::Light>> m_lights;

      std::unique_ptr<bloom::Camera> m_camera;

      std::unique_ptr<bloom::VertexArray> m_vertexArray;
      std::unique_ptr<bloom::VertexBuffer> m_vertexBuffer;
      std::unique_ptr<bloom::IndexBuffer> m_indexBuffer;
      std::unique_ptr<bloom::Shader> m_objectShader;
      std::unique_ptr<bloom::Shader> m_lightShader;

      glm::vec4 m_lightAmbient = glm::vec4{.7, .7, .7, 1.};
      glm::vec4 m_lightDiffuse = glm::vec4{.5, .5, .5, 1.};
      glm::vec4 m_lightSpecular = glm::vec4{1., 1., 1., 1.};

      int32_t m_sectorCount = 30;
      int32_t m_stackCount = 30;

    public:
      Light();

      void onUpdate(const float deltaTime) override;
      void onRender(const float deltaTime) override;
      void onImGuiRender() override;

      void inspector();
      void hierarchy();
      void addSphere(std::string *name = nullptr, glm::vec3 *position = nullptr,
                     float *radius = nullptr);
      void enableGuizmo();
    };
  }  // namespace scene
}  // namespace bloom
