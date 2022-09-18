// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>
#include "HitRecord.hpp"
#include "Ray.hpp"
#include <glm/gtx/transform.hpp>
const float epsilon = 0.0001f;

bool is_within(float val, float expected, float epsilon);


class Primitive {
public:
  virtual HitRecord intersect(Ray ray) { return HitRecord();}; 
  virtual glm::vec3 getNormal(glm::vec3 point) {return glm::vec3(0.0f);};
  virtual glm::vec2 uv_coords(glm::vec3 point);
  virtual ~Primitive();
};

class Sphere : public Primitive {
  glm::vec3 center = glm::vec3(0.0f);
  double radius = 1.0f;
public:
  Sphere(){};
  virtual glm::vec2 uv_coords(glm::vec3 point) override; 
  virtual HitRecord intersect(Ray ray) override;
  virtual glm::vec3 getNormal(glm::vec3 point) override;
  virtual ~Sphere();
};

class Cube : public Primitive {
  // we model a cube as a collection of 6 squares i.e its faces
  struct Square {
    glm::vec3 center;
    float side;
    Square(){};
    Square(glm::vec3 center, float side): center{center}, side{side} {}
    HitRecord intersect(Ray ray) {
      if (center.z == 0.0f || center.z == -1.0f) {
        float t = (center.z - ray.origin.z) / ray.direction.z;
        float xpos = ray.origin.x + t * ray.direction.x;
        float ypos = ray.origin.y + t * ray.direction.y;
        return HitRecord(xpos <= 1.0f && xpos >= 0.0f && ypos <= 1.0f && ypos >= 0.0f, t, glm::vec3(0.0f), nullptr);
      }
      else if (center.y == 0.0f || center.y == 1.0f) {
        float t = (center.y - ray.origin.y) / ray.direction.y;
        float xpos = ray.origin.x + t * ray.direction.x;
        float zpos = ray.origin.z + t * ray.direction.z;
        return HitRecord(xpos <= 1.0f && xpos >= 0.0f && zpos <= 0.0f && zpos >= -1.0f, t, glm::vec3(0.0f), nullptr);
      }
      else if (center.x == 0.0f || center.x == 1.0f) {
        float t = (center.x - ray.origin.x) / ray.direction.x;
        float ypos = ray.origin.y + t * ray.direction.y;
        float zpos = ray.origin.z + t * ray.direction.z;
        return HitRecord(ypos <= 1.0f && ypos >= 0.0f && zpos <= 0.0f && zpos >= -1.0f, t, glm::vec3(0.0f), nullptr);
      }
      return HitRecord();
    }

  };
  Square faces[6];
  glm::vec3 normals[6];
public:
  Cube();
  virtual glm::vec2 uv_coords(glm::vec3 point) override;
  virtual HitRecord intersect(Ray ray) override;
  virtual glm::vec3 getNormal(glm::vec3 point) override;
  virtual ~Cube();
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
    transform_to_unit_sphere = glm::scale(glm::mat4(1.0f), glm::vec3(1 / m_radius)) * glm::translate(glm::mat4(1.0f), -m_pos);
  }
  virtual HitRecord intersect(Ray ray) override;
  virtual glm::vec2 uv_coords(glm::vec3 point) override;
  virtual glm::vec3 getNormal(glm::vec3 point) override;
  virtual ~NonhierSphere();

private:
  glm::vec3 m_pos;
  double m_radius;
  Sphere m_sphere;
  glm::mat4 transform_to_unit_sphere;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
    transform_to_unit_cube = glm::scale(glm::mat4(1.0f), glm::vec3(1 / size)) * glm::translate(glm::mat4(1.0f), -pos);
  }
  virtual HitRecord intersect(Ray ray) override;
  virtual glm::vec2 uv_coords(glm::vec3 point) override;
  virtual glm::vec3 getNormal(glm::vec3 point) override;
  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;
  Cube m_cube;
  glm::mat4 transform_to_unit_cube;
};

// A Cylinder Primitive
class Cylinder: public Primitive {
  glm::vec3 center = glm::vec3(0.0f);
  double radius = 1.0f; // the radius of the base of the cyllinder
  double height = 1.0f; // the height of the cyllinder
  public:
  Cylinder(){};
  virtual HitRecord intersect(Ray ray) override;
  virtual glm::vec2 uv_coords(glm::vec3 point) override;
  virtual glm::vec3 getNormal(glm::vec3 point) override;
  virtual ~Cylinder();
};

// A Cone primitive
class Cone: public Primitive {
  glm::vec3 center = glm::vec3(0.0f);
  double radius = 1.0f; // the radius of the base of the cone
  double height = 1.0f; //  the height of the cone
  public:
  Cone(){};
  virtual HitRecord intersect(Ray ray) override;
  virtual glm::vec2 uv_coords(glm::vec3 point) override;
  virtual glm::vec3 getNormal(glm::vec3 point) override;
  virtual ~Cone();
};