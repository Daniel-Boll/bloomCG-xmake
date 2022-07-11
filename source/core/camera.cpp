#include <bloomCG/core/camera.hpp>
#include <bloomCG/core/core.hpp>

#include "GLFW/glfw3.h"

namespace bloom {
  Camera::Camera(glm::vec3 position, glm::vec3 up, glm::vec3 front, double yaw, double pitch)
      : m_cameraPosition(position),
        m_cameraUp(up),
        m_cameraFront(front),
        m_yaw(yaw),
        m_pitch(pitch) {
    m_movementEnabled = false;
    m_mouseMovementEnabled = false;
  }

  Camera::~Camera() {
    // ¯\_ (ツ)_/¯
  }

  void Camera::update(double deltaTime) {
    float cameraSpeed = m_cameraSpeed * deltaTime;

    if (m_movementEnabled) {
      if (bloom::Input::isKeyPressed(GLFW_KEY_W)) {
        m_cameraPosition += cameraSpeed * m_cameraFront;
      }
      if (bloom::Input::isKeyPressed(GLFW_KEY_S)) {
        m_cameraPosition -= cameraSpeed * m_cameraFront;
      }
      if (bloom::Input::isKeyPressed(GLFW_KEY_A)) {
        m_cameraPosition -= glm::normalize(glm::cross(m_cameraFront, m_cameraUp)) * cameraSpeed;
      }
      if (bloom::Input::isKeyPressed(GLFW_KEY_D)) {
        m_cameraPosition += glm::normalize(glm::cross(m_cameraFront, m_cameraUp)) * cameraSpeed;
      }
    }

    if (m_mouseMovementEnabled) {
      if (bloom::Input::isMouseButtonPressed(bloom::Input::MouseButton::RIGHT)) {
        // glfwSetInputMode(bloom::gl::getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        auto [x, y] = bloom::Input::getMousePosition();
        x *= m_cameraSensitivity;
        y *= m_cameraSensitivity;

        m_yaw += x;
        m_pitch += y;

        // fmt::print("{} {}\n", x, y);

        if (m_pitch > 89.0f) m_pitch = 89.0f;
        if (m_pitch < -89.0f) m_pitch = -89.0f;

        m_direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_direction.y = sin(glm::radians(m_pitch));
        m_direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

        m_cameraFront = glm::normalize(m_direction);
      }
    }
  }

  glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_cameraPosition, m_cameraPosition + m_cameraFront, m_cameraUp);
  }

  glm::mat4 Camera::getProjectionMatrix() const { return m_projectionMatrix; }

  Camera* Camera::changeCameraType(CameraType cameraType, double* args) {
    m_cameraType = cameraType;

    switch (cameraType) {
      case CameraType::AXONOMETRIC: {
        // Project matrix will be identity, so we can just use the camera position and direction
        // to calculate the view matrix.
        m_projectionMatrix = glm::mat4(1.0f);

        // Change 2,2 to 0
        m_projectionMatrix[2][2] = 0.0f;

        break;
      }

      case CameraType::PERSPECTIVE: {
        const double fov = args[0];
        const double aspectRatio = args[1];
        const double nearPlane = args[2];
        const double farPlane = args[3];

        m_projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
      }
    }

    return this;
  }

  Camera* Camera::toggleMovement() {
    m_movementEnabled = !m_movementEnabled;
    return this;
  }

  Camera* Camera::toggleMouseMovement() {
    m_mouseMovementEnabled = !m_mouseMovementEnabled;
    return this;
  }

  Camera* Camera::setCameraSpeed(double speed) {
    m_cameraSpeed = speed;
    return this;
  }

  Camera* Camera::setCameraSensitivity(float sensitivity) {
    m_cameraSensitivity = sensitivity;
    return this;
  }

  void Camera::setPosition(glm::vec3 position) { m_cameraPosition = position; }
  void Camera::setFront(glm::vec3 front) { m_cameraFront = front; }
  void Camera::setUp(glm::vec3 up) { m_cameraUp = up; }
  void Camera::setYaw(double yaw) { m_yaw = yaw; }
  void Camera::setPitch(double pitch) { m_pitch = pitch; }

  glm::vec3 Camera::getPosition() const { return m_cameraPosition; }
  glm::vec3 Camera::getFront() const { return m_cameraFront; }
  glm::vec3 Camera::getUp() const { return m_cameraUp; }

  double Camera::getYaw() const { return m_yaw; }
  double Camera::getPitch() const { return m_pitch; }

  CameraType Camera::getCameraType() const { return m_cameraType; }
}  // namespace bloom
