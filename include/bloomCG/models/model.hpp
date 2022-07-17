#pragma once

#include <bloomCG/core/common.hpp>

namespace bloom {
  class Entity {
  public:
    Entity();

    // Set and get position virtual
    virtual glm::vec3 getPosition() = 0;
    virtual void setPosition(glm::vec3 position) = 0;
  };

  class Object : public Entity {
  public:
    virtual glm::vec3 getAppliedRotation() = 0;
    virtual glm::vec3 getAppliedScale() = 0;
    virtual void setAppliedRotation(glm::vec3 rotation) = 0;
    virtual void setAppliedScale(glm::vec3 scale) = 0;

    virtual glm::vec3 getColor() = 0;
    virtual glm::vec3 getKa() = 0;
    virtual glm::vec3 getKd() = 0;
    virtual glm::vec3 getKs() = 0;
    virtual float getShininess() = 0;
    virtual void setKa(glm::vec3 ka) = 0;
    virtual void setKd(glm::vec3 kd) = 0;
    virtual void setKs(glm::vec3 ks) = 0;
    virtual void setShininess(float shininess) = 0;

    virtual void draw() = 0;
  };
}  // namespace bloom
