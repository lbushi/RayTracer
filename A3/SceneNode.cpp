// Termm-Fall 2020

#include "SceneNode.hpp"

#include "cs488-framework/MathUtils.hpp"

#include "JointNode.hpp"
#include <iostream>
#include <sstream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;


// Static class variable
unsigned int SceneNode::nodeInstanceCount = 0;


//---------------------------------------------------------------------------------------
SceneNode::SceneNode(const std::string& name)
  : m_name(name),
	m_nodeType(NodeType::SceneNode),
	trans(mat4()),
	isSelected(false),
	m_nodeId(nodeInstanceCount++)
{

}

//---------------------------------------------------------------------------------------
// Deep copy
SceneNode::SceneNode(const SceneNode & other)
	: m_nodeType(other.m_nodeType),
	  m_name(other.m_name),
	  trans(other.trans),
	  invtrans(other.invtrans)
{
	for(SceneNode * child : other.children) {
		this->children.push_front(new SceneNode(*child));
	}
}

//---------------------------------------------------------------------------------------
SceneNode::~SceneNode() {
	for(SceneNode * child : children) {
		delete child;
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::set_transform(const glm::mat4& m) {
	trans = m;
	invtrans = m;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_transform() const {
	return trans;
}

//---------------------------------------------------------------------------------------
const glm::mat4& SceneNode::get_inverse() const {
	return invtrans;
}

//---------------------------------------------------------------------------------------
void SceneNode::add_child(SceneNode* child) {
	children.push_back(child);
}

//---------------------------------------------------------------------------------------
void SceneNode::remove_child(SceneNode* child) {
	children.remove(child);
}

SceneNode* SceneNode::get_node(std::string name) {
	if (m_name == name) {
		return this;
	}
	else {
		for (auto child: children) {
			auto node = child->get_node(name);
			if (node) {
				return node;
			}
		}
	}
	return nullptr;
}


SceneNode* SceneNode::get_node(unsigned int id) {
	if (m_nodeId == id) {
		return this;
	}
	else {
		for (auto child: children) {
			auto node = child->get_node(id);
			if (node) {
				return node;
			}
		}
	}
	return nullptr;
}
//---------------------------------------------------------------------------------------
void SceneNode::rotate(char axis, float angle) {
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
	        break;
		case 'z':
			rot_axis = vec3(0,0,1);
	        break;
		default:
			break;
	}
	mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
	trans = rot_matrix * trans;
}

//---------------------------------------------------------------------------------------
void SceneNode::scale(const glm::vec3 & amount) {
	trans = glm::scale(amount) * trans;
}

void SceneNode::add_parent(SceneNode* p) {
	parent = p;
}

void SceneNode::joint_transform(std::string name, double joint_xoffset, double joint_yoffset) {
	if (m_name == name) {
		JointNode* jnode = dynamic_cast<JointNode*>(this);
		if (joint_xoffset >= jnode->m_joint_x.min && joint_xoffset <= jnode->m_joint_x.max) {
			jnode->rotate('x', joint_xoffset);
		}
		if (joint_yoffset >= jnode->m_joint_y.min && joint_xoffset <= jnode->m_joint_y.max) {
			jnode->rotate('y', joint_yoffset);
		}
	}
	else {
		for (const auto child: children) {
			child->joint_transform(name, joint_xoffset, joint_yoffset);
		}
	}
}

//---------------------------------------------------------------------------------------
void SceneNode::translate(const glm::vec3& amount) {
	trans = glm::translate(amount) * trans;
}


//---------------------------------------------------------------------------------------
int SceneNode::totalSceneNodes() const {
	return nodeInstanceCount;
}


//---------------------------------------------------------------------------------------
std::ostream & operator << (std::ostream & os, const SceneNode & node) {

	//os << "SceneNode:[NodeType: ___, name: ____, id: ____, isSelected: ____, transform: ____"
	switch (node.m_nodeType) {
		case NodeType::SceneNode:
			os << "SceneNode";
			break;
		case NodeType::GeometryNode:
			os << "GeometryNode";
			break;
		case NodeType::JointNode:
			os << "JointNode";
			break;
	}
	os << ":[";

	os << "name:" << node.m_name << ", ";
	os << "id:" << node.m_nodeId;
	os << "]";

	return os;
}
