#pragma once

#include <bloomCG/models/cube.hpp>

namespace bloom {
  // Make Light a subclass of Cube completely
  class Light : public Cube {
  public:
    Light(float sideSize, glm::vec3 position, CubeType type = CubeType::INDEXED);
    ~Light();
  };
}  // namespace bloom
