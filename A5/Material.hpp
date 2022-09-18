// Termm--Fall 2020

#pragma once

class Material {
public:
  virtual ~Material();
  virtual bool isTexture() const = 0;

protected:
  Material();
};
