#include "TextureMaterial.hpp"
#include <iostream>
TextureMaterial::~TextureMaterial() {

}

TextureMaterial::TextureMaterial(unsigned char* data, int width, int height, int channels): data{data}, width{width}, height{height}, channels{channels}{}


glm::vec3 TextureMaterial::getkd(float x, float y) const {
    glm::vec3 result;
    auto image_x = x * width;
    auto image_y = y * height;
    glm::vec2 nearest_texels[4];
    nearest_texels[0] = glm::vec2(glm::floor(image_x), glm::floor(image_y));
    nearest_texels[1] = glm::vec2(glm::min(glm::floor(image_x) + 1, float(width - 1)), glm::floor(image_y));
    nearest_texels[2] = glm::vec2(glm::floor(image_x), glm::min(glm::floor(image_y) + 1, float(height - 1)));
    nearest_texels[3] = glm::vec2(glm::min(glm::floor(image_x) + 1, float(width - 1)), glm::min(glm::floor(image_y) + 1, float(height - 1)));
    float distances[4];
    float sum = 0;
    for (int i = 0; i < 4; ++i) {
        distances[i] = glm::distance(glm::vec2(image_x, image_y), nearest_texels[i]);
        sum += distances[i];
    }
    for (int i = 0; i < 4; ++i) {
        result += (sum - distances[i]) / sum * glm::vec3(float(data[channels * (int(nearest_texels[i].y) * width + int(nearest_texels[i].x))]) / 255.0f,
       float( data[channels * (int(nearest_texels[i].y) * width + int(nearest_texels[i].x)) + 1]) / 255.0f,
        float( data[channels * (int(nearest_texels[i].y) * width + int(nearest_texels[i].x)) + 2]) / 255.0f);
    }
    return result / 3.0f;
}

bool TextureMaterial::isTexture() const {
    return 1;
}

glm::vec3 TextureMaterial::getks() const {
    return glm::vec3(0.0f);
}
glm::vec3 TextureMaterial::getreflectivity() const {
    return glm::vec3(0.0f);
}