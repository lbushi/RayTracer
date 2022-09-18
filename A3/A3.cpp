// Termm-Fall 2020

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"
#include "trackball.h"
#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{
	for (auto child: undo_stack) {
		delete child;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.85, 0.85, 0.85, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();


	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, -0.5f, 10.0f), vec3(0.0f, -0.5f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(-5.0f, -0.5f, 1.0f);
	m_light.rgbIntensity = vec3(0.6f); // light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;


		//-- Set LightSource uniform for the scene:
		{
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}

		//-- Set background light ambient intensity
		{
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.25f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);
			if (ImGui::BeginMainMenuBar()) {
					if (ImGui::BeginMenu("Application")) {
						if (ImGui::MenuItem("Reset Position(I)", nullptr, nullptr)) keyInputEvent(GLFW_KEY_I, GLFW_PRESS, 0);
						if (ImGui::MenuItem("Reset Orientation(O)", nullptr, nullptr)) keyInputEvent(GLFW_KEY_O, GLFW_PRESS, 0);
						if (ImGui::MenuItem("Reset Joints(S)", nullptr, nullptr)) keyInputEvent(GLFW_KEY_S, GLFW_PRESS, 0);
						if (ImGui::MenuItem("Reset All(A)", nullptr, nullptr)) keyInputEvent(GLFW_KEY_A, GLFW_PRESS, 0);
						if (ImGui::MenuItem("Quit(Q)", nullptr, nullptr)) keyInputEvent(GLFW_KEY_Q, GLFW_PRESS, 0);
						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu("Edit")) {
						if (ImGui::MenuItem("Undo(U)", nullptr, nullptr)) keyInputEvent(GLFW_KEY_U, GLFW_PRESS, 0);
						if (ImGui::MenuItem("Redo(R)", nullptr, nullptr)) keyInputEvent(GLFW_KEY_R, GLFW_PRESS, 0);
						ImGui::EndMenu();
					}
					if (ImGui::BeginMenu("Options")) {
						if (ImGui::MenuItem("Circle(C)", nullptr, nullptr)) keyInputEvent(GLFW_KEY_C, GLFW_PRESS, 0);
						if (ImGui::MenuItem("Z-buffer(Z)", nullptr, nullptr)) keyInputEvent(GLFW_KEY_Z, GLFW_PRESS, 0);
						if (ImGui::MenuItem("Backface culling(B)", nullptr, nullptr)) keyInputEvent(GLFW_KEY_B, GLFW_PRESS, 0);
						if (ImGui::MenuItem("Frontface Culling(F)", nullptr, nullptr)) keyInputEvent(GLFW_KEY_F, GLFW_PRESS, 0);
						ImGui::EndMenu();
					}
					}
					ImGui::EndMainMenuBar();
					static int e;
					ImGui::PushID(0);
					if (ImGui::RadioButton("Position(P)", &e, 0)) {
						keyInputEvent(GLFW_KEY_P, GLFW_PRESS, 0);
					}
					ImGui::PopID();
					ImGui::PushID(1);
					if (ImGui::RadioButton("Joints(J)", &e, 1)) {
						keyInputEvent(GLFW_KEY_J, GLFW_PRESS, 0);
					}
					ImGui::PopID();
					ImGui::Text("Last Undo/Redo Operation: %s", undo_state);
		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & parent_transforms,
		const bool picking,
		const glm::mat4 & viewMatrix
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * parent_transforms * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;
		float pickingr = float(node.m_nodeId&0xff) / 255.0f;
		float pickingg = float((node.m_nodeId>>8)&0xff) / 255.0f;
		float pickingb = float((node.m_nodeId>>16)&0xff) / 255.0f;
		location = shader.getUniformLocation("pickingcolour");
		glUniform3f( location, pickingr, pickingg, pickingb );
		CHECK_GL_ERRORS;
		glUniform1i(shader.getUniformLocation("picking"), int(picking));
		glUniform1i(shader.getUniformLocation("selected"), int(node.isSelected));
		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;


		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.material.kd;
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

	if (z_buffer) glEnable( GL_DEPTH_TEST );
	if (backface_culling) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	if (frontface_culling) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
	}
	renderSceneGraph(*m_rootNode);


	if (z_buffer) glDisable( GL_DEPTH_TEST );
	if (circle) renderArcCircle();
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);


	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.
	glm::mat4 parent_transforms = glm::translate(mat4(1.0f), puppet_origin);
	vector<std::pair<const SceneNode*, glm::mat4>> scene = {make_pair(&root, parent_transforms)};
