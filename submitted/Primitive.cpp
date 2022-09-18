// Termm--Fall 2020

#include "Primitive.hpp"
#include "polyroots.hpp"
#include <iostream>

// Below we have the implementation of intersection and getNormal functions for various primitives supported by
// the ray tracer such as cones, cubes, boxes, spheres. Each intersection function is specific to the primitive 
// and is just computational geometry that uses the provided stable quadratic solver to find intersection points.
// There is also a uv_coords function that is implemented for all of them but is trivial for all except for the sphere
// which means out of the primitives here only the sphere supports texturing.
Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

Cylinder::~Cylinder() 
{
}

Cone::~Cone() 
{
}
// returns a normalized normal to the sphere at the point given
glm::vec3 Sphere::getNormal(glm::vec3 point) {
    return glm::normalize(point - center);
}

glm::vec2 Primitive::uv_coords(glm::vec3 point) {
    return glm::vec2(0.0f, 0.0f);
}


// map point on sphere to texture space coords. Simply take spherical coordinates without the radius.
glm::vec2 Sphere::uv_coords(glm::vec3 point) {
    auto proj_length = glm::length(glm::vec2(point.x, point.z));
    float theta, psi;
    if (point.z <= 0.0f && point.x >= 0.0f) {
        theta = -glm::asin(point.z / proj_length);
    }
    else if (point.z <= 0.0f && point.x <= 0.0f) {
        theta = glm::pi<float>() + glm::asin(point.z / proj_length);

    }
    else if (point.z >= 0.0f && point.x >= 0.0f) {
        theta = 2.0f * glm::pi<float>() - glm::asin(point.z / proj_length);
    }
    else {
        theta = glm::pi<float>() + glm::asin(point.z / proj_length);
    }
    theta /= (2.0f * glm::pi<float>());
    psi = glm::asin(point.y / glm::length(point));
    psi = (psi + glm::pi<float>() / 2) / (glm::pi<float>());
    return glm::vec2(theta, 1 - psi);
}

// bunch of trivial implementations for the other primives. Needed in order to reap the benefits of polymorphism and
// is generally a good idea to do cause these might be filled at a later time.
glm::vec2 Cone::uv_coords(glm::vec3 point) {return glm::vec2();};
glm::vec2 Cube::uv_coords(glm::vec3 point) {return glm::vec2();};
glm::vec2 NonhierBox::uv_coords(glm::vec3 point) {return glm::vec2();};
glm::vec2 NonhierSphere::uv_coords(glm::vec3 point) {return glm::vec2();};
glm::vec2 Cylinder::uv_coords(glm::vec3 point) {return glm::vec2();};


HitRecord Sphere::intersect(Ray ray) {
    auto o = ray.origin;
    auto d = ray.direction;
    auto c = center;
    double a = d.x*d.x + d.y*d.y + d.z * d.z;
    double b = 2*(o.x*d.x + o.y*d.y + o.z*d.z - c.x*d.x - c.y*d.y - c.z*d.z);
    double coeff = o.x*o.x + o.y*o.y + o.z*o.z  + c.x*c.x + c.y*c.y + c.z*c.z - 2 * c.x * o.x - 2 * c.y * o.y - 2 * c.z * o.z - radius*radius;
    double roots[2];
    if (quadraticRoots(a, b, coeff, &roots[0])) {
        // there are real solutions
        auto root = glm::min(roots[0], roots[1]);
        auto point = o + float(root) * d;
        auto retval = HitRecord(true, root, getNormal(point), nullptr);
        retval.localcoords = ray.origin + float(root) * ray.direction;
        return retval;
    }
    else {
        return HitRecord();
    }
}

