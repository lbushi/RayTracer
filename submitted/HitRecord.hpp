
#pragma once
// this will be returned by a "hittable" object(primitive, bounding box...) when querying its intersection with a ray
#include <glm/glm.hpp>
#include "Material.hpp"
class Primitive;
struct HitRecord {
	Primitive* primitive; // what primitive was hit, if any
	bool hit = false; // did it hit or not, if false the remaining fields should be ignored
	float t; // where did the ray it the object
	glm::vec3 Normal; // the normal vector at the point of intersection
	glm::vec3 localcoords; // the local coordinates of the object where the intersection happened. used for texture loookup
	Material* m; // the material at the point of intersection(kd, ks)
    HitRecord(bool hit = false, float t = 0.0f ,glm::vec3 Normal = glm::vec3(0.0f), Material* m = nullptr): hit{ hit }, t{ t }, Normal{Normal}, m{ m }{}
};