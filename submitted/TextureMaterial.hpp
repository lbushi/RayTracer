// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"


class TextureMaterial : public Material {
public:
  TextureMaterial(unsigned char* data, int width, int height, int channels);
  virtual ~TextureMaterial();
  glm::vec3 getkd(float x, float y) const;
  glm::vec3 getks() const;
  glm::vec3 getreflectivity() const;
  virtual bool isTexture() const;
private:
  unsigned char* data; //  the actual data containg the pixel colours
  int width; 
  int height;
  int channels; // channels in a texture(RGB, RGBA etc...)
};
