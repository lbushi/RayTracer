// Termm-Fall 2020

#pragma once

#include "Material.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include <glm/glm.hpp>
#include <list>
#include <string>
#include <iostream>

enum class NodeType {
	SceneNode,
	GeometryNode,
	JointNode
};

class SceneNode {
public:
    SceneNode(const std::string & name);

	SceneNode(const SceneNode & other);

    virtual ~SceneNode();
    
	int totalSceneNodes() const;
    
    const glm::mat4& get_transform() const;
    const glm::mat4& get_inverse() const;
    void joint_transform(std::string joint_name, double joint_xoffset, double joint_yoffset);
    
    void set_transform(const glm::mat4& m);
    
    void add_child(SceneNode* child);
    
    void remove_child(SceneNode* child);

	//-- Transformations:
    void rotate(char axis, float angle);
    void scale(const glm::vec3& amount);
    void translate(const glm::vec3& amount);
    void add_parent(SceneNode*);
	friend std::ostream & operator << (std::ostream & os, const SceneNode & node);

	bool isSelected;
    
    // Transformations
    glm::mat4 trans;
    glm::mat4 invtrans;
    
    std::list<SceneNode*> children;

	NodeType m_nodeType;
	std::string m_name;
    SceneNode* parent = nullptr;
	unsigned int m_nodeId;
    SceneNode* get_node(std::string name);
    SceneNode* get_node(unsigned int id);


private:
	// The number of SceneNode instances.
	static unsigned int nodeInstanceCount;
};
