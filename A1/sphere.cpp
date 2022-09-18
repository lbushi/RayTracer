#include "sphere.hpp"
#include <iostream>

// A simple helper function that transfers data from a glm::vec3 type to an array of 3 floats
void vec3_to_array(glm::vec3 from, float * to) {
    to[0] = from.x;
    to[1] = from.y;
    to[2] = from.z;
}

Face::Face(const float const * vertex1, const float const * vertex2, const float const *  vertex3) {
        this->vertex1[0] = vertex1[0];
        this->vertex1[1] = vertex1[1];
        this->vertex1[2] = vertex1[2];
        this->vertex2[0] = vertex2[0];
        this->vertex2[1] = vertex2[1];
        this->vertex2[2] = vertex2[2];
        this->vertex3[0] = vertex3[0];
        this->vertex3[1] = vertex3[1];
        this->vertex3[2] = vertex3[2];
}

std::vector<Face> Face::triangulate() {
        float midpoint12[3];
        float midpoint13[3];
        float midpoint23[3];
        for (int i = 0; i < 3; ++i) {
            midpoint12[i] = (vertex1[i] + vertex2[i]) / 2;
            midpoint13[i] = (vertex1[i] + vertex3[i]) / 2;
            midpoint23[i] = (vertex2[i] + vertex3[i]) / 2;
        }
        Face f1{vertex1, midpoint13, midpoint12};
        Face f2{vertex2, midpoint23, midpoint12};
        Face f3{vertex3, midpoint13, midpoint23};
        Face f4{midpoint12, midpoint13, midpoint23};
        std::vector<Face> faces{f1, f2, f3, f4};
        return faces;
}

void Face::normalize_vertices(const float * center, const float radius) {
    auto vertex1_vec3 = glm::vec3(vertex1[0], vertex1[1], vertex1[2]);
    auto vertex2_vec3 = glm::vec3(vertex2[0], vertex2[1], vertex2[2]);
    auto vertex3_vec3 = glm::vec3(vertex3[0], vertex3[1], vertex3[2]);
    auto center_vec3 = glm::vec3(center[0], center[1], center[2]);
    vertex1_vec3 = center_vec3 + radius * glm::normalize(glm::vec3(vertex1_vec3 - center_vec3));
    vertex2_vec3 = center_vec3 + radius * glm::normalize(glm::vec3(vertex2_vec3 - center_vec3));
    vertex3_vec3 = center_vec3 + radius * glm::normalize(glm::vec3(vertex3_vec3 - center_vec3));
    vec3_to_array(vertex1_vec3, vertex1);
    vec3_to_array(vertex2_vec3, vertex2);
    vec3_to_array(vertex3_vec3, vertex3);
}

void Face::update_normals(const float * center, const float radius) {
    auto center_vec3 = glm::vec3(center[0], center[1], center[2]);
    auto vertex1_vec3 = glm::vec3(vertex1[0], vertex1[1], vertex1[2]);
    auto vertex2_vec3 = glm::vec3(vertex2[0], vertex2[1], vertex2[2]);
    auto vertex3_vec3 = glm::vec3(vertex3[0], vertex3[1], vertex3[2]);
    normal1 = glm::normalize(glm::cross(glm::vec3(vertex2[0], vertex2[1], vertex2[2])- 
    glm::vec3(vertex1[0], vertex1[1], vertex1[2]), glm::vec3(vertex3[0], vertex3[1], vertex3[2]) - 
    glm::vec3(vertex1[0], vertex1[1], vertex1[2])));
    if (glm::distance(vertex1_vec3 + 0.1f * normal1, center_vec3) < radius) normal1 = -normal1;
    normal2 = glm::normalize(glm::cross(glm::vec3(vertex3[0], vertex3[1], vertex3[2])- 
    glm::vec3(vertex2[0], vertex2[1], vertex2[2]), glm::vec3(vertex1[0], vertex1[1], vertex1[2]) - 
    glm::vec3(vertex2[0], vertex2[1], vertex2[2])));
    if (glm::distance(vertex2_vec3 + 0.1f * normal2, center_vec3) < radius) normal2 = -normal2;
    normal3 = glm::normalize(glm::cross(glm::vec3(vertex2[0], vertex2[1], vertex2[2])- 
    glm::vec3(vertex3[0], vertex3[1], vertex3[2]), glm::vec3(vertex1[0], vertex1[1], vertex1[2]) - 
    glm::vec3(vertex3[0], vertex3[1], vertex3[2])));
    if (glm::distance(vertex3_vec3 + 0.1f * normal3, center_vec3) < radius) normal3 = -normal3;
}
static std::vector<Face> recursive_sphere(std::vector<Face>& faces, const float * center, const float radius, 
const int smoothness_so_far, const int smoothness_level){
    if (smoothness_so_far == smoothness_level) {
        return faces;
    }
    std::vector<Face> result;
    for (auto face:faces) {
        face.normalize_vertices(center, radius);
        auto sub_faces = face.triangulate();
        auto rec_vector = recursive_sphere(sub_faces, center, radius, smoothness_so_far + 1, smoothness_level);
        result.insert(result.end(), rec_vector.begin(), rec_vector.end());
    }
    return result;
}

std::vector<Face> create_sphere(const float * center, const float radius, const int smoothness_level) {
    float base_vertex1[3];
    float base_vertex2[3];
    float base_vertex3[3];
    float top_vertex[3];
    float bottom_vertex[3];
    glm::vec3 base_vertex1_vec3;
    glm::vec3 base_vertex2_vec3;
    glm::vec3 base_vertex3_vec3;
    glm::vec3 top_vertex_vec3;
    glm::vec3 bottom_vertex_vec3;
    glm::vec3 center_vec3(center[0], center[1], center[2]);
    base_vertex1_vec3 = center_vec3 + radius * glm::vec3(glm::cos(glm::radians(0.0f)), 0.0f, glm::sin(glm::radians(0.0f)));
    base_vertex2_vec3 = center_vec3 + radius * glm::vec3(glm::cos(glm::radians(120.0f)), 0.0f, glm::sin(glm::radians(120.0f)));
    base_vertex3_vec3 = center_vec3 + radius * glm::vec3(glm::cos(glm::radians(240.0f)), 0.0f, glm::sin(glm::radians(240.0f)));
    top_vertex_vec3 = center_vec3 + radius * glm::vec3(0.0f, 1.0f, 0.0f);
    bottom_vertex_vec3 = center_vec3 + radius * glm::vec3(0.0f, -1.0f, 0.0f);
    vec3_to_array(base_vertex1_vec3, base_vertex1);
    vec3_to_array(base_vertex2_vec3, base_vertex2);
    vec3_to_array(base_vertex3_vec3, base_vertex3);
    vec3_to_array(top_vertex_vec3, top_vertex);
    vec3_to_array(bottom_vertex_vec3, bottom_vertex);
    Face f1{base_vertex1, base_vertex2, base_vertex3};
    Face f2{top_vertex, base_vertex1, base_vertex2};
    Face f3{top_vertex, base_vertex1, base_vertex3};
    Face f4{top_vertex, base_vertex2, base_vertex3};
    Face f5{ base_vertex1, base_vertex2, base_vertex3};
    Face f6{bottom_vertex, base_vertex1, base_vertex2};
    Face f7{bottom_vertex, base_vertex1, base_vertex3};
    Face f8{bottom_vertex, base_vertex2, base_vertex3};
    std::vector<Face> faces{f1, f2, f3, f4, f5, f6, f7, f8};
    return recursive_sphere(faces, center, radius, 0, smoothness_level);
}