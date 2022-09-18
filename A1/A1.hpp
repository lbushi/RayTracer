// Termm--Fall 2020

#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "maze.hpp"
#include "sphere.hpp"

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

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

private:
	void initGrid();

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.
	GLint avatar_P_uni; // Uniform location for Projection matrix.
	GLint avatar_V_uni; // Uniform location for View matrix.
	GLint avatar_M_uni; // Uniform location for Model matrix.
	GLint avatar_col_uni;   // Uniform location for cube colour.
	ShaderProgram avatar_shader;

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;
	// The maze that dictates the placement of the cubes on the grid
	Maze m;
	// Fields related to the geometry of the block that will generate the maze 
	GLuint m_block_vao;
	GLuint m_block_vbo;
	// Fields related to the geometry of the floor that will support the maze
	GLuint m_floor_vao;
	GLuint m_floor_vbo;
	// The position of the avatar which will be changed based on keyboard keys pressed
	glm::vec3 avatar_pos;
	// Fields related to the geometry of the avatar
	GLuint m_avatar_vao;
	GLuint m_avatar_vbo;
	// This field will be true for as long as SHIFT is pressed so that we can that match it with an
	// arrow to remove a wall
	bool wall_removal;
	enum class Rotation_State { Off, Manual, Automatic };
	Rotation_State rotation_state;
	// The rotation matrix
	glm::mat4 rotation;
	// The scaling matrix when the scroll event is recorded
	glm::mat4 scaling;
	// This will be the time when the left mouse button is released to determine whether to keep rotating
	double release_time;
	// This is the speed of the automatic rotation based on the mouse movement
	float automatic_speed;
	// The avatar smoothness level, the higher this value tho more the avatar will resemble a perfect sphere
	const int smoothness_level;
	int wall_height;
	float blocks_colour[3];
	float floor_colour[3];
	float avatar_colour[3];
	int current_col;
};
