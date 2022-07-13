#pragma once

#include <bloomCG/core/common.hpp>

namespace bloom {
  class Model {
  public:
    Model();

    // Set and get position virtual
    virtual glm::vec3 getPosition() = 0;
    virtual void setPosition(glm::vec3 position) = 0;
  };
}  // namespace bloom
