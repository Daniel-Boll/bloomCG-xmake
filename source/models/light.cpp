#include <bloomCG/models/light.hpp>

namespace bloom {
  Light::Light(float sideSize, glm::vec3 position, CubeType type) : Cube(sideSize, position, type) {
    m_type = type;
    m_position = position;
    m_size = sideSize;
  }

  Light::~Light() {}

}  // namespace bloom
