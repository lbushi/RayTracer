// Termm--Fall 2020

#include <glm/ext.hpp>

#include "A5.hpp"
#include "Primitive.hpp"
#include "PhongMaterial.hpp"
#include "TextureMaterial.hpp"
#include "Mesh.hpp"
#include "PerlinNoise.hpp"
#include <iostream>

// control panel of the ray tracer as a whole
// Note that depth_of_field, focal_length and aperture are highly scene-dependent so you might need to play around with these 
// values based on your specific scene in order to get good results.
const int light_samples = 2; // how frequently to sample area lights for soft shadows (light_samples per each row and column)
const bool depth_of_field = false;
const float focal_length = 21.0f;
const float aperture = 0.05f;
const int dof_samples = 4;
const int recursion = 4; // recursion that controls number of times a ray can bounce(or refract) off a surface
const int row_samples = 3; //  how many samples to take per pixel for antialiasing( row_samples ^ 2)


// this function simply returns an orthonormal basis of size 2 for a plane that has the given normal
// Needed in depth of field and glossy reflections
std::vector<glm::vec3> get_basis_plane(glm::vec3 normal) {
	// a silly way to get a vector normal to (x, y, z) is to simply use (y, -x, 0) which will have 
	// a dot product of zero with the first one and thus is orthogonal.
	// An edge case occurs when both x and y are zero which triggers division by zero. 
	// In that case we simply choose (1, 1, 0).
	// Once we have the first one, the cross product comes to our help for the second one.
	glm::vec3 first;
	if (is_within(normal.y, 0.0f, epsilon) && is_within(normal.x, 0.0f, epsilon)) {
		first = glm::normalize(glm::vec3(1.0f, 1.0f, 0.0f));
	}
	else {
		first = glm::normalize(glm::vec3(normal.y, -normal.x, 0.0f));
	}
	auto second = glm::normalize(glm::cross(first, normal));
	return {first, second};
}
// this function returns the coords of the pixel px, py in the image with respect to the viewing plane in the scene
glm::vec3 pixel_coords(double fovy, uint px, uint py,  float offsetx, float offsety, glm::vec3 eye, glm::vec3 view, uint width, uint height) {
	float aspect = width / height;
	float top_left_y = (eye+view).y + tan(glm::radians(fovy / 2)) * glm::abs(view.z);
	float top_left_x = (eye+view).x - tan(glm::radians(fovy / 2)) * glm::abs(view.z) * aspect;
	float pixel_width = -2 * top_left_x / width;
	float pixel_height = 2 * top_left_y / height;
	return glm::vec3(top_left_x + (px + offsetx) * pixel_width, top_left_y - (py + offsety) * pixel_height, (eye + view).z);
}

