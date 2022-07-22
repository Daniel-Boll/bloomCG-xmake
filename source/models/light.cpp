#include <bloomCG/models/light.hpp>

namespace bloom {
  Light::Light(glm::vec3 position) : Sphere(position) {}

  AmbientLight::AmbientLight(glm::vec3 ambientIntensity) : m_ambientIntensity(ambientIntensity) {}

  AmbientLight* AmbientLight::setIntensity(const glm::vec3& ambientIntensity) {
    m_ambientIntensity = ambientIntensity;
    return this;
  }

  glm::vec3 AmbientLight::getIntensity() const { return m_ambientIntensity; }

  glm::vec3 AmbientLight::getPosition() { return (glm::vec3)0; };
  void AmbientLight::setPosition(glm::vec3 position) {}

  PointLight::PointLight(glm::vec3 position, glm::vec3 intensity, float constant, float linear,
                         float quadratic)
      : Light(position),
        m_intensity(intensity),
        m_constant(constant),
        m_linear(linear),
        m_quadratic(quadratic) {}

  PointLight* PointLight::setIntensity(const glm::vec3& intensity) {
    m_intensity = intensity;
    return this;
  }

  PointLight* PointLight::setConstant(const float& constant) {
    m_constant = constant;
    return this;
  }

  PointLight* PointLight::setLinear(const float& linear) {
    m_linear = linear;
    return this;
  }

  PointLight* PointLight::setQuadratic(const float& quadratic) {
    m_quadratic = quadratic;
    return this;
  }

  glm::vec3 PointLight::getIntensity() const { return m_intensity; }

  float PointLight::getConstant() const { return m_constant; }
  float PointLight::getLinear() const { return m_linear; }
  float PointLight::getQuadratic() const { return m_quadratic; }
}  // namespace bloom
