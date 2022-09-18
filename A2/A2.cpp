// Termm--Fall 2020

#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <algorithm>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

const double SCR_WIDTH = 768;
const double SCR_HEIGHT = 768;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.resize(kMaxVertices);
	colours.resize(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f))
{
	// The initial vertices of the cube in model space
	cube.insert(cube.end(),
		{
		glm::vec3(-1.0f, -1.0f, 1.0f),
		glm::vec3(1.0f, -1.0f, 1.0f),
		glm::vec3(1.0f, -1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(-1.0f, 1.0f, 1.0f),
		glm::vec3(-1.0f, 1.0f, 1.0f),
		glm::vec3(-1.0f, -1.0f, 1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, 1.0f, -1.0f),
		glm::vec3(1.0f, 1.0f, -1.0f),
		glm::vec3(-1.0f, 1.0f, -1.0f),
		glm::vec3(-1.0f, 1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(-1.0f, -1.0f, 1.0f),
		glm::vec3(-1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, 1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, -1.0f),
		glm::vec3(-1.0f, 1.0f, 1.0f),
		glm::vec3(-1.0f, 1.0f, -1.0f)
		}
	);
	cube_gnomon.insert(cube_gnomon.end(), 
	{
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.5f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.5f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.5f)
	});
	model_gnomon.insert(model_gnomon.end(), 
	{
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.5f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.5f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.5f)
	});
	glm::vec3 up = glm::vec3(0.0f, 1.0f ,0.0f);
	glm::vec3 eyedir = glm::normalize(eye);
	glm::vec3 right = glm::normalize(glm::cross(up, eyedir));
	view = transpose(mat4(glm::vec4(right, 0.0f), glm::vec4(up, 0.0f), glm::vec4(eyedir, -eye.z), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));

}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//---------------------------------------------------------------------------------------- Spring 2020
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & V0,   // Line Start (NDC coordinate)
		const glm::vec2 & V1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = V0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = V1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

void A2::viewportdrawLine(vector<glm::vec2> line) {
	if (line.size() == 0) {
		return;
	}
	auto start = line[0];
	auto end = line[1];
	drawLine(vec2(viewport_left_corner.x + ((start.x + 1) / 2) * viewport_xdim, viewport_left_corner.y + ((start.y - 1) / 2 * viewport_ydim)),
	vec2(viewport_left_corner.x + ((end.x + 1) / 2) * viewport_xdim, viewport_left_corner.y + ((end.y - 1) / 2 * viewport_ydim)));
}

vector<glm::vec2> A2::graphics_pipeline(mat4 model, glm::vec4 u, glm::vec4 v){
	u = model * u;
	v = model * v;
	auto near_position = eye.z - near;
	auto far_position = eye.z - far;
	// perform near and far plane clipping in world space
	if ((u.z > near_position && v.z > near_position) || (u.z < far_position && v.z < far_position)) {
		return {};
	}
	if ((u.z - near_position) * (v.z - near_position) < 0) {
		auto start = u.z < near_position ? u : v;
		auto end = start == u ? v : u;
		auto t = (near_position - start.z) / (end.z - start.z);
		if ( end == v) {
			v = start + t * (end - start);
		}
		else {
			u = start + t * (end - start);
		}
	}

	if ((u.z - far_position) * (v.z - far_position) < 0) {
		auto start = u.z > far_position ? u : v;
		auto end = start == u ? v : u;
		auto t = (far_position - start.z) / (end.z - start.z);
		if ( end == v) {
			v = start + t * (end - start);
		}
		else {
			u = start + t * (end - start);
		}
	}

	u = proj * view * u;
	v = proj * view * v;
	auto u_ndc = vec2(u.x / u.w, u.y / u.w);
	auto v_ndc = vec2(v.x / v.w, v.y / v.w);

	// left and right plane clipping
	if ((u_ndc.x < -1.0f && v_ndc.x < -1.0f) || (u_ndc.x > 1.0f && v_ndc.x > 1.0f)) {
		return {};
	}
	if ((u_ndc.x + 1.0f) * (v_ndc.x + 1.0f) < 0) {
		auto start = u_ndc.x > -1.0f ? u_ndc : v_ndc;
		auto end = start == u_ndc ? v_ndc : u_ndc;
		auto t = (-1.0f - start.x) / (end.x - start.x);
		if ( end == v_ndc) {
			v_ndc = start + t * (end - start);
		}
		else {
			u_ndc = start + t * (end - start);
		}
	}

	if ((u_ndc.x - 1.0f) * (v_ndc.x - 1.0f) < 0) {
		auto start = u_ndc.x < 1.0f ? u_ndc : v_ndc;
		auto end = start == u_ndc ? v_ndc : u_ndc;
		auto t = (1.0f - start.x) / (end.x - start.x);
		if ( end == v_ndc) {
			v_ndc = start + t * (end - start);
		}
		else {
			u_ndc = start + t * (end - start);
		}
	}

	// clip the up/down planes
	if ((u_ndc.y < -1.0f && v_ndc.y < -1.0f) || (u_ndc.y > 1.0f && v_ndc.y > 1.0f)) {
		return {};
	}
	if ((u_ndc.y + 1.0f) * (v_ndc.y + 1.0f) < 0) {
		auto start = u_ndc.y > -1.0f ? u_ndc : v_ndc;
		auto end = start == u_ndc ? v_ndc : u_ndc;
		auto t = (-1.0f - start.y) / (end.y - start.y);
		if ( end == v_ndc) {
			v_ndc = start + t * (end - start);
		}
		else {
			u_ndc = start + t * (end - start);
		}
	}
	if ((u_ndc.y - 1.0f) * (v_ndc.y - 1.0f) < 0) {
		auto start = u_ndc.y < 1.0f ? u_ndc : v_ndc;
		auto end = start == u_ndc ? v_ndc : u_ndc;
		auto t = (1.0f - start.y) / (end.y - start.y);
		if ( end == v_ndc) {
			v_ndc = start + t * (end - start);
		}
		else {
			u_ndc = start + t * (end - start);
		}
	}

	vector<glm::vec2> result = { u_ndc, v_ndc };
	return result;
}

