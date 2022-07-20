#include <bloomCG/models/model.hpp>

namespace bloom {
  Entity::Entity() {}

  void Object::setColor(glm::vec3 color) {
    m_objectKd = color;
    m_objectKa = color;
  }
  void Object::setKa(glm::vec3 ka) { m_objectKa = ka; }
  void Object::setKd(glm::vec3 kd) { m_objectKd = kd; }
  void Object::setKs(glm::vec3 ks) { m_objectKs = ks; }
  void Object::setShininess(float shininess) { m_objectShininess = shininess; }

  glm::vec3 Object::getColor() { return m_objectKd; }
  glm::vec3 Object::getKa() { return m_objectKa; }
  glm::vec3 Object::getKd() { return m_objectKd; }
  glm::vec3 Object::getKs() { return m_objectKs; }
  float Object::getShininess() { return m_objectShininess; }

  glm::vec3 Object::getAppliedRotation() { return m_appliedRotation; }
  glm::vec3 Object::getAppliedScale() { return m_appliedScale; }
  glm::vec3 Object::getAppliedTransformation() { return m_appliedTransformation; }
  void Object::setAppliedRotation(glm::vec3 rotation) { m_appliedRotation = rotation; }
  void Object::setAppliedScale(glm::vec3 scale) { m_appliedScale = scale; }
  void Object::setAppliedTransformation(glm::vec3 transformation) {
    m_appliedTransformation = transformation;
  }

  Object::Shading Object::getShading() { return m_shading; }
  void Object::setShading(Shading shading) { m_shading = shading; }

}  // namespace bloom