loop:	for (;scene.size();) {
		auto node = scene.back();
		scene.pop_back();
		for (const auto child: node.first->children) {
			if (node.first->m_nodeType != NodeType::JointNode) {
				scene.emplace_back(make_pair(child, node.second));
			}
			else {
				scene.emplace_back(make_pair(child, node.second * node.first->get_transform()));
				goto loop;
			}
		}
		if (node.first->m_nodeType == NodeType::SceneNode) continue;
		const GeometryNode * geometryNode = static_cast<const GeometryNode *>(node.first);
		updateShaderUniforms(m_shader, *geometryNode, node.second, picking, m_view);
		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];
		//-- Now render the mesh:
		m_shader.enable();
		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		m_shader.disable();
	}
	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
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
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	static double prevxPos, prevyPos;
	double xoffset = (xPos - prevxPos)/4;
	double yoffset = (yPos - prevyPos)/4;
	if (rotate) {
		auto rot_vec = glm::vec3(0.0f);
		float diameter = m_windowHeight < m_windowWidth ? m_windowHeight / 2 : m_windowWidth / 2;
		xPos -= diameter;
		yPos -= diameter;
		prevxPos -= diameter;
		prevyPos -= diameter;
		vCalcRotVec(xPos, yPos, prevxPos, prevyPos, diameter, &rot_vec[0], &rot_vec[1], &rot_vec[2]);
		rot_vec /= 12.0f;
		Matrix mat;
		vAxisRotMatrix(rot_vec.x, -rot_vec.y, rot_vec.z, mat);
		glm::mat4 actualmat = glm::mat4(mat[0][0], mat[0][1], mat[0][2], mat[0][3],
		mat[1][0], mat[1][1], mat[1][2], mat[1][3],
		mat[2][0], mat[2][1], mat[2][2], mat[2][3],
		mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
		auto rotnode = m_rootNode->children.front();//->children.front();
		rotnode->translate(-puppet_origin);
		rotnode->set_transform(actualmat * rotnode->get_transform());
		rotnode->translate(puppet_origin);
		orientation = glm::translate(mat4(1.0f), puppet_origin) * actualmat * glm::translate(mat4(1.0f), -puppet_origin) * orientation;
		eventHandled = true;
	}
	else { 
	if (move) {
		xoffset /= 3;
		yoffset /= 3;
		m_rootNode->children.front()->translate(vec3(xoffset, -yoffset, 0.0f));
		puppet_origin += vec3(xoffset, -yoffset, 0.0f);
		eventHandled = true;
	}
	if (moveZ) {
		m_rootNode->children.front()->translate(vec3(0.0f, 0.0f, yoffset));
		puppet_origin += vec3(0.0f, 0.0f, yoffset);
		eventHandled = true;
	}
	}
	if (limb_rotation) {
		for (auto limb: selectedParts) {
			if (!limb.second.first) continue;
			JointNode* p = static_cast<JointNode*>(limb.first->parent);
			if (p->joint_x - yoffset > p->m_joint_x.max || p->joint_x - yoffset < p->m_joint_x.min) continue;
			p->joint_x -= yoffset;
			p->translate(-p->joint_location);
			p->rotate('x', float(yoffset));
			p->translate(p->joint_location);
			selectedParts[limb.first].second += yoffset;
		}
		eventHandled = true;
	}
	if (head_side_movement) {
		JointNode* headjoint = static_cast<JointNode*>(m_rootNode->get_node("headjoint"));
		if (!(headjoint->joint_y - xoffset > headjoint->m_joint_y.max || headjoint->joint_y - xoffset < headjoint->m_joint_y.min)){
		headjoint->joint_y -= xoffset;
		headjoint->translate(-(headjoint->joint_location + glm::vec3(0.0f, 0.5f, 0.0f)));
		headjoint->rotate('y', float(xoffset));
		headjoint->translate(headjoint->joint_location + glm::vec3(0.0f, 0.5f, 0.0f));
		selectedParts[headjoint->children.front()].second += xoffset;
		}
		eventHandled = true;
	}
	prevxPos = xPos;
	prevyPos = yPos;
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);
	if (mode == AppMode::Position) {
		if (button == GLFW_MOUSE_BUTTON_1 && actions == GLFW_PRESS) {
			move = true;
		}
		if (button == GLFW_MOUSE_BUTTON_1 && actions == GLFW_RELEASE) {
			move = false;
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT && actions == GLFW_PRESS) {
			rotate = true;
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT && actions == GLFW_RELEASE) {
			rotate = false;
		}
		if (button == GLFW_MOUSE_BUTTON_MIDDLE && actions == GLFW_PRESS) {
			moveZ = true;
		}
		if (button == GLFW_MOUSE_BUTTON_MIDDLE && actions == GLFW_RELEASE) {
			moveZ = false;
		}
		eventHandled = true;
	}
	if (mode == AppMode::Picking) {
		if (button == GLFW_MOUSE_BUTTON_1 && actions == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos( m_window, &xpos, &ypos );

		picking = true;

		uploadCommonSceneUniforms();
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		draw();

		// I don't know if these are really necessary anymore.
		// glFlush();
		// glFinish();

		CHECK_GL_ERRORS;

		xpos *= double(m_framebufferWidth) / double(m_windowWidth);
		ypos = m_windowHeight - ypos;
		ypos *= double(m_framebufferHeight) / double(m_windowHeight);

		GLubyte buffer[ 4 ] = { 0, 0, 0, 0 };
		glReadBuffer( GL_BACK );
		// Actually read the pixel at the mouse location.
		glReadPixels( int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
		CHECK_GL_ERRORS;

		// Reassemble the object ID.
		unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);
		auto node = m_rootNode->get_node(what);
		if (node && node->parent) {
			m_rootNode->get_node(what)->isSelected = !m_rootNode->get_node(what)->isSelected;
			if (selectedParts.count(node)) {
				selectedParts[node].first = !selectedParts[node].first;
			} 
			else {
				selectedParts[node].first = 1;
			}
		}

		picking = false;
		eventHandled = true;
		CHECK_GL_ERRORS;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && actions == GLFW_PRESS) {
		limb_rotation = true;
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && actions == GLFW_RELEASE){
		undo_stack.erase(undo_stack.begin() + stack_current + 1, undo_stack.end());
		vector<std::pair<SceneNode*, float>> selected;
		for (auto part: selectedParts) {
			if (part.second.first) {
				selected.emplace_back(make_pair(part.first, part.second.second));
			}
		}
		undo_stack.emplace_back(new CommandX(selected));
		stack_current++;
		for (auto& part: selectedParts) {
			if (part.second.first) part.second.second = 0.0f;
		}
		limb_rotation = false;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && actions == GLFW_PRESS) {
		head_side_movement = true;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && actions == GLFW_RELEASE) {
		undo_stack.erase(undo_stack.begin() + stack_current + 1, undo_stack.end());
		vector<std::pair<SceneNode*, float>> selected;
		for (auto part: selectedParts) {
			if (part.second.first) {
				selected.emplace_back(make_pair(part.first, part.second.second));
			}
		}
		undo_stack.emplace_back(new CommandY(selected));
		stack_current++;
		for (auto& part: selectedParts) {
			if (part.second.first) part.second.second = 0.0f;
		}
		head_side_movement = false;
	}
	}
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

