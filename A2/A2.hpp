// Termm--Fall 2020

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;


// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};


class A2 : public CS488Window {
public:
	A2();
	virtual ~A2();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initLineData();

	void setLineColour(const glm::vec3 & colour);

	void drawLine (
			const glm::vec2 & v0,
			const glm::vec2 & v1
	);

	ShaderProgram m_shader;

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object

	VertexData m_vertexData;

	glm::vec3 m_currentLineColour;

	std::vector<glm::vec3> cube; // the vertices making up the cube
	std::vector<glm::vec3> cube_gnomon;
	std::vector<glm::vec3> model_gnomon;
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 gnomon_model_mat = glm::mat4(1.0f);
	glm::mat4 gnomon_cube_mat;
	glm::mat4 view;
	glm::mat4 proj;
	const float aspect = 1.0f;
	float near = 1.0f;
	float far  = 10.0f;
	float fov = glm::radians(30.0f);
	glm::vec3 eye = glm::vec3(0.0f, 0.0f, 5.0f);
	enum class Interaction_Mode { RotateView, TranslateView, Perspective, RotateModel, TranslateModel, ScaleModel, Viewport};
	Interaction_Mode mode = Interaction_Mode::RotateModel;
	bool Xmode = false;
	bool Ymode = false;
	bool Zmode = false;
	bool fovmode = false;
	bool nearmode = false;
	bool farmode = false;
	bool start_dragging = false;
	bool end_dragging = true;
	glm::vec2 viewport_left_corner = glm::vec2(-0.9f, 0.9f);
	float viewport_xdim = 1.8f;
	float viewport_ydim = 1.8f;
	void viewportdrawLine(std::vector<glm::vec2> line);
	std::vector<glm::vec2> graphics_pipeline(glm::mat4 model, glm::vec4 u, glm::vec4 v);
	std::vector<glm::vec2> get_viewport_info(glm::vec2 corner1, glm::vec2 corner2);
};