Cube::Cube() {
        faces[0] = Square(glm::vec3(0.5f, 0.5f, 0.0f), 1.0f);
        normals[0] = glm::vec3(0.0f, 0.0f, 1.0f);
        faces[1] = Square(glm::vec3(0.5f, 0.5f, -1.0f), 1.0f);
        normals[1] = glm::vec3(0.0f, 0.0f, -1.0f);
        faces[2] = Square(glm::vec3(0.0f, 0.5f, -0.5f), 1.0f);
        normals[2] = glm::vec3(-1.0f, 0.0f, 0.0f);
        faces[3] = Square(glm::vec3(1.0f, 0.5f, -0.5f), 1.0f);
        normals[3] = glm::vec3(1.0f, 0.0f, 0.0f);
        faces[4] = Square(glm::vec3(0.5f, 0.0f, -0.5f), 1.0f);
        normals[4] = glm::vec3(0.0f, -1.0f, 0.0f);
        faces[5] = Square(glm::vec3(0.5f, 1.0f, -0.5f), 1.0f);
        normals[5] = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 Cube::getNormal(glm::vec3 point) {
    if (is_within(point.z, 0.0f, epsilon)) {
        return normals[0];
    }
    else if (is_within(point.z, -1.0f, epsilon)) {
        return normals[1];
    }
    else if (is_within(point.x, 0.0f, epsilon)) {
        return normals[2];
    }
    else if (is_within(point.x, 1.0f, epsilon)) {
        return normals[3];
    }
    else if (is_within(point.y, 0.0f, epsilon)) {
        return normals[4];
    }
    else if (is_within(point.y, 1.0f, epsilon)) {
        return normals[5];
    }
    return glm::vec3(0.0f);
}

HitRecord Cube::intersect(Ray ray) {
    HitRecord record;
    for (int i = 0; i < 6; ++i) {
        auto facerecord = faces[i].intersect(ray);
        if (facerecord.hit && (record.hit == false || facerecord.t < record.t)) {
            record = facerecord;
        }
    }

    record.Normal = getNormal(ray.origin + record.t * ray.direction);
    return record;
}

Cube::~Cube()
{
}

NonhierSphere::~NonhierSphere()
{
}

HitRecord NonhierSphere::intersect(Ray ray) {
    ray.transform_ray(transform_to_unit_sphere);
    HitRecord record = m_sphere.intersect(ray);
    return record;
}
glm::vec3 NonhierSphere::getNormal(glm::vec3 point) {
    return glm::mat3(glm::transpose(transform_to_unit_sphere)) * (m_sphere.getNormal(glm::vec3(transform_to_unit_sphere * glm::vec4(point, 1.0f))));
}

NonhierBox::~NonhierBox()
{
}
HitRecord NonhierBox::intersect(Ray ray) {
    ray.transform_ray(transform_to_unit_cube);
    HitRecord record = m_cube.intersect(ray);
    return record;
}
glm::vec3 NonhierBox::getNormal(glm::vec3 point) {
    return glm::mat3(glm::transpose(transform_to_unit_cube)) * (m_cube.getNormal(glm::vec3(transform_to_unit_cube * glm::vec4(point, 1.0f))));
}



bool is_within(float val, float expected, float epsilon) {
    return glm::abs(expected - val) <= epsilon;
}

HitRecord Cylinder::intersect(Ray ray) {
    auto o = ray.origin;
    auto d = ray.direction;
    auto c = center;
    double a = d.x * d.x + d.z * d.z;
    double b = 2* ( o.x * d.x + o.z * d.z);
    double coeff = o.x * o.x + o.z * o.z - radius * radius;
    double roots[2];
    double t = -1.0f;
    bool hit = false;
    if (quadraticRoots(a, b, coeff, &roots[0])) {
        // there are real solutions
        auto root = glm::min(roots[0], roots[1]);
        auto point = o + float(root) * d;
        if (point.y >= 0.0f && point.y <= height) {
            hit = true;
            t = root;
        }
    }
    auto t0 = (height - o.y) / d.y;
    if ((o.x + t0 * d.x) * (o.x + t0 * d.x) + (o.z + t0 * d.z) * (o.z + t0 * d.z) <= radius * radius) {
        if (hit) {
            t = glm::min(t, t0);
        }
        else {
            hit = true;
            t = t0;
        }
    }
    auto t1 = - o.y / d.y;
    if ((o.x + t1 * d.x) * (o.x + t1 * d.x) + (o.z + t1 * d.z) * (o.z + t1 * d.z) <= radius * radius) {
        if (hit) {
            t = glm::min(t, double(t1));
        }
        else {
            hit = true;
            t = t1;
        }
    }
    return HitRecord(hit, t, getNormal(o + float(t) * d), nullptr);
}

glm::vec3 Cylinder::getNormal(glm::vec3 point) {
    if (is_within(point.x * point.x + point.z * point.z, radius*radius, epsilon)) {
        auto retval = glm::normalize(point - center);
        retval.y = 0.0f;
        return retval;
    }
    else {
        if (is_within(point.y, height, epsilon)) {
            return glm::vec3(0.0f, 1.0f, 0.0f);
        }
        else {
            return glm::vec3(0.0f, -1.0f, 0.0f);
        }
    }
}

HitRecord Cone::intersect(Ray ray) {
    auto o = ray.origin;
    auto d = ray.direction;
    auto c = center;
    double a = d.x * d.x + d.z * d.z - d.y * d.y;
    double b = 2 * ( o.x * d.x + o.z * d.z + d.y - o.y * d.y);
    double coeff = o.x * o.x + o.z * o.z + 2 * o.y - o.y * o.y - 1;
    double roots[2];
    if (quadraticRoots(a, b, coeff, &roots[0])) {
        // there are real solutions
        auto root = glm::min(roots[0], roots[1]);
        auto point = o + float(root) * d;
        if (point.y > 1.0f || point.y < 0.0f) return HitRecord();
        return HitRecord(true, root, getNormal(point), nullptr);
    }
    else {
        return HitRecord();
    }
}

glm::vec3 Cone::getNormal(glm::vec3 point) {
    if (is_within(point.y, 0, epsilon)) {
        return glm::vec3(0.0f, -1.0f, 0.0f);
    }
    else {
        return glm::normalize(glm::vec3(2 * point.x, 2 - 2 * point.y, 2 * point.z));
    }
}
