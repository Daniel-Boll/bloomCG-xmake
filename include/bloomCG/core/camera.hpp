#pragma once

#include <bloomCG/core/common.hpp>

namespace bloom {

  enum class CameraType { PERSPECTIVE, AXONOMETRIC };

  class Camera {
  private:
    glm::vec3 m_cameraPosition, m_cameraFront, m_cameraUp;
    glm::vec3 m_direction;
    double m_yaw, m_pitch;
    CameraType m_cameraType;

    glm::mat4 m_viewMatrix, m_projectionMatrix;

    double m_cameraSpeed;
    bool m_movementEnabled;
    bool m_mouseMovementEnabled;
    bool m_firstClick;

    float m_cameraSensitivity;

  public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f), double yaw = -90.0f,
           double pitch = 0.0f);
    ~Camera();

    void update(double deltaTime);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    Camera* changeCameraType(CameraType cameraType, double* args);
    Camera* toggleMovement();
    Camera* toggleMouseMovement();
    Camera* setCameraSpeed(double speed);
    Camera* setCameraSensitivity(float sensitivity);

    void setPosition(glm::vec3 position);
    void setFront(glm::vec3 front);
    void setUp(glm::vec3 up);
    void setYaw(double yaw);
    void setPitch(double pitch);

    glm::vec3 getPosition() const;
    glm::vec3 getFront() const;
    glm::vec3 getUp() const;
    double getYaw() const;
    double getPitch() const;

    CameraType getCameraType() const;
  };
}  // namespace bloom
