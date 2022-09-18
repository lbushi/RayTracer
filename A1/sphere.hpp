#pragma once
#include <vector>
#include <glm/glm.hpp>

// Utility functions, constants and structs that generate vertices which together approximate a sphere.

// A face is simply a collection of 3 vertices in 3d space.
// It is used to represent the face of a triangular bipyramid.
struct Face {
    float vertex1[3];
    float vertex2[3];
    float vertex3[3];
    glm::vec3 normal1;
    glm::vec3 normal2;
    glm::vec3 normal3;
    Face(const float const * vertex1, const float const * vertex2, const float const *  vertex3);
    // The triangulate function splits a face into 4 smaller triangles by constructing its medial triangle.
    std::vector<Face> triangulate();
    void update_normals(const float * center, const float radius);
    // Normalize the vertices of a face so that all of them have distance radius from center
    void normalize_vertices(const float* center, const float radius);
};

// the recursive function recursive_sphere will process each face, that is, the batch of 4 triangles that are produced by each 
// face triangulation in the following way. It will first normalize the vertices of each face in order to 
// make them equidistant from the center of the sphere, which is passed as a parameter together with the radius,
// and after normalizing them it further triangulates them and inputs them to itself recursively.
// It does this up to the level smoothness_level, which is a predefined value that dictates how smooth the 
// resulting sphere will appear. 
static std::vector<Face> recursive_sphere(std::vector<Face>& faces, const float * center, const float radius, const int smoothness_so_far, 
const int smoothness_level);

// The create_sphere function creates an approximation of a sphere with a given center and radius.
// It accomplishes this through first contructing a triangular bipyramid. After consructing the bipyramid,
// it calls the function above in order to recursively triangulate and normalize the faces of the bipyramid
// so that it will start to resemble a sphere as we go deeper in the recursion stack.
std::vector<Face> create_sphere(const float * center, const float radius, const int smoothness_level);
