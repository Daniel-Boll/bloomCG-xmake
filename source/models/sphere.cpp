#include <bloomCG/buffers/vertex_buffer_layout.hpp>
#include <bloomCG/core/common.hpp>
#include <bloomCG/core/core.hpp>
#include <bloomCG/models/sphere.hpp>

namespace bloom {
  Sphere::Sphere(glm::vec3 center, glm::vec3 color, float radius, uint16_t sectorCount,
                 uint16_t stackCount)
      : m_center(center), m_radius(radius) {
    m_objectKs = glm::vec3{.5, .5, .5};
    m_objectShininess = 32;
    m_objectKa = color;
    m_objectKd = color;
    m_appliedTransformation = center;
    set(m_radius, sectorCount, stackCount);
  }

  void Sphere::set(float radius, uint16_t sectorCount, uint16_t stackCount) {
    m_radius = radius;
    m_sectorCount = sectorCount;
    m_stackCount = stackCount;

    if (sectorCount < MIN_SECTOR_COUNT) m_sectorCount = MIN_SECTOR_COUNT;
    if (sectorCount < MIN_STACK_COUNT) m_sectorCount = MIN_STACK_COUNT;

    buildVertices();
    bindBuffers();
  }

  void Sphere::bindBuffers() {
    m_vertexBuffer = std::make_unique<bloom::VertexBuffer>(m_vertexData.data(),
                                                           m_vertexData.size() * sizeof(float));

    m_layoutBuffer = new bloom::VertexBufferLayout();
    m_layoutBuffer
        ->push<float>(3)  // Three floats (coordinates)        x, y, z
        .push<float>(3);  // Three floats (normals)            nx, ny, nz

    m_indexBuffer = new bloom::IndexBuffer(m_indices.data(), m_indices.size());

    m_vertexArray = std::make_unique<bloom::VertexArray>();
    m_vertexArray->addBuffer(*m_vertexBuffer, *m_layoutBuffer);
  }

  void Sphere::setRadius(float radius) {
    if (radius != m_radius) set(radius, m_sectorCount, m_stackCount);
  }

  void Sphere::setSectorCount(uint16_t sectorCount) {
    if (sectorCount != m_sectorCount) set(m_radius, sectorCount, m_stackCount);
  }

  void Sphere::setStackCount(uint16_t stackCount) {
    if (stackCount != m_stackCount) set(m_radius, m_sectorCount, stackCount);
  }

  void Sphere::print() {
    fmt::print("==== Sphere ====\n");
    fmt::print("Radius: {:>15}\n", m_radius);
    fmt::print("Sector Count: {:>15}\n", m_sectorCount);
    fmt::print("Stack count: {:>15}\n", m_stackCount);
    fmt::print("Triangle count: {:>15}\n", getTriangleCount());
    fmt::print("Index count: {:>15}\n", getIndexCount());
    fmt::print("Vertices count: {:>15}\n", getVertexCount());
    fmt::print("Positions count: {:>15}\n", getPositionCount());
    fmt::print("Normals count: {:>15}\n", getNormalCount());
  }

  void Sphere::clearVertex() {
    std::vector<float>().swap(m_positions);
    std::vector<float>().swap(m_normals);
    std::vector<uint32_t>().swap(m_indices);
  }

