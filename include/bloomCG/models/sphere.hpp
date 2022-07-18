#pragma once

#include <bloomCG/buffers/index_buffer.hpp>
#include <bloomCG/buffers/vertex_array.hpp>
#include <bloomCG/buffers/vertex_buffer_layout.hpp>
#include <bloomCG/core/common.hpp>
#include <bloomCG/core/core.hpp>
#include <bloomCG/core/shader.hpp>
#include <bloomCG/models/model.hpp>

namespace bloom {
  class Sphere : public Object {
  protected:
    bloom::IndexBuffer* m_indexBuffer;
    bloom::VertexBufferLayout* m_layoutBuffer;

  private:
    std::unique_ptr<bloom::VertexArray> m_vertexArray;
    std::unique_ptr<bloom::VertexBuffer> m_vertexBuffer;

  private:
    static const int MIN_SECTOR_COUNT = 3;
    static const int MIN_STACK_COUNT = 2;
    static const int POSITIONS_PER_VERTEX = 3;
    static const int NORMALS_PER_VERTEX = 3;

    glm::vec3 m_center;
    float m_radius;
    uint16_t m_sectorCount, m_stackCount;

    std::vector<float> m_positions, m_normals;
    std::vector<uint32_t> m_indices;

    std::vector<float> m_vertexData;  // Combination of (position, normal)

  public:
    Sphere(glm::vec3 center, glm::vec3 color = glm::vec3{1., .0, .0}, float radius = 0.2,
           uint16_t sectorCount = 30, uint16_t stackCount = 30);
    ~Sphere() {}

    // Getters
    constexpr float getRadius() const { return m_radius; }
    constexpr uint16_t getSectorCount() const { return m_sectorCount; }
    constexpr uint16_t getStackCount() const { return m_stackCount; }

    // Setters
    void set(float radius, uint16_t sectorCount, uint16_t stackCount);
    void setRadius(float radius);
    void setSectorCount(uint16_t sectorCount);
    void setStackCount(uint16_t stackCount);
    void setColor(glm::vec3 color);
    void setKa(glm::vec3 ka);
    void setKd(glm::vec3 kd);
    void setKs(glm::vec3 ks);
    void setShininess(float shininess);
    void setPosition(glm::vec3 position);
    float getShininess();

    glm::vec3 getPosition();
    glm::vec3 getColor();
    glm::vec3 getKa();
    glm::vec3 getKd();
    glm::vec3 getKs();

    // Functionalities
    void draw();
    glm::vec3 getAppliedRotation();
    glm::vec3 getAppliedScale();
    void setAppliedRotation(glm::vec3 rotation);
    void setAppliedScale(glm::vec3 scale);
    // Debug
    void print();

  private:
    // Private member functions
    void bindBuffers();

    void buildVertices();
    void buildVertexData();

    void clearVertex();

    void addPosition(float x, float y, float z);
    void addNormal(float x, float y, float z);
    void addTextureCoordinates(float s, float t);

    // Vertex data
    uint16_t getPositionCount() const {
      return (uint16_t)m_positions.size() / Sphere::POSITIONS_PER_VERTEX;
    }
    uint16_t getNormalCount() const {
      return (uint16_t)m_normals.size() / Sphere::NORMALS_PER_VERTEX;
    }
    uint16_t getIndexCount() const { return (uint16_t)m_indices.size(); }

    uint16_t getTriangleCount() const { return getIndexCount() / 3; }
    uint16_t getVertexCount() const { return (uint16_t)m_vertexData.size() / 6; }

    uint16_t getPositionSize() const { return (uint16_t)m_positions.size() * sizeof(float); }
    uint16_t getNormalSize() const { return (uint16_t)m_normals.size() * sizeof(float); }
    uint16_t getIndexSize() const { return (uint16_t)m_indices.size() * sizeof(uint16_t); }
    uint16_t getVertexDataSize() const { return (uint16_t)m_vertexData.size() * sizeof(float); }

    const float* getPositions() const { return m_positions.data(); }
    const float* getNormals() const { return m_normals.data(); }
    const uint32_t* getIndices() const { return m_indices.data(); }
    const float* getVertexData() const { return m_vertexData.data(); }

    // Shift vertex based on new center position
    void shiftVertex();

    void addIndices(uint32_t a, uint32_t b, uint32_t c);

    std::vector<float> computeFaceNormal(float x1, float y1, float z1, float x2, float y2, float z2,
                                         float x3, float y3, float z3);
  };
}  // namespace bloom
