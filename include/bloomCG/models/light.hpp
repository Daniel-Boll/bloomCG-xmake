#pragma once

#include <bloomCG/models/sphere.hpp>

namespace bloom {
  // Make Light a subclass of Cube completely
  class Light : public Sphere {
  public:
    Light(glm::vec3 position);
    ~Light();
  };
}  // namespace bloom
