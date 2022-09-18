// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"
#include "Ray.hpp"


class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, glm::vec3 reflectivity, bool dielectric, float refractive_index, int glossiness);
  virtual ~PhongMaterial();
  glm::vec3 getkd() const;
  glm::vec3 getks() const;
  double getShininess() const;
  glm::vec3 getreflectivity() const;
  void setDielectric();
  bool isDielectric() const;
  void refract(Ray& ray, glm::vec3 point, glm::vec3 normal) const;
  virtual bool isTexture() const;
  int getglossiness() const;
private:
  float refractive_index = 0.0f;
  bool is_dielectric;
  glm::vec3 m_kd;
  glm::vec3 m_ks;
  glm::vec3 m_reflectivity;
  int glossiness; // number of rays to shoot for glossy reflections, the more the glossier it will look
  double m_shininess;
};
