#pragma once
#include <glm/glm.hpp>


// this is an enum that has meaning only when considered in a ray. It tells us whether the ray is going inside(in)
// a material as opposed to when it is going outside(out) i.e towards air. This is needed for dielectric materials.

enum class Mode { IN, OUT };

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
    Mode mode = Mode::IN; // default is IN
    Ray(glm::vec3 origin, glm::vec3 direction): origin{ origin }, direction{ direction }{}
    void transform_ray(const glm::mat4& mat) {
        origin = glm::vec3(mat * glm::vec4(origin, 1.0f));
	    direction = glm::vec3(glm::mat3(mat) * direction);
    }
};