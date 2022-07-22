#pragma once

#include <bloomCG/core/common.hpp>

namespace bloom {
  class Entity {
  public:
    // Set and get position virtual
    virtual glm::vec3 getPosition() = 0;
    virtual void setPosition(glm::vec3 position) = 0;
  };

  class Object : public Entity {
  public:
    enum class Shading { FLAT, GOURAUD, PHONG };

    glm::vec3 m_objectKa, m_objectKd, m_objectKs;
    float m_objectShininess;

    glm::vec3 m_appliedTransformation = glm::vec3(0.0f);
    glm::vec3 m_appliedRotation = glm::vec3(0.0f);
    glm::vec3 m_appliedScale = glm::vec3(1.0f);

    Shading m_shading = Shading::PHONG;

    glm::vec3 getAppliedRotation();
    glm::vec3 getAppliedScale();
    glm::vec3 getAppliedTransformation();
    void setAppliedRotation(glm::vec3 rotation);
    void setAppliedScale(glm::vec3 scale);
    void setAppliedTransformation(glm::vec3 transformation);

    glm::vec3 getColor();
    glm::vec3 getKa();
    glm::vec3 getKd();
    glm::vec3 getKs();
    float getShininess();
    void setColor(glm::vec3 color);
    void setKa(glm::vec3 ka);
    void setKd(glm::vec3 kd);
    void setKs(glm::vec3 ks);
    void setShininess(float shininess);

    Shading getShading();
    void setShading(Shading shading);

    virtual void draw() = 0;
  };
}  // namespace bloom