// this function gets the colour of a pixel in the image plane by intersecting it with the geo of the scene.
// Recursion controls how deep the reflected rays shhould be followed and normalized_y is simply a pixel y-coordinate
// that will only be used to determine background colour if the ray misses the scene.
glm::vec3 shade_point(Ray ray, SceneNode* root, const std::list<Light *> & lights, const glm::vec3 ambient, float normalized_y, int recursion) {
	if (!recursion) return glm::vec3(0.0f);
	// the variable below {info} has everything we need to do basic lighting at this point of intersection, of course if 
	// the ray hit something.
	HitRecord info;
	info = root->intersect(ray);
	if (!info.hit) {
		return glm::vec3(0.0f, 0.0f, glm::abs(0.5f - normalized_y));
	}
	auto point = ray.origin + info.t * ray.direction;
	auto raydir = glm::normalize(-ray.direction);
	auto normal = info.Normal;
	glm::vec3 local_colour;
	if (!info.m->isTexture()) {
	
	auto material = static_cast<PhongMaterial*>(info.m);
	// ambient colour
	local_colour += material->getkd() *  ambient;
	// we now know where the ray hit the scene the earliest and the corresponding primitive(stored in info.primitive)
	// must peform local shading at this point of intersection(Blinn-Phong Illumination Model)
	for (auto light: lights) {
		// distinguish between point light and area light
		auto lightDir = light->position - point;
		auto distance = glm::length(lightDir);
		lightDir = glm::normalize(lightDir);
		float hit_rate = 1.0f;
		if (light->is_area) {
			hit_rate = light->get_hit_rate(point, root, light_samples);	
		}
		else {
		// first let us see if the point should be in shadow so that we do not do local illumination on it
		Ray shadowray(point + epsilon * 10 * lightDir, lightDir);
		auto shadow_info = root->intersect(shadowray);
		if (shadow_info.hit) {
			//the point is in shadow, set hit_rate to zero. 
			hit_rate = 0.0f;
		}
		}
		// diffuse lighting
		auto diff_colour = material->getkd();
		auto diffuse = glm::max(glm::dot(lightDir, normal), 0.0f);
		local_colour += hit_rate * diffuse * diff_colour * light->colour / (light->falloff[0] + light->falloff[1] * distance + light->falloff[2] * distance * distance);
		// now specular
		auto halfway = glm::normalize(raydir + lightDir);
		auto specular = glm::max(glm::dot(halfway, normal), 0.0f);
		local_colour += hit_rate * glm::pow(specular, material->getShininess()) * material->getks() * light->colour / (light->falloff[0] + light->falloff[1] * distance + light->falloff[2] * distance * distance);
		if (material->isDielectric()) {
			local_colour -= hit_rate * diffuse * diff_colour * light->colour / (light->falloff[0] + light->falloff[1] * distance + light->falloff[2] * distance * distance);
		}
		// done
	}
	// if the material is dielectric, modify the local colour by adding values based on the light gathered
	// by the refracted light ray. Note that we consider this to be part of the local colour. So local colour
	// is colour produced by Blinn-Phong and refraction. It will later be averaged according to weight 
	// with the reflected colour.
	if (material->isDielectric()) {
		material->refract(ray, point + epsilon * 10 * glm::normalize(ray.direction), normal);
		local_colour += shade_point(ray, root, lights, ambient, normalized_y, recursion - 1);
	}
	glm::vec3 reflected_colour;
	auto glossy = material->getglossiness();
	auto reflected_direction = glm::normalize(glm::reflect(ray.direction, normal));
	std::vector<glm::vec3> basis_vectors = get_basis_plane(reflected_direction);
	if (glossy) {
	// shoot glossy * glossy reflection rays perturbed from the original ray to simulate glossy mirror reflections
	// The colours gathered from each ray are then averaged
	for (int i = 0; i < glossy; ++i) {
		for (int j = 0; j < glossy; ++j) {
			Ray reflected_ray(point + epsilon * 10 * reflected_direction, (reflected_direction + 2 * (i - float(glossy) / 2) / float(glossy) * basis_vectors[0] / 40.0f + (j - float(glossy) / 2)  / float(glossy) * basis_vectors[1] / 40.0f));
			reflected_colour += shade_point(reflected_ray, root, lights, ambient, normalized_y, recursion - 1);
		}
	}
	reflected_colour /= float(glossy * glossy);
	}
	else {
		// this is the normal non-glossy reflection case.
		// A simple efficiency hack is to check that reflectivity values are at least some epsilon 
		// and only do ray calculation in those cases.
		if (material->getreflectivity().x > epsilon || material->getreflectivity().y > epsilon || material->getreflectivity().z > epsilon) {
		Ray reflected_ray(point + epsilon * 10 * reflected_direction, reflected_direction);
		reflected_colour += shade_point(reflected_ray, root, lights, ambient, normalized_y, recursion - 1);
		}
	}
	// 
	return material->getreflectivity()*reflected_colour + (glm::vec3(1.0f) - material->getreflectivity()) * local_colour;
	}
	else { // ********************************************************
		// material is texture in this case. Only local illumination will be done.
		auto material = static_cast<TextureMaterial*>(info.m);
		// get the uv texture coordinates of the primitive based on its local coordinates (this is primitive specific)
		// It is achieved using polymorphism
		auto uv = info.primitive->uv_coords(info.localcoords);
		// this gets the colour corresponding to the point (uv.x, uv.y) using bilinear filtering with 4 nearest texels
		auto diff_colour = material->getkd(uv.x, uv.y);
		local_colour += diff_colour *  ambient;
		for (auto light: lights) {
			// distinguish between point light and area light
			auto lightDir = light->position - point;
			auto distance = glm::length(lightDir);
			lightDir = glm::normalize(lightDir);
			float hit_rate = 1.0f;
			if (light->is_area) {
				hit_rate = light->get_hit_rate(point, root, light_samples);	
			}
			else {
			// first let us see if the point should be in shadow so that we do not do local illumination on it
			Ray shadowray(point + epsilon * 10 * lightDir, lightDir);
			auto shadow_info = root->intersect(shadowray);
			if (shadow_info.hit) {
				//the point is in shadow, set hit_rate to zero
				hit_rate = 0.0f;
			}
			}
			// diffuse lighting
			auto diffuse = glm::max(glm::dot(lightDir, normal), 0.0f);
			local_colour += hit_rate * diffuse * diff_colour * light->colour / (light->falloff[0] + light->falloff[1] * distance + light->falloff[2] * distance * distance);
			// done
	}
	return local_colour;
	}
}


