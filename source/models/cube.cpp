#include <bloomCG/buffers/index_buffer.hpp>
#include <bloomCG/core/core.hpp>
#include <bloomCG/models/cube.hpp>

namespace bloom {
  Cube::Cube(glm::vec3 position, float side, glm::vec3 color, CubeType type)
      : m_size(side), m_position(position), m_type(type), m_objectKa(color), m_objectKd(color) {
    m_objectKs = glm::vec3{.5, .5, .5};
    m_objectShininess = 32;

    if (m_type == CubeType::INDEXED)
      generateIndexedVertices();
    else
      generateRepeatedVertices();

    setupBuffers();
  }

  Cube::~Cube() {}

  void Cube::generateIndexedVertices() {
    // Generate the vertices of the cube based on the size and position
    auto addVertex = [this](float x, float y, float z) {
      m_vertices.push_back(x);
      m_vertices.push_back(y);
      m_vertices.push_back(z);
    };

    addVertex(m_position.x - m_size, m_position.y - m_size, m_position.z - m_size);
    addVertex(m_position.x + m_size, m_position.y - m_size, m_position.z - m_size);
    addVertex(m_position.x + m_size, m_position.y + m_size, m_position.z - m_size);
    addVertex(m_position.x - m_size, m_position.y + m_size, m_position.z - m_size);
    addVertex(m_position.x - m_size, m_position.y - m_size, m_position.z + m_size);
    addVertex(m_position.x + m_size, m_position.y - m_size, m_position.z + m_size);
    addVertex(m_position.x + m_size, m_position.y + m_size, m_position.z + m_size);
    addVertex(m_position.x - m_size, m_position.y + m_size, m_position.z + m_size);

    // Generate the indices of the cube
    // Front
    float front[6] = {0, 1, 2, 2, 3, 0};
    // Back
    float back[6] = {4, 5, 6, 6, 7, 4};
    // Left
    float left[6] = {4, 0, 3, 3, 7, 7};
    // Right
    float right[6] = {1, 5, 6, 6, 2, 1};
    // Top
    float top[6] = {1, 0, 4, 4, 5, 1};
    // Bottom
    float bottom[6] = {3, 2, 6, 6, 7, 3};

    // Add the indices to the cube
    auto addIndices = [&](float* indices, int size) {
      for (int i = 0; i < size; i++) {
        m_indices.push_back(indices[i]);
      }
    };

    addIndices(front, 6);
    addIndices(back, 6);
    addIndices(left, 6);
    addIndices(right, 6);
    addIndices(top, 6);
    addIndices(bottom, 6);

    buildIndexedVertexData();
  }

