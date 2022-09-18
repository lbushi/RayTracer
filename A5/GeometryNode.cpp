// Termm--Fall 2020

#include "GeometryNode.hpp"
#include "PhongMaterial.hpp"
#include "Mesh.hpp"
//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
{
	m_nodeType = NodeType::GeometryNode;
}

// This is the star player. It gets the closest intersection of the ray with the scene
// and it returns a HitRecord structure containg all the required info to do lighting on the intersection point.
// It delegates the work to the primitive itself which has a specific way of checking for an intersection(sphere, plane etc...)
// and then does some post processing on the info returned as documented below.
HitRecord GeometryNode::intersect(Ray ray) {
	// here we transform the ray into the local coords of the primitive underlying this geometry node.
	ray.transform_ray(glm::inverse(get_transform()));
	HitRecord record = m_primitive->intersect(ray);
	if (record.hit) {
		record.m = m_material;
	}
	if (normals) {
		// this node is special. it is using a texture for normals and hence it overrides the normal returned by the primitive itself.
		// It does this by indexing the normal texture as below
		auto p = record.primitive;
		record.Normal = normals->getkd(p->uv_coords(record.localcoords).x, p->uv_coords(record.localcoords).y);
		// normals must be rotated because the primitive wants them oriented roughly towards y-axis whereas
		//  the ones in most normal maps are actually towards z-axis
		record.Normal = glm::vec3(glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::vec4(record.Normal, 1.0f));
	}

	// set the primitive that the ray hit. Could be done in the individual primitives but its centralized here instead.
	record.primitive = this->m_primitive;
	return record;
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}
