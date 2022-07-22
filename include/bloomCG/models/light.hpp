#pragma once

#include <bloomCG/models/sphere.hpp>

#include "bloomCG/models/model.hpp"

namespace bloom {
  // Create this class just for verbose purposes.
  // TODO: Make light entity, and then PointLight inherit exclusively from it.
  class Light : public Sphere {
  public:
    explicit Light(glm::vec3 position);
  };

  class AmbientLight : public Entity {
  private:
    glm::vec3 m_ambientIntensity;

  public:
    explicit AmbientLight(glm::vec3 ambientIntensity);

    glm::vec3 getPosition() override;
    void setPosition(glm::vec3 position) override;

    AmbientLight* setIntensity(const glm::vec3& ambientIntensity);
    glm::vec3 getIntensity() const;
  };

  class PointLight : public Light {
  private:
    glm::vec3 m_intensity;
    float m_constant;
    float m_linear;
    float m_quadratic;

  public:
    explicit PointLight(glm::vec3 position, glm::vec3 intensity = glm::vec3{1},
                        float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f);

    PointLight* setIntensity(const glm::vec3& intensity);
    PointLight* setConstant(const float& constant);
    PointLight* setLinear(const float& linear);
    PointLight* setQuadratic(const float& quadratic);

    glm::vec3 getIntensity() const;

    float getConstant() const;
    float getLinear() const;
    float getQuadratic() const;
  };
}  // namespace bloom