vector<glm::vec2> A2::get_viewport_info(glm::vec2 corner1, glm::vec2 corner2) {
	vector<glm::vec2> corners = {corner1, corner2};
	sort(corners.begin(), corners.end(), [](glm::vec2 first, glm::vec2 second) { return first.x <= second.x;});
	if (corners[0].y < corners[1].y) {
		return {(corners[0] - vec2(SCR_WIDTH / 2)) / float(SCR_WIDTH / 2),
				(corners[1] - vec2(SCR_HEIGHT / 2)) / float(SCR_HEIGHT / 2)};
	}
	return {(vec2(corners[0].x, corners[1].y) - vec2(SCR_WIDTH / 2)) / float(SCR_WIDTH / 2),
			(vec2(corners[1].x, corners[0].y) - vec2(SCR_HEIGHT / 2)) / float(SCR_HEIGHT / 2)};
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...
	proj = transpose(mat4(glm::vec4(1 / (glm::tan(fov/2) * aspect), 0.0f, 0.0f, 0.0f),
	glm::vec4(0.0f, 1/glm::tan(fov/2), 0.0f, 0.0f),
	glm::vec4(0.0f, 0.0f, (-far - near) / (far - near), (-2 * far * near) / (far - near)),
	glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
	// Call at the beginning of frame, before drawing lines:
	initLineData();

	setLineColour(vec3(1.0f, 1.0f, 1.0f));
	for(int i = 0; i < 24; i += 2) {
		auto line = graphics_pipeline(model, glm::vec4(cube[i], 1.0f), glm::vec4(cube[i + 1], 1.0f));
		viewportdrawLine(line);
	}
	auto cube_gnomon_right = graphics_pipeline(mat4(1.0f), glm::vec4(cube_gnomon[0], 1.0f), glm::vec4(cube_gnomon[1], 1.0f));
	auto cube_gnomon_up = graphics_pipeline(mat4(1.0f), glm::vec4(cube_gnomon[2], 1.0f), glm::vec4(cube_gnomon[3], 1.0f));
	auto cube_gnomon_z = graphics_pipeline(mat4(1.0f), glm::vec4(cube_gnomon[4], 1.0f), glm::vec4(cube_gnomon[5], 1.0f));
	setLineColour(vec3(1.0f, 0.0f, 1.0f));
	viewportdrawLine(cube_gnomon_right);
	setLineColour(vec3(0.0f, 1.0f, 1.0f));
	viewportdrawLine(cube_gnomon_up);
	setLineColour(vec3(1.0f, 1.0f, 0.0f));
	viewportdrawLine(cube_gnomon_z);

	auto model_gnomon_right = graphics_pipeline(gnomon_model_mat, glm::vec4(model_gnomon[0], 1.0f), glm::vec4(model_gnomon[1], 1.0f));
	auto model_gnomon_up = graphics_pipeline(gnomon_model_mat, glm::vec4(model_gnomon[2], 1.0f), glm::vec4(model_gnomon[3], 1.0f));
	auto model_gnomon_z = graphics_pipeline(gnomon_model_mat, glm::vec4(model_gnomon[4], 1.0f), glm::vec4(model_gnomon[5], 1.0f));
	setLineColour(vec3(1.0f, 0.0f, 0.0f));
	viewportdrawLine(model_gnomon_right);
	setLineColour(vec3(0.0f, 1.0f, 0.0f));
	viewportdrawLine(model_gnomon_up);
	setLineColour(vec3(0.0f, 0.0f, 1.0f));
	viewportdrawLine(model_gnomon_z);
	
	// drawing the viewport
	setLineColour(vec3(0.0f, 0.0f, 0.0f));
	drawLine(viewport_left_corner, viewport_left_corner + vec2(viewport_xdim, 0.0f));
	drawLine(viewport_left_corner, viewport_left_corner + vec2(0.0f, -viewport_ydim));
	drawLine(viewport_left_corner + vec2(viewport_xdim, 0.0f), viewport_left_corner + vec2(viewport_xdim, -viewport_ydim));
	drawLine(viewport_left_corner + vec2(0.0f, -viewport_ydim), viewport_left_corner + vec2(viewport_xdim, -viewport_ydim));
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);
	static int e;
	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);
	ImGui::Text("Rotate View          (o)"); ImGui::SameLine();
	ImGui::PushID(0);
	if (ImGui::RadioButton("", &e, 2)) {
		keyInputEvent(GLFW_KEY_O, 0, 0);
	}
	ImGui::PopID();
	ImGui::PushID(1);
	ImGui::Text("Translate View       (e)"); ImGui::SameLine();
	if (ImGui::RadioButton("", &e, 3)) {
		keyInputEvent(GLFW_KEY_E, 0, 0);
	}
	ImGui::PopID();
	ImGui::PushID(2);
	ImGui::Text("Perspective          (p)"); ImGui::SameLine();
	if (ImGui::RadioButton("", &e, 1)) {
		keyInputEvent(GLFW_KEY_P, 0, 0);
	}
	ImGui::PopID();
	ImGui::PushID(3);
	ImGui::Text("Rotate Model         (r)"); ImGui::SameLine();
	if (ImGui::RadioButton("", &e, 0)) {
		keyInputEvent(GLFW_KEY_R, 0, 0);
	}
	ImGui::PopID();
	ImGui::PushID(6);
	ImGui::Text("Translate Model      (t)"); ImGui::SameLine();
	if (ImGui::RadioButton("", &e, 4)) {
		keyInputEvent(GLFW_KEY_T, 0, 0);
	}
	ImGui::PopID();
	ImGui::PushID(4);
	ImGui::Text("Scale Model          (s)"); ImGui::SameLine();
	if (ImGui::RadioButton("", &e, 5)) {
		keyInputEvent(GLFW_KEY_S, 0, 0);
	}
	ImGui::PopID();
	ImGui::PushID(5);
	ImGui::Text("Viewport             (v)"); ImGui::SameLine();
	if (ImGui::RadioButton("", &e, 6)) {
		keyInputEvent(GLFW_KEY_V, 0, 0);
	}
	ImGui::PopID();

		// Create Button, and check if it was clicked:
		if( ImGui::Button("Quit Application     (q)" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if (ImGui::Button("Reset                (a)")) {
			keyInputEvent(GLFW_KEY_A, 0, 0);
		}
		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
		ImGui::Text("Near: %.2f, Far: %.2f", near, far);

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);
	static double prevxPos;
	if (mode == Interaction_Mode::RotateModel) {
		float rotation_angle = glm::radians((xPos - prevxPos) / 2);
		if (Xmode) {
			model = model * transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, cos(rotation_angle), -sin(rotation_angle), 0.0f),
			glm::vec4(0.0f, sin(rotation_angle), cos(rotation_angle), 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
			gnomon_model_mat = gnomon_model_mat * transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, cos(rotation_angle), -sin(rotation_angle), 0.0f),
			glm::vec4(0.0f, sin(rotation_angle), cos(rotation_angle), 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
		}
		if (Ymode) {
			model = model * transpose(glm::mat4(glm::vec4(cos(rotation_angle), 0.0f, sin(rotation_angle), 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec4(-sin(rotation_angle), 0.0f, cos(rotation_angle), 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
			gnomon_model_mat = gnomon_model_mat * transpose(glm::mat4(glm::vec4(cos(rotation_angle), 0.0f, sin(rotation_angle), 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec4(-sin(rotation_angle), 0.0f, cos(rotation_angle), 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
		}
		if (Zmode) {
			model = model * transpose(glm::mat4(glm::vec4(cos(rotation_angle), -sin(rotation_angle), 0.0f, 0.0f), glm::vec4(sin(rotation_angle), cos(rotation_angle), 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
			gnomon_model_mat = gnomon_model_mat * transpose(glm::mat4(glm::vec4(cos(rotation_angle), -sin(rotation_angle), 0.0f, 0.0f), glm::vec4(sin(rotation_angle), cos(rotation_angle), 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
		}
	}
	else if (mode == Interaction_Mode::RotateView) {
		float rotation_angle = glm::radians((xPos - prevxPos) / 2);
		if (Zmode) {
			view = inverse(transpose(glm::mat4(glm::vec4(cos(rotation_angle), -sin(rotation_angle), 0.0f, 0.0f), glm::vec4(sin(rotation_angle), cos(rotation_angle), 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)))) * view;
			gnomon_cube_mat = inverse(transpose(glm::mat4(glm::vec4(cos(rotation_angle), -sin(rotation_angle), 0.0f, 0.0f), glm::vec4(sin(rotation_angle), cos(rotation_angle), 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)))) * gnomon_cube_mat;
			
		}
		if (Ymode) {
			view = inverse(transpose(glm::mat4(glm::vec4(cos(rotation_angle), 0.0f, sin(rotation_angle), 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec4(-sin(rotation_angle), 0.0f, cos(rotation_angle), 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)))) * view;
			gnomon_cube_mat = inverse(transpose(glm::mat4(glm::vec4(cos(rotation_angle), 0.0f, sin(rotation_angle), 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec4(-sin(rotation_angle), 0.0f, cos(rotation_angle), 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)))) * gnomon_cube_mat;
		}
		if (Xmode) {
			view = inverse(transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, cos(rotation_angle), -sin(rotation_angle), 0.0f),
			glm::vec4(0.0f, sin(rotation_angle), cos(rotation_angle), 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)))) * view;
			gnomon_cube_mat = inverse(transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, cos(rotation_angle), -sin(rotation_angle), 0.0f),
			glm::vec4(0.0f, sin(rotation_angle), cos(rotation_angle), 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)))) * gnomon_cube_mat;
		}
	}
	else if (mode == Interaction_Mode::TranslateView) {
		float displacement = (xPos - prevxPos) / 4;
		if (Zmode) {
			view = inverse(transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, displacement), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)))) * view;
			gnomon_cube_mat = inverse(transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, displacement), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)))) * gnomon_cube_mat;
			
		}
		if (Ymode) {
			view = inverse(transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, displacement),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)))) * view;
			gnomon_cube_mat = inverse(transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, displacement),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)))) * gnomon_cube_mat;
		}
		if (Xmode) {
			view = inverse(transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, displacement), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)))) * view;
			gnomon_cube_mat = inverse(transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, displacement), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)))) * gnomon_cube_mat;
		}
	}
	else if (mode == Interaction_Mode::TranslateModel) {
		float displacement = (xPos - prevxPos) / 4;
		if (Xmode) {
			model = model * transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, displacement), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
			gnomon_model_mat = gnomon_model_mat * transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, displacement), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
		}
		if (Ymode) {
			model = model * transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, displacement),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
			gnomon_model_mat = gnomon_model_mat * (transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, displacement),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))));
		}
		if (Zmode) {
			model = model * transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, displacement), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
			gnomon_model_mat = gnomon_model_mat * transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, displacement), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))); 
			
		}
	}
	else if (mode == Interaction_Mode::ScaleModel) {
		static float scaling_so_far = 1.0f;
		float scale = (xPos - prevxPos) / 10;
		scaling_so_far = scaling_so_far * (1.0f + scale);
		if (scaling_so_far < 0.05f) {
			return true;
		}
		if (Xmode) {
			model = model * transpose(glm::mat4(glm::vec4(1 + scale, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
		}
		if (Ymode) {
			model = model * transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1 + scale, 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1.0f, 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
		}
		if (Zmode) {
			model = model * transpose(glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
			glm::vec4(0.0f, 0.0f, 1 + scale, 0.0f), 
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))); 
		}
	}
	else if (mode == Interaction_Mode::Perspective) {
		float diff = (xPos - prevxPos) / 10;
		if (fovmode) {
			fov += diff;
			fov = glm::max(glm::min(fov, float(glm::radians(160.0f))), float(glm::radians(5.0f)));
		}
		if (nearmode) {
			near += diff;
			near = glm::min(glm::max(near, 0.1f), far - 0.1f);
		}
		if (farmode) {
			far += diff;
			far = glm::max(far, near + 0.1f);
		}
	}
	else if (mode == Interaction_Mode::Viewport) {
		if (xPos > SCR_WIDTH) {
			xPos = SCR_WIDTH;
		}
		if (xPos < 0) {
			xPos = 0;
		}
		if (yPos > SCR_HEIGHT) {
			yPos = SCR_HEIGHT;
		}
		if (yPos < 0) {
			yPos = 0;
		}
		static glm::vec2 first_viewport_corner;
		static glm::vec2 second_viewport_corner;
		if (start_dragging) {
			first_viewport_corner = vec2(xPos, yPos);
			start_dragging = false;
		}
		else if (!end_dragging) {
			second_viewport_corner = vec2(xPos, yPos);
			auto viewport_info = get_viewport_info(first_viewport_corner, second_viewport_corner);
			viewport_left_corner = vec2(viewport_info[0].x, -viewport_info[0].y);
			auto viewport_right_corner = vec2(viewport_info[1].x, -viewport_info[1].y);
			viewport_xdim = viewport_right_corner.x - viewport_left_corner.x;
			viewport_ydim = viewport_left_corner.y - viewport_right_corner.y; 
		}
	}
	prevxPos = xPos;
	eventHandled = true;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);
	static bool first_viewport_click = true;
	if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_PRESS) {
		if (mode != Interaction_Mode::Perspective) {
			Xmode = true;
		}
		else {
			fovmode = true;
		}
		if (mode == Interaction_Mode::Viewport) {
			if (first_viewport_click) {
				start_dragging = true;
				first_viewport_click = false;
			}
			end_dragging = false;
		}
		eventHandled = true;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && actions == GLFW_RELEASE) {
		if (mode != Interaction_Mode::Perspective){
			Xmode = false;
		}
		else {
			fovmode = false;
		}
		if (mode == Interaction_Mode::Viewport) {
			end_dragging = true;
			first_viewport_click = true;
		}
		eventHandled = true;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && actions == GLFW_PRESS) {
		if (mode != Interaction_Mode::Perspective) {
			Ymode = true;
		}
		else {
			nearmode = true;
		}
		eventHandled = true;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && actions == GLFW_RELEASE) {
		if (mode != Interaction_Mode::Perspective){
			Ymode = false;
		}
		else {
			nearmode = false;
		}
		eventHandled = true;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && actions == GLFW_PRESS) {
		if (mode != Interaction_Mode::Perspective) {
			Zmode = true;
		}
		else {
			farmode = true;
		}
		eventHandled = true;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && actions == GLFW_RELEASE) {
		if (mode != Interaction_Mode::Perspective){
			Zmode = false;
		}
		else {
			farmode = false;
		}
		eventHandled = true;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if (key == GLFW_KEY_O) {
		mode = Interaction_Mode::RotateView;
	}
	if (key == GLFW_KEY_E) {
		mode = Interaction_Mode::TranslateView;
	}
	if (key == GLFW_KEY_R) {
		mode = Interaction_Mode::RotateModel;
	}
	if (key == GLFW_KEY_T){
		mode = Interaction_Mode::TranslateModel;
	}
	if (key == GLFW_KEY_S) {
		mode = Interaction_Mode::ScaleModel;
	}
	if (key == GLFW_KEY_V) {
		mode = Interaction_Mode::Viewport;
	}
	if (key == GLFW_KEY_P) {
		mode = Interaction_Mode::Perspective;
	}
	if (key == GLFW_KEY_S) {
		mode = Interaction_Mode::ScaleModel;
	}
	if (key == GLFW_KEY_Q) {
		glfwSetWindowShouldClose(m_window, GL_TRUE);
	}
	if (key == GLFW_KEY_A) {
		model = glm::mat4(1.0f);
		gnomon_model_mat = glm::mat4(1.0f);
		gnomon_cube_mat = glm::mat4(1.0f);
		near = 1.0f;
		far  = 10.0f;
		fov = glm::radians(30.0f);
		Xmode = false;
		Ymode = false;
		Zmode = false;
		fovmode = false;
		nearmode = false;
		farmode = false;
		start_dragging = false;
		end_dragging = true;
		viewport_left_corner = glm::vec2(-0.9f, 0.9f);
		viewport_xdim = 1.8f;
		viewport_ydim = 1.8f;
		mode = Interaction_Mode::RotateModel;
		glm::vec3 up = glm::vec3(0.0f, 1.0f ,0.0f);
		glm::vec3 eyedir = glm::normalize(eye);
		glm::vec3 right = glm::normalize(glm::cross(up, eyedir));
		view = transpose(mat4(glm::vec4(right, 0.0f), glm::vec4(up, 0.0f), glm::vec4(eyedir, -eye.z), glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
	}
	eventHandled = true;
	return eventHandled;
}
