#include "Command.hpp"
Command::Command(std::vector<std::pair<SceneNode*, float>>& nodes): nodes{nodes}{}
CommandX::CommandX(std::vector<std::pair<SceneNode*, float>>& nodes): Command{nodes}{}
CommandY::CommandY(std::vector<std::pair<SceneNode*, float>>& nodes): Command{nodes}{}

void CommandX::execute() {
		for (auto node: nodes) {
			auto thenode = node.first;
			auto rotamount = node.second;
			JointNode* p = static_cast<JointNode*>(thenode->parent);
			auto yoffset = rotamount;
			if (p->joint_x - yoffset > p->m_joint_x.max || p->joint_x - yoffset < p->m_joint_x.min) continue;
			p->joint_x -= yoffset;
			p->translate(-p->joint_location);
			p->rotate('x', float(yoffset));
			p->translate(p->joint_location);
		}
}

void CommandX::undo() {
		for (auto node: nodes) {
			auto thenode = node.first;
			auto rotamount = node.second;
			JointNode* p = static_cast<JointNode*>(thenode->parent);
			auto yoffset = -rotamount;
			if (p->joint_x - yoffset > p->m_joint_x.max || p->joint_x - yoffset < p->m_joint_x.min) continue;
			p->joint_x -= yoffset;
			p->translate(-p->joint_location);
			p->rotate('x', float(yoffset));
			p->translate(p->joint_location);
		}
}

void CommandY::undo() {
		for (auto node: nodes) {
			auto thenode = node.first;
			auto rotamount = node.second;
			JointNode* p = static_cast<JointNode*>(thenode->parent);
			auto xoffset = -rotamount;
			if (p->joint_y - xoffset > p->m_joint_y.max || p->joint_y - xoffset < p->m_joint_y.min) continue;
			p->joint_y -= xoffset;
			p->translate(-p->joint_location);
			p->rotate('y', float(xoffset));
			p->translate(p->joint_location);
		}
}

void CommandY::execute() {
		for (auto node: nodes) {
			auto thenode = node.first;
			auto rotamount = node.second;
			JointNode* p = static_cast<JointNode*>(thenode->parent);
			auto xoffset = rotamount;
			if (p->joint_y - xoffset > p->m_joint_y.max || p->joint_y - xoffset < p->m_joint_y.min) continue;
			p->joint_y -= xoffset;
			p->translate(-p->joint_location);
			p->rotate('y', float(xoffset));
			p->translate(p->joint_location);
		}
}