// Termm--Fall 2020

#include <iostream>

#include <glm/ext.hpp>

#include "Light.hpp"

Light::Light()
  : colour(0.0, 0.0, 0.0),
    position(0.0, 0.0, 0.0)
{
  falloff[0] = 1.0;
  falloff[1] = 0.0;
  falloff[2] = 0.0;
}

std::ostream& operator<<(std::ostream& out, const Light& l)
{
  out << "L[" << glm::to_string(l.colour) 
  	  << ", " << glm::to_string(l.position) << ", ";
  for (int i = 0; i < 3; i++) {
    if (i > 0) out << ", ";
    out << l.falloff[i];
  }
  out << "]";
  return out;
}

// returns the hit rate of a light from a certain point on the scene. If not an area light return zero cause 
// point lights are handled separately by the lighting model
float Light::get_hit_rate(glm::vec3 point, SceneNode* root, const int samples) {
  int samples_hit = 0;
  if (!is_area) return 0.0f;
  for (int i = 0; i < samples; ++i) {
    for (int j = 0; j < samples; ++j) {
      auto sample_point = position + glm::vec3(i * light_size / samples - light_size / 2, j * light_size / samples - light_size / 2, 0.0f);
      Ray ray = Ray(point + epsilon * 10 * glm::normalize(sample_point - point), sample_point - point);
      auto info = root->intersect(ray);
      if (!(info.hit)) {
        samples_hit += 1;
      }
    }
  }
  return samples_hit / float(samples * samples); 
}