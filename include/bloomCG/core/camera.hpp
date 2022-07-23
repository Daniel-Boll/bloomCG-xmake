#pragma once

#include <bloomCG/core/common.hpp>
#include <bloomCG/models/model.hpp>

namespace bloom {

  enum class CameraType { PERSPECTIVE, AXONOMETRIC };

  class Camera : public Entity {
  private:
    glm::vec3 m_cameraPosition, m_cameraFront, m_cameraUp;
    glm::vec3 m_direction;
    double m_yaw, m_pitch;
    CameraType m_cameraType;

    glm::mat4 m_viewMatrix, m_projectionMatrix, m_viewportMatrix;
    glm::vec2 m_windowX, m_windowY;
    glm::vec2 m_viewportV, m_viewportU;

    float m_fov, m_aspectRatio, m_near, m_far;

    double m_cameraSpeed;
    bool m_movementEnabled;
    bool m_mouseMovementEnabled;
    bool m_firstClick;

    float m_cameraSensitivity;

    void regenerateWindow2ViewportMatrix();

  public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f), double yaw = -90.0f,
           double pitch = 0.0f);
    ~Camera();

    void update(double deltaTime);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewportMatrix() const;

    void setViewportU(glm::vec2 u);
    void setViewportV(glm::vec2 v);
    void setWindowSizeX(glm::vec2 x);
    void setWindowSizeY(glm::vec2 y);

    glm::vec2 getViewportU() const;
    glm::vec2 getViewportV() const;
    glm::vec2 getWindowSizeX() const;
    glm::vec2 getWindowSizeY() const;

    Camera* changeCameraType(CameraType cameraType);
    Camera* toggleMovement();
    Camera* toggleMouseMovement();
    Camera* setCameraSpeed(double speed);
    Camera* setCameraSensitivity(float sensitivity);
    Camera* setCameraPosition(glm::vec3 position);
    Camera* setNearPlane(float near);
    Camera* setFarPlane(float far);
    Camera* setFieldOfView(float fov);
    Camera* setAspectRatio(float aspectRatio);
    Camera* setFront(glm::vec3 front);
    Camera* setUp(glm::vec3 up);
    Camera* setYaw(double yaw);
    Camera* setPitch(double pitch);
    void setPosition(glm::vec3 position);

    glm::vec3 getPosition();
    glm::vec3 getFront() const;
    glm::vec3 getUp() const;
    double getYaw() const;
    double getPitch() const;
    float getCameraSensitivity() const;
    float getFieldOfView() const;
    float getAspectRatio() const;
    float getNearPlane() const;
    float getFarPlane() const;
    double getCameraSpeed() const;

    CameraType getCameraType() const;
  };
}  // namespace bloom
