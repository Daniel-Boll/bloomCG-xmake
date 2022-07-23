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

      if (bloom::Input::isKeyPressed(GLFW_KEY_SPACE)) {
        m_cameraPosition += glm::vec3(0.0f, cameraSpeed, 0.0f);
      }

      if (bloom::Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        m_cameraPosition -= glm::vec3(0.0f, cameraSpeed, 0.0f);
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

  glm::mat4 Camera::getViewportMatrix() const { return m_viewportMatrix; }

  void Camera::setViewportU(glm::vec2 u) {
    m_viewportU = u;

    regenerateWindow2ViewportMatrix();
  }

  void Camera::setViewportV(glm::vec2 v) {
    m_viewportV = v;

    regenerateWindow2ViewportMatrix();
  }

  void Camera::setWindowSizeX(glm::vec2 x) {
    m_windowX = x;

    regenerateWindow2ViewportMatrix();
  }

  void Camera::setWindowSizeY(glm::vec2 y) {
    m_windowY = y;

    regenerateWindow2ViewportMatrix();
  }

  glm::vec2 Camera::getViewportU() const { return m_viewportU; }
  glm::vec2 Camera::getViewportV() const { return m_viewportV; }
  glm::vec2 Camera::getWindowSizeX() const { return m_windowX; }
  glm::vec2 Camera::getWindowSizeY() const { return m_windowY; }

  void Camera::regenerateWindow2ViewportMatrix() {
    m_viewportMatrix = glm::mat4(0.0f);

    // .x --> min
    // .y --> max

    m_viewportMatrix[0][0] = (m_viewportU.y - m_viewportU.x) / (m_windowX.y - m_windowX.x);
    m_viewportMatrix[0][3] = -m_windowX.x * (m_viewportMatrix[0][0]) + m_viewportU.x;
    m_viewportMatrix[1][1] = (m_viewportV.y - m_viewportV.x) / (m_windowY.y - m_windowY.x);
    m_viewportMatrix[1][3] = m_windowY.x * (m_viewportMatrix[1][1]) + m_viewportV.y;
    m_viewportMatrix[2][2] = 1;
    m_viewportMatrix[3][3] = 1;
  }

  Camera* Camera::changeCameraType(CameraType cameraType) {
    m_cameraType = cameraType;

    switch (cameraType) {
      case CameraType::AXONOMETRIC: {
        // Project matrix will be identity, so we can just use the camera position and direction
        // to calculate the view matrix.
        m_projectionMatrix = glm::mat4(1.0f);

        // Change 2,2 to 0 (z-axis)
        m_projectionMatrix[2][2] = 0.0f;

        break;
      }

      case CameraType::PERSPECTIVE: {
        m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near, m_far);
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

  Camera* Camera::setCameraPosition(glm::vec3 position) {
    m_cameraPosition = position;
    return this;
  }

  Camera* Camera::setNearPlane(float near) {
    m_near = near;
    changeCameraType(m_cameraType);
    return this;
  }

  Camera* Camera::setFarPlane(float far) {
    m_far = far;
    changeCameraType(m_cameraType);
    return this;
  }

  Camera* Camera::setFieldOfView(float fov) {
    m_fov = fov;
    changeCameraType(m_cameraType);
    return this;
  }

  Camera* Camera::setAspectRatio(float aspectRatio) {
    m_aspectRatio = aspectRatio;
    changeCameraType(m_cameraType);
    return this;
  }

  Camera* Camera::setFront(glm::vec3 front) {
    m_cameraFront = front;
    return this;
  }

  Camera* Camera::setUp(glm::vec3 up) {
    m_cameraUp = up;
    changeCameraType(m_cameraType);
    return this;
  }

  Camera* Camera::setYaw(double yaw) {
    m_yaw = yaw;
    return this;
  }

  Camera* Camera::setPitch(double pitch) {
    m_pitch = pitch;
    return this;
  }

  glm::vec3 Camera::getPosition() { return m_cameraPosition; }
  void Camera::setPosition(glm::vec3 position) { m_cameraPosition = position; }
  glm::vec3 Camera::getFront() const { return m_cameraFront; }
  glm::vec3 Camera::getUp() const { return m_cameraUp; }

  double Camera::getYaw() const { return m_yaw; }
  double Camera::getPitch() const { return m_pitch; }

  float Camera::getCameraSensitivity() const { return m_cameraSensitivity; }
  float Camera::getFieldOfView() const { return m_fov; }
  float Camera::getAspectRatio() const { return m_aspectRatio; }
  float Camera::getNearPlane() const { return m_near; }
  float Camera::getFarPlane() const { return m_far; }
  double Camera::getCameraSpeed() const { return m_cameraSpeed; }

  CameraType Camera::getCameraType() const { return m_cameraType; }
}  // namespace bloom
