// Termm-Fall 2020

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"
#include <map>
#include <glm/glm.hpp>
#include <memory>
#include "Command.hpp"
struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};



class A3 : public CS488Window {
public:
	A3(const std::string & luaSceneFile);
	virtual ~A3();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();
	int get_false_colour(int x, int y) const;

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderSceneGraph(const SceneNode &node);
	void renderArcCircle();
	void resetJoints(SceneNode*);
	void resetUndoStack();
	glm::mat4 m_perpsective;
	glm::mat4 m_view;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;

	//-- GL resources for trackball circle geometry:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;
	std::string m_luaSceneFile;
	enum class AppMode {Position, Picking};
	AppMode mode = AppMode::Position;
	bool move = false;
	std::shared_ptr<SceneNode> m_rootNode;
	std::map<SceneNode*, std::pair<bool, float>> selectedParts;
	bool picking = false;
	bool limb_rotation = false;
	bool z_buffer = true;
	bool circle = false;
	bool backface_culling = false;
	bool frontface_culling = false;
	bool rotate = false;
	glm::vec3 puppet_origin = glm::vec3(0.0f, -2.3f, -5.0f);
	glm::mat4 orientation = glm::mat4(1.0f);
	glm::mat4 translation = glm::mat4(1.0f);
	bool moveZ = false;
	bool head_side_movement = false;
	int stack_current = -1;
	const char* undo_state = "Successful";
	std::vector<Command*> undo_stack;
};
