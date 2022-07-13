#include <bloomCG/models/light.hpp>

namespace bloom {
  Light::Light(glm::vec3 position) : Sphere(position) {}

  Light::~Light() {}

}  // namespace bloom
