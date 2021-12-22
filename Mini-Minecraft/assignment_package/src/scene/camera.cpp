#include "camera.h"
#include "glm_includes.h"

Camera::Camera(glm::vec3 pos)
    : Camera(400, 400, pos)
{}

Camera::Camera(unsigned int w, unsigned int h, glm::vec3 pos)
    : Entity(pos), m_fovy(45), m_width(w), m_height(h),
      m_near_clip(0.1f), m_far_clip(1000.f), m_aspect(w / static_cast<float>(h))
{}

Camera::Camera(const Camera &c)
    : Entity(c),
      m_fovy(c.m_fovy),
      m_width(c.m_width),
      m_height(c.m_height),
      m_near_clip(c.m_near_clip),
      m_far_clip(c.m_far_clip),
      m_aspect(c.m_aspect)
{}


void Camera::setWidthHeight(unsigned int w, unsigned int h) {
    m_width = w;
    m_height = h;
    m_aspect = w / static_cast<float>(h);
}


void Camera::tick(float dT, InputBundle &input) {
    // Do nothing
}

glm::mat4 Camera::getViewProj() const {
    return glm::perspective(glm::radians(m_fovy), m_aspect, m_near_clip, m_far_clip) * glm::lookAt(m_position, m_position + m_forward, m_up);
}

glm::mat4 Camera::getView() const
{
    return glm::lookAt(m_position, m_position + m_forward, m_up);
}

glm::mat4 Camera::getProj() const
{
    return glm::perspective(glm::radians(m_fovy), m_aspect, m_near_clip, m_far_clip);
}

void Camera::setZ(float z)
{
    this->m_position.z += z;
}
void Camera::setY(float y)
{
    this->m_position.y += y;
}
void Camera::setDirection(glm::vec3 d)
{
    this->m_position += d;
}

void Camera::setFov(float fov)
{
    this->m_fovy = fov;
}

void Camera::reset(glm::vec3 dir)
{
    this->m_up = glm::vec3(0.f, 1.f, 0.f);
    this->m_forward = glm::normalize(glm::vec3(dir.x, 0.f, dir.z));
    this->m_right = glm::cross(m_forward, m_up);
}

void Camera::rotateOnUpPolar(float degrees, glm::vec3 pos)
{
    float rad = glm::radians(degrees);

    m_forward = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(0,1,0)) * glm::vec4(m_forward, 0.f));
    m_right = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(0,1,0)) * glm::vec4(m_right, 0.f));
    m_up = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(0,1,0)) * glm::vec4(m_up, 0.f));

    m_position -= pos;
    this->m_position =  glm::rotateY(m_position, rad);
    m_position += pos;
}


void Camera::rotateOnRightPolar(float degrees, glm::vec3 pos)
{
    float rad = glm::radians(degrees);
    m_forward = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(1,0,0)) * glm::vec4(m_forward, 0.f));
    m_right = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(1,0,0)) * glm::vec4(m_right, 0.f));
    m_up = glm::vec3(glm::rotate(glm::mat4(), rad, glm::vec3(1,0,0)) * glm::vec4(m_up, 0.f));

    m_position -= pos;
    this->m_position =  glm::rotateX(m_position, rad);
    m_position += pos;
}