  void Sphere::buildVertices() {
    // Clear old data
    clearVertex();

    float sectorStep = 2 * M_PI / m_sectorCount;
    float stackStep = M_PI / m_stackCount;
    float lengthInverse = 1.0f / m_radius;

    struct Vertex {
      float x, y, z;
      float nx, ny, nz;
    };
    std::vector<Vertex> tmpVertices;

    // Calculate the vertices
    for (uint16_t i = 0; i <= m_stackCount; i++) {
      float stackAngle = M_PI / 2 - i * stackStep;
      float xy = m_radius * cos(stackAngle);
      float zx = m_radius * sin(stackAngle);

      for (uint16_t j = 0; j <= m_sectorCount; j++) {
        float sectorAngle = j * sectorStep;

        float x = xy * cos(sectorAngle);
        float y = xy * sin(sectorAngle);
        float z = zx;

        tmpVertices.push_back({x, y, z, x * lengthInverse, y * lengthInverse, z * lengthInverse});
      }
    }

    clearVertex();

    uint32_t index = 0;
    for (uint16_t i = 0; i < m_stackCount; ++i) {
      uint32_t k1 = i * (m_sectorCount + 1);
      uint32_t k2 = k1 + m_sectorCount + 1;

      for (uint16_t j = 0; j < m_sectorCount; ++j, ++k1, ++k2) {
        Vertex v1 = tmpVertices[k1];
        Vertex v2 = tmpVertices[k2];
        Vertex v3 = tmpVertices[k1 + 1];
        Vertex v4 = tmpVertices[k2 + 1];

        if (i == 0) {
          addPosition(v1.x, v1.y, v1.z);
          addPosition(v2.x, v2.y, v2.z);
          addPosition(v4.x, v4.y, v4.z);

          addNormal(v1.nx, v1.ny, v1.nz);
          addNormal(v2.nx, v2.ny, v2.nz);
          addNormal(v4.nx, v4.ny, v4.nz);

          addIndices(index, index + 1, index + 2);
          index += 3;
        } else if (i == (m_stackCount - 1)) {
          addPosition(v1.x, v1.y, v1.z);
          addPosition(v2.x, v2.y, v2.z);
          addPosition(v3.x, v3.y, v3.z);

          addNormal(v1.nx, v1.ny, v1.nz);
          addNormal(v2.nx, v2.ny, v2.nz);
          addNormal(v3.nx, v3.ny, v3.nz);

          addIndices(index, index + 1, index + 2);
          index += 3;
        } else {
          addPosition(v1.x, v1.y, v1.z);
          addPosition(v2.x, v2.y, v2.z);
          addPosition(v3.x, v3.y, v3.z);
          addPosition(v4.x, v4.y, v4.z);

          addNormal(v1.x, v1.y, v1.z);
          addNormal(v2.x, v2.y, v2.z);
          addNormal(v3.x, v3.y, v3.z);
          addNormal(v4.x, v4.y, v4.z);

          addIndices(index, index + 1, index + 2);
          addIndices(index + 2, index + 1, index + 3);

          index += 4;
        }
      }
    }

    buildVertexData();
  }

  void Sphere::buildVertexData() {
    std::vector<float>().swap(m_vertexData);

    for (std::size_t i = 0; i < m_positions.size(); i += 3) {
      m_vertexData.push_back(m_positions[i]);
      m_vertexData.push_back(m_positions[i + 1]);
      m_vertexData.push_back(m_positions[i + 2]);

      m_vertexData.push_back(m_normals[i]);
      m_vertexData.push_back(m_normals[i + 1]);
      m_vertexData.push_back(m_normals[i + 2]);
    }
  }

  void Sphere::addPosition(float x, float y, float z) {
    m_positions.emplace_back(x);
    m_positions.emplace_back(y);
    m_positions.emplace_back(z);
  }

  void Sphere::addNormal(float nx, float ny, float nz) {
    m_normals.emplace_back(nx);
    m_normals.emplace_back(ny);
    m_normals.emplace_back(nz);
  }

  void Sphere::addIndices(uint32_t a, uint32_t b, uint32_t c) {
    m_indices.emplace_back(a);
    m_indices.emplace_back(b);
    m_indices.emplace_back(c);
  }

  glm::vec3 Sphere::getPosition() { return m_appliedTransformation; }

  void Sphere::setPosition(glm::vec3 position) {
    m_appliedTransformation = position;
    buildVertices();
  }

  void Sphere::shiftVertex() {
    for (std::size_t i = 0; i < m_positions.size(); i += 3) {
      m_positions[i] += m_center.x;
      m_positions[i + 1] += m_center.y;
      m_positions[i + 2] += m_center.z;
    }
  }

  void Sphere::draw() {
    m_vertexArray->bind();
    m_vertexBuffer->bind();
    m_indexBuffer->bind();

    GLCall(glDrawElements(GL_TRIANGLES, m_indexBuffer->getCount(), GL_UNSIGNED_INT, nullptr));

    m_indexBuffer->unbind();
    m_vertexBuffer->unbind();
    m_vertexArray->unbind();
  }
}  // namespace bloom
