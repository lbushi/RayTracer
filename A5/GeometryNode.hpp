// Termm--Fall 2020

#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#include "TextureMaterial.hpp"
#include "HitRecord.hpp"
#include "Ray.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

	void setMaterial( Material *material );
	virtual HitRecord intersect(Ray ray);
	TextureMaterial * normals = nullptr; //  the normal map 
	Material *m_material;
	Primitive *m_primitive;
};