  void Cube::generateRepeatedVertices() {
    std::vector<float>().swap(m_vertices);
    m_vertices.reserve(36 * 3);

    auto addVertex = [this](float x, float y, float z) {
      m_vertices.push_back(x);
      m_vertices.push_back(y);
      m_vertices.push_back(z);
    };

    // Generate all the 36 vertices of the cube (36 vertices because each vertex is repeated 4
    // times)
    addVertex(m_position.x - m_size, m_position.y - m_size, m_position.z - m_size);
    addVertex(m_position.x + m_size, m_position.y - m_size, m_position.z - m_size);
    addVertex(m_position.x + m_size, m_position.y + m_size, m_position.z - m_size);
    addVertex(m_position.x + m_size, m_position.y + m_size, m_position.z - m_size);
    addVertex(m_position.x - m_size, m_position.y + m_size, m_position.z - m_size);
    addVertex(m_position.x - m_size, m_position.y - m_size, m_position.z - m_size);

    addVertex(m_position.x - m_size, m_position.y - m_size, m_position.z + m_size);
    addVertex(m_position.x + m_size, m_position.y - m_size, m_position.z + m_size);
    addVertex(m_position.x + m_size, m_position.y + m_size, m_position.z + m_size);
    addVertex(m_position.x + m_size, m_position.y + m_size, m_position.z + m_size);
    addVertex(m_position.x - m_size, m_position.y + m_size, m_position.z + m_size);
    addVertex(m_position.x - m_size, m_position.y - m_size, m_position.z + m_size);

    addVertex(m_position.x - m_size, m_position.y + m_size, m_position.z + m_size);
    addVertex(m_position.x - m_size, m_position.y + m_size, m_position.z - m_size);
    addVertex(m_position.x - m_size, m_position.y - m_size, m_position.z - m_size);
    addVertex(m_position.x - m_size, m_position.y - m_size, m_position.z - m_size);
    addVertex(m_position.x - m_size, m_position.y - m_size, m_position.z + m_size);
    addVertex(m_position.x - m_size, m_position.y + m_size, m_position.z + m_size);

    addVertex(m_position.x + m_size, m_position.y + m_size, m_position.z + m_size);
    addVertex(m_position.x + m_size, m_position.y + m_size, m_position.z - m_size);
    addVertex(m_position.x + m_size, m_position.y - m_size, m_position.z - m_size);
    addVertex(m_position.x + m_size, m_position.y - m_size, m_position.z - m_size);
    addVertex(m_position.x + m_size, m_position.y - m_size, m_position.z + m_size);
    addVertex(m_position.x + m_size, m_position.y + m_size, m_position.z + m_size);

    addVertex(m_position.x - m_size, m_position.y - m_size, m_position.z - m_size);
    addVertex(m_position.x + m_size, m_position.y - m_size, m_position.z - m_size);
    addVertex(m_position.x + m_size, m_position.y - m_size, m_position.z + m_size);
    addVertex(m_position.x + m_size, m_position.y - m_size, m_position.z + m_size);
    addVertex(m_position.x - m_size, m_position.y - m_size, m_position.z + m_size);
    addVertex(m_position.x - m_size, m_position.y - m_size, m_position.z - m_size);

    addVertex(m_position.x - m_size, m_position.y + m_size, m_position.z - m_size);
    addVertex(m_position.x + m_size, m_position.y + m_size, m_position.z - m_size);
    addVertex(m_position.x + m_size, m_position.y + m_size, m_position.z + m_size);
    addVertex(m_position.x + m_size, m_position.y + m_size, m_position.z + m_size);
    addVertex(m_position.x - m_size, m_position.y + m_size, m_position.z + m_size);
    addVertex(m_position.x - m_size, m_position.y + m_size, m_position.z - m_size);

    generateNormals();
    buildRepeatedVertexData();
  }

  void Cube::setupBuffers() {
    m_vertexBuffer = std::make_unique<bloom::VertexBuffer>(m_vertexData.data(),
                                                           m_vertexData.size() * sizeof(float));

    // Vertex buffer layout and index buffer are the EXACT same for every single instance
    // of the cube class, so the static member is declared in order to accomplish the share
    // of this same data.
    if (m_vertexBufferIndexedLayout == nullptr || m_vertexBufferRepeatedLayout == nullptr
        || m_indexBuffer == nullptr) {
      if (m_type == CubeType::INDEXED) {
        m_vertexBufferIndexedLayout = new bloom::VertexBufferLayout();
        m_vertexBufferIndexedLayout->push<float>(3);  // Position
      } else {
        m_vertexBufferRepeatedLayout = new bloom::VertexBufferLayout();
        m_vertexBufferRepeatedLayout
            ->push<float>(3)  // Position
            .push<float>(3);  // Normal
      }

      if (m_type == CubeType::INDEXED) {
        m_indexBuffer = new bloom::IndexBuffer(m_indices.data(), m_indices.size());
      }
    }

    m_vertexArray = new bloom::VertexArray();
    m_vertexArray->addBuffer(*m_vertexBuffer, m_type == CubeType::INDEXED
                                                  ? *m_vertexBufferIndexedLayout
                                                  : *m_vertexBufferRepeatedLayout);
  }

  void Cube::generateNormals() {
    std::vector<glm::vec3>().swap(m_normals);
    m_normals.resize(36);

    m_normals[0] = glm::vec3(0.0f, 0.0f, -1.0f);
    m_normals[1] = glm::vec3(0.0f, 0.0f, 1.0f);
    m_normals[2] = glm::vec3(-1.0f, 0.0f, 0.0f);
    m_normals[3] = glm::vec3(1.0f, 0.0f, 0.0f);
    m_normals[4] = glm::vec3(0.0f, -1.0f, 0.0f);
    m_normals[5] = glm::vec3(0.0f, 1.0f, 0.0f);
  }

  uint32_t* Cube::getIndices() { return m_indices.data(); }