void A5_Render(
		// What to render  
		SceneNode * root,

		// Image to write to, set to a given width and height  
		Image & image,

		// Viewing parameters  
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters  
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
) {

  srand(time(nullptr));

  std::cout << "F20: Calling A5_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;

	int h = image.height();
	int w = image.width();
	const int col_samples = row_samples;
	const int modulo = 100;
	std::vector<glm::vec3> lens_basis = get_basis_plane(view);
	glm::vec3 dof_view;
	glm::vec3 lens_samples[dof_samples][dof_samples];
	// if depth of field enabled, then we are dealing with a square lens and hence we sample it dof_samples*dof_samples times.
	if (depth_of_field) {
		dof_view = focal_length * glm::normalize(view);
		for (int i = 0; i < dof_samples; ++i) {
			for (int j = 0; j < dof_samples; ++j) {
				lens_samples[i][j] = eye + aperture * (2 * (i - float(dof_samples) / 2) / float(dof_samples) * lens_basis[0] + 2 * (j - float(dof_samples) / 2) / float(dof_samples) * lens_basis[1]);
			}
		}
	}
	for (int y = 0; y < h; ++y) {
		for (int x = 0; x < w; ++x) {
			glm::vec3 colour;
			// Jittered anti-aliasing
			float offsetx = (rand() % modulo) / (row_samples * modulo);
			float offsety = (rand() % modulo) / (col_samples * modulo);
			// i, j are the indices of the antialiasing samples per pixel
			for (int i = 0; i < row_samples; ++i) {
				for (int j = 0; j < col_samples; ++j) {
					if (depth_of_field) {
						// shoot rays from each lens sample to create the circle of confusion on the image plane for each pixel
						for (int k = 0; k < dof_samples; ++k) {
							for (int l = 0; l < dof_samples; ++l) {
								Ray ray = Ray(lens_samples[k][l], pixel_coords(fovy, x, y, float(i) / row_samples + offsetx, float(j) / col_samples + offsety, eye, dof_view, w, h) - lens_samples[k][l]); 
								colour += shade_point(ray, root, lights, ambient, float(y) / h, recursion);
						}
					}
					// normalize the colour
					colour /= dof_samples * dof_samples;
					}
					else {
						Ray ray = Ray(eye, pixel_coords(fovy, x, y, float(i) / row_samples + offsetx, float(j) / col_samples + offsety, eye, view, w, h) - eye); 
						colour += shade_point(ray, root, lights, ambient, float(y) / h, recursion) / (row_samples * col_samples);			
					}
			}
			}
			// Red: 
			image(x, y, 0) = colour.x;
			// Green: 
			image(x, y, 1) = colour.y;
			// Blue: 
			image(x, y, 2) = colour.z;
		}
	}

}
