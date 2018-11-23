#pragma once 

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

class View {
  glm::vec2 _center;
  glm::vec2 _radius;
  glm::mat4 _proj;
  bool _dirty;

public:
  View& center(GLfloat cx, GLfloat cy) {
    _center = glm::vec2(cx, cy);
    _dirty = true;
    return *this;
  }

  View& radius(GLfloat rx, GLfloat ry) {
    _radius = glm::vec2(rx, ry);
    _dirty = true;
    return *this;
  }

  glm::mat4& proj() {
    if (_dirty) computeProj();
    return _proj;
  }

  float left()   { return _center[0] - _radius[0]; }
  float right()  { return _center[0] + _radius[0]; }
  float top()    { return _center[1] + _radius[1]; }
  float bottom() { return _center[1] - _radius[1]; }


  bool dirty() { return _dirty; }

  void computeProj() {
    _proj = glm::ortho(left(), right(), 
                       top(), bottom(),  
                       -1.f, 1.f);
    _dirty = false;
  }
};