  void Cube::buildIndexedVertexData() {
    std::vector<float>().swap(m_vertexData);
    m_vertexData.reserve(8 * 3);  // 8 vertices * 3 components

    // At every 3 vertices position
    for (int i = 0; i < 24; i += 3) {
      m_vertexData.push_back(m_vertices[i]);
      m_vertexData.push_back(m_vertices[i + 1]);
      m_vertexData.push_back(m_vertices[i + 2]);
    }
  }

  void Cube::buildRepeatedVertexData() {
    std::vector<float>().swap(m_vertexData);
    m_vertexData.reserve(36 * 6);  // 36 vertices * 6 components (3 position + 3 normal)

    for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 18; j += 3) {
        m_vertexData.push_back(m_vertices[i * 18 + j]);
        m_vertexData.push_back(m_vertices[i * 18 + j + 1]);
        m_vertexData.push_back(m_vertices[i * 18 + j + 2]);
        m_vertexData.push_back(m_normals[i].x);
        m_vertexData.push_back(m_normals[i].y);
        m_vertexData.push_back(m_normals[i].z);
      }
    }
  }

  glm::vec3 Cube::getPosition() { return m_position; }

  void Cube::print() {
    fmt::print("\nCube:\n");

    fmt::print("Normals: ");
    for (int i = 0; i < m_normals.size(); i++) {
      fmt::print("({:2},{:2},{:2})\n", m_normals[i].x, m_normals[i].y, m_normals[i].z);
    }

    if (m_type == CubeType::INDEXED) {
      fmt::print("Vertices\n");
      for (int i = 0; i < m_vertexData.size() - 1; i += 3) {
        fmt::print("({:2},{:2},{:2})\n", m_vertexData[i], m_vertexData[i + 1], m_vertexData[i + 2],
                   m_vertexData[i + 3]);
      }
    } else {
      fmt::print("Vertices\t\tNormal\n");
      for (int i = 0; i < m_vertexData.size() - 1; i += 6) {
        fmt::print("({:2},{:2},{:2})\t\t({:2},{:2},{:2})\n", m_vertexData[i], m_vertexData[i + 1],
                   m_vertexData[i + 2], m_vertexData[i + 3], m_vertexData[i + 4],
                   m_vertexData[i + 5]);
      }
    }
  }

  void Cube::setPosition(glm::vec3 position) { m_position = position; }

  void Cube::draw() {
    m_vertexArray->bind();
    m_vertexBuffer->bind();
    if (m_type == CubeType::INDEXED) {
      m_indexBuffer->bind();
      GLCall(
          glad_glDrawElements(GL_TRIANGLES, m_indexBuffer->getCount(), GL_UNSIGNED_INT, nullptr));
      m_indexBuffer->unbind();
    } else {
      GLCall(glad_glDrawArrays(GL_TRIANGLES, 0, 36));
    }

    m_vertexArray->unbind();
    m_vertexBuffer->unbind();
  }

  void Cube::setSide(float side) {
    m_size = side;
    if (m_type == CubeType::INDEXED)
      generateIndexedVertices();
    else
      generateRepeatedVertices();

    setupBuffers();
  }
  void Cube::setColor(glm::vec3 color) {
    m_objectKa = color;
    m_objectKd = color;
  }
  void Cube::setKa(glm::vec3 ka) { m_objectKa = ka; }
  void Cube::setKd(glm::vec3 kd) { m_objectKd = kd; }
  void Cube::setKs(glm::vec3 ks) { m_objectKs = ks; }
  void Cube::setShininess(float shininess) { m_objectShininess = shininess; }

  glm::vec3 Cube::getAppliedRotation() { return m_appliedRotation; }
  glm::vec3 Cube::getAppliedScale() { return m_appliedScale; }
  void Cube::setAppliedRotation(glm::vec3 rotation) { m_appliedRotation = rotation; }
  void Cube::setAppliedScale(glm::vec3 scale) { m_appliedScale = scale; }

  float Cube::getSide() { return m_size; }
  glm::vec3 Cube::getColor() { return m_objectKa; }
  glm::vec3 Cube::getKa() { return m_objectKa; }
  glm::vec3 Cube::getKd() { return m_objectKd; }
  glm::vec3 Cube::getKs() { return m_objectKs; }
  float Cube::getShininess() { return m_objectShininess; }

  IndexBuffer* Cube::m_indexBuffer = nullptr;
  VertexBufferLayout* Cube::m_vertexBufferIndexedLayout = nullptr;
  VertexBufferLayout* Cube::m_vertexBufferRepeatedLayout = nullptr;
}  // namespace bloom
