// Termm--Fall 2020

#pragma once

#include <iosfwd>

#include <glm/glm.hpp>
#include "GeometryNode.hpp"

// Represents a simple point/area light.
struct Light {
  Light();
  bool is_area = false;
  glm::vec3 colour;
  glm::vec3 position;
  double falloff[3];
  float light_size = 0.5; // in case of area light, the length of the side of the square representing the light
  float get_hit_rate(glm::vec3 point, SceneNode* root, const int samples); // returns the visibility percentage of the light from a certain point
};

std::ostream& operator<<(std::ostream& out, const Light& l);
