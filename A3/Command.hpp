#include <vector>
#include "JointNode.hpp"
class Command {
	public:
	virtual ~Command() {};
	Command(std::vector<std::pair<SceneNode*, float>>& nodes);
	virtual void execute() = 0;
	virtual void undo() = 0;
	protected:
	std::vector<std::pair<SceneNode*, float>> nodes;
};

class CommandX: public Command {
	public:
	CommandX(std::vector<std::pair<SceneNode*, float>>& nodes);
	virtual void execute() override;
	virtual void undo() override;
};

class CommandY: public Command{
	public:
	CommandY(std::vector<std::pair<SceneNode*, float>>& nodes);
	virtual void execute() override;
	virtual void undo() override;
};