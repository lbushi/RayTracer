// Termm--Fall 2020

#include "PhongMaterial.hpp"

// The following are self-explanatory.

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess , glm::vec3 reflectivity, bool dielectric, float refractive_index, int glossiness)
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
	, m_reflectivity(reflectivity)
	, is_dielectric{ dielectric }
	, refractive_index { refractive_index} 
	, glossiness{ glossiness }
{}

PhongMaterial::~PhongMaterial()
{}
glm::vec3 PhongMaterial::getkd() const {
	return m_kd;
}
glm::vec3 PhongMaterial::getks() const {
	return m_ks;
}


int PhongMaterial::getglossiness() const {
	return glossiness;
}

glm::vec3 PhongMaterial::getreflectivity() const {
	return m_reflectivity;
}

double PhongMaterial::getShininess() const {
	return m_shininess;
}

void PhongMaterial::setDielectric() {
	is_dielectric = true;
}
bool PhongMaterial::isDielectric() const {
	return is_dielectric;
}

bool PhongMaterial::isTexture() const {
	return 0;
}

void PhongMaterial::refract(Ray& ray, glm::vec3 point, glm::vec3 normal) const {
	ray.direction = glm::refract(glm::normalize(ray.direction), normal, ray.mode == Mode::IN ? 1 / refractive_index : refractive_index);
	ray.mode = ray.mode == Mode::IN ? Mode::OUT : Mode::IN;
	ray.origin = point;
	return;
}