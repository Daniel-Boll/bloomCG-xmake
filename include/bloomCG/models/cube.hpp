#pragma once

#include <bloomCG/buffers/index_buffer.hpp>
#include <bloomCG/buffers/vertex_array.hpp>
#include <bloomCG/buffers/vertex_buffer.hpp>
#include <bloomCG/buffers/vertex_buffer_layout.hpp>
#include <bloomCG/core/core.hpp>
#include <bloomCG/core/shader.hpp>
#include <bloomCG/models/model.hpp>

namespace bloom {

  enum class CubeType {
    INDEXED,  // Create a cube using index buffer, which is cheaper and do not repeat vertices,
              // but cannot calculate normals accordingly
    REPEATED  // Create a cube using vertex array, which is more expensive and repeat vertices,
              // but has normals values correct
  };

  class Cube : public Object {
  protected:
    static bloom::IndexBuffer *m_indexBuffer;
    static bloom::VertexBufferLayout *m_vertexBufferIndexedLayout;
    static bloom::VertexBufferLayout *m_vertexBufferRepeatedLayout;

  protected:
    float m_size;
    glm::vec3 m_position;
    CubeType m_type;

    std::vector<float> m_vertices;
    std::vector<glm::vec3> m_normals;
    std::vector<unsigned int> m_indices;

    std::vector<float> m_vertexData;

    bloom::VertexArray *m_vertexArray;
    std::unique_ptr<bloom::VertexBuffer> m_vertexBuffer;

    glm::vec3 m_objectKa, m_objectKd, m_objectKs;
    float m_objectShininess;

    glm::vec3 m_appliedRotation = glm::vec3(0.0f);
    glm::vec3 m_appliedScale = glm::vec3(1.0f);

    void generateIndexedVertices();
    void generateRepeatedVertices();
    void generateNormals();

    void setupBuffers();
    void buildIndexedVertexData();
    void buildRepeatedVertexData();

    uint32_t *getIndices();

  public:
    Cube(glm::vec3 position, float side = 2.f, glm::vec3 color = glm::vec3{.0, 1., 1.},
         CubeType type = CubeType::REPEATED);

    void draw();
    void print();

    glm::vec3 getPosition();
    void setPosition(glm::vec3 position);

    void setSide(float side);
    void setColor(glm::vec3 color);
    void setKa(glm::vec3 ka);
    void setKd(glm::vec3 kd);
    void setKs(glm::vec3 ks);
    void setShininess(float shininess);

    float getSide();
    glm::vec3 getColor();
    glm::vec3 getKa();
    glm::vec3 getKd();
    glm::vec3 getKs();
    float getShininess();

    glm::vec3 getAppliedRotation();
    glm::vec3 getAppliedScale();
    void setAppliedRotation(glm::vec3 rotation);
    void setAppliedScale(glm::vec3 scale);

    ~Cube();
  };
}  // namespace bloom