void A3::resetUndoStack() {
	stack_current = -1;
	undo_stack.clear();
}


//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

void A3::resetJoints(SceneNode* root) {
	for (auto child: root->children) {
				if (child->m_nodeType == NodeType::JointNode && child->m_name != "torsojoint") {
					JointNode* joint = static_cast<JointNode*>(child);
					joint->joint_x = joint->m_joint_x.init;
					joint->joint_y = joint->m_joint_y.init;
					joint->set_transform(mat4(1.0f));
				}
				resetJoints(child);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
		if (key == GLFW_KEY_P) {
			mode = AppMode::Position;
			eventHandled = true;
		}
		if (key == GLFW_KEY_J) {
			mode = AppMode::Picking;
			eventHandled = true;
		}
		if (key == GLFW_KEY_C) {
			circle = !circle;
			eventHandled = true;
		}
		if (key == GLFW_KEY_Z) {
			z_buffer = !z_buffer;
			eventHandled = true;
		}
		if (key == GLFW_KEY_B) {
			backface_culling = !backface_culling;
			eventHandled = true;
		}
		if (key == GLFW_KEY_F) {
			frontface_culling = !frontface_culling;
			eventHandled = true;
		}
		if (key == GLFW_KEY_I) {
			m_rootNode->children.front()->set_transform(glm::translate(mat4(1.0f), glm::vec3(0.0f, -2.3f, -5.0f) - puppet_origin)* m_rootNode->children.front()->get_transform());
			puppet_origin = glm::vec3(0.0f, -2.3f, -5.0f);
		}
		if (key == GLFW_KEY_O) {
			m_rootNode->children.front()->set_transform(inverse(orientation) * m_rootNode->children.front()->get_transform());
			orientation = mat4(1.0f);
		}
		if (key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if (key == GLFW_KEY_S) {
			resetJoints(m_rootNode->children.front());
			resetUndoStack();
		}
		if (key == GLFW_KEY_A) {
			keyInputEvent (GLFW_KEY_O, GLFW_PRESS, 0);
			keyInputEvent (GLFW_KEY_I, GLFW_PRESS, 0);
			keyInputEvent (GLFW_KEY_S, GLFW_PRESS, 0);
		}
		if (key == GLFW_KEY_U) {
			if (stack_current > -1) {
				undo_stack[stack_current]->undo();
				stack_current -= 1;
				undo_state = "Successful";
			}
			else {
				undo_state = "Failed";
			}
		}
		if (key == GLFW_KEY_R) {
			if (stack_current + 1 < undo_stack.size()) {
				stack_current++;
				undo_stack[stack_current]->execute();
				undo_state = "Successful";
			}
			else {
				undo_state = "Failed";
			}
		}
		eventHandled = true;
	}
	// Fill in with event handling code...

	return eventHandled;
}
