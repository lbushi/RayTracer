// Termm--Fall 2020

#include <iostream>
#include <fstream>
#include <glm/ext.hpp>
#include <algorithm>
// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
	// now we calculate the bounding spheres for the mesh
	#ifdef RENDER_BOUNDING_VOLUMES
	glm::vec3 center;
	unsigned int size = m_vertices.size();
	for (auto vertex: m_vertices) {
		center += vertex / size;
	}
	double radius = 0.0f;
	for (auto vertex: m_vertices) {
		if (glm::distance(vertex, center) > radius) {
			radius = glm::distance(vertex, center);
		}
	}
	boundingsphere = new NonhierSphere(center, radius);
	#endif
}

// intersection routine for a mesh. First find the intersection point with the plane of each trinagle in the mesh
// and then checks to see if the barycentric coordinates of the point w.r.t to the trinagle 
// add to one to determine whether its inside or not.
HitRecord Mesh::intersect(Ray ray) {
	HitRecord record;
	if (boundingsphere) {
		record = boundingsphere->intersect(ray);
		return record;
	}
	for (auto face: m_faces) {
		HitRecord facerecord;
		// get the vertices of the face
		auto v1 = m_vertices[face.v1];
		auto v2 = m_vertices[face.v2];
		auto v3 = m_vertices[face.v3];
		// find plane normal using cross prdouct
		glm::vec3 plane_normal = glm::cross(v2 - v1, v3 - v1);
		float t = glm::dot((v1 - ray.origin), plane_normal) / glm::dot(ray.direction, plane_normal);
		auto point = ray.origin + t * ray.direction;
		// must now check if this point is inside our triangle, barycentric coordinates
		auto area = glm::length(plane_normal);
		auto area1 = glm::length(glm::cross(point - v2, point - v3));
		auto area2 = glm::length(glm::cross(point - v1, point - v3));
		auto area3 = glm::length(glm::cross(point - v1, point - v2));
		if (is_within(area1 + area2 + area3, area, epsilon)) {
			facerecord.hit = true;
			facerecord.t = t;
			facerecord.Normal = glm::normalize(plane_normal);
		}
		if (facerecord.hit && (record.hit == false || facerecord.t < record.t)) {
			record = facerecord;
		}
	}
	record.localcoords = ray.origin + record.t * ray.direction;
	return record;
}

// simple texture function that maps point on the primitives to uv-coords in [0, 1] x [0, 1] texture space.
// it is quite simple since it only works if the mesh is a plane. For other meshes, you will probably
// get funny results.
glm::vec2 Mesh::uv_coords(glm::vec3 point) {
	return (glm::vec2(((point + glm::vec3(1.0f, 0.0f, 1.0f)) / 2).x, ((point + glm::vec3(1.0f, 0.0f, 1.0f)) / 2).z));
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}

Mesh::~Mesh() {
	delete boundingsphere;
}