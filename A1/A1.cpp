// Termm--Fall 2020

#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <sys/types.h>
#include <unistd.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;


static const size_t DIM = 16;
static const vec3 avatar_default_pos = vec3(0.0f);
static const float scaling_rate = 1.1f;

// how many seconds to wait after left mouse button release 
// before determining if rotation must continue independently
static const float rotation_window = 0.1f;

static const float max_scaling = 2.0f;
static const float min_scaling = 0.5f;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 ), m{ Maze(DIM) }, avatar_pos{avatar_default_pos}, wall_removal{ false }, 
	rotation_state{ Rotation_State::Off }, rotation{ glm::mat4(1.0f)}, wall_height{ 1 }, 
	smoothness_level{ 6 }
	
{
	for (int i = 0; i < 3; ++i) {
		blocks_colour[i] = 0.0f;
		floor_colour[i] = 1.0f;
		avatar_colour[i] = i == 0 ? 1.0f : 0.0f;
	}
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Dig the maze
	//m.digMaze();
	// Initialize random number generator
	int rseed=getpid();
	srandom(rseed);
	// Print random number seed in case we want to rerun with
	// same random numbers
	cout << "Random number seed = " << rseed << endl;
	
	
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );
	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();
	avatar_shader.generateProgramObject();
	avatar_shader.attachVertexShader(
		getAssetFilePath( "AvatarVertexShader.vs" ).c_str() );
	avatar_shader.attachFragmentShader(
		getAssetFilePath( "AvatarFragmentShader.fs" ).c_str() );
	avatar_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	avatar_P_uni = avatar_shader.getUniformLocation( "P" );
	avatar_V_uni = avatar_shader.getUniformLocation( "V" );
	avatar_M_uni = avatar_shader.getUniformLocation( "M" );

	col_uni = m_shader.getUniformLocation( "colour" );
	avatar_col_uni = avatar_shader.getUniformLocation("colour");

	initGrid();
	// below is vertex data for the cube that will generate the maze by appropriate translation
	const size_t sz = 3 * 6 * 6;
	float cubeverts[sz] = {
			// first face
			1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			// second face
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 1.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			// third face
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 1.0f,
			// face opposite face 1
			1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 1.0f,
			// face opposite face 2
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			// face opposite face 3
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
		};
	// The initialization of the buffers for the block that will generate the maze
	glGenVertexArrays( 1, &m_block_vao );
	glBindVertexArray( m_block_vao );

	glGenBuffers( 1, &m_block_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_block_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		cubeverts, GL_STATIC_DRAW );

	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
	glBindVertexArray(0);

	// the floor vertex data
	const size_t floorsz = 3 * 6;
	float floorverts[floorsz] = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, DIM,
		DIM, 0.0f, 0.0f,
		DIM, 0.0f, 0.0f,
		0.0f, 0.0f, DIM,
		DIM, 0.0f, DIM
	};
	// The initialization of the buffers for the floor represented as a square
	glGenVertexArrays( 1, &m_floor_vao );
	glBindVertexArray( m_floor_vao );
	glGenBuffers( 1, &m_floor_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_floor_vbo );
	glBufferData( GL_ARRAY_BUFFER, floorsz*sizeof(float),
		floorverts, GL_STATIC_DRAW );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
	glBindVertexArray(0);
	
	const float sphere_center[3] = {0.5f, 0.5f, 0.5f};
	const float radius = 0.5f;
	std::vector<Face> sphere = create_sphere(sphere_center, radius, smoothness_level);
	auto size = sphere.size() * 3 * 3 * 2; // each face has 3 vertices which have each 3 coordinates plus the normals
	float * avatar_vertices = new float[size];
	int i = 0;
	for (auto face: sphere) {
		face.update_normals(sphere_center, radius);
		avatar_vertices[i] = face.vertex1[0];
		avatar_vertices[i + 1] = face.vertex1[1];
		avatar_vertices[i + 2] = face.vertex1[2];
		avatar_vertices[i + 3] = face.normal1.x;
		avatar_vertices[i + 4] =  face.normal1.y;
		avatar_vertices[i + 5] =  face.normal1.z;
		avatar_vertices[i + 6] =  face.vertex2[0];
		avatar_vertices[i + 7] =  face.vertex2[1];
		avatar_vertices[i + 8] =  face.vertex2[2];
		avatar_vertices[i + 9] =  face.normal2.x;
		avatar_vertices[i + 10] = face.normal2.y;
		avatar_vertices[i + 2 + 9] = face.normal2.z;
		avatar_vertices[i + 3 + 9] = face.vertex3[0];
		avatar_vertices[i + 4 + 9] = face.vertex3[1];
		avatar_vertices[i + 5 + 9] = face.vertex3[2];
		avatar_vertices[i + 6 + 9] = face.normal3.x;
		avatar_vertices[i + 7 + 9] = face.normal3.y;
		avatar_vertices[i + 8 + 9] = face.normal3.z;
		i += 18;
	}
	// The initialization of the buffers for the avatar
	glGenVertexArrays( 1, &m_avatar_vao );
	glBindVertexArray( m_avatar_vao );
	glGenBuffers( 1, &m_avatar_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_avatar_vbo );
	glBufferData( GL_ARRAY_BUFFER, size*sizeof(float),
		avatar_vertices, GL_STATIC_DRAW );
	// can now delete the avatar vertices since the buffer is stored by OpenGL
	free(avatar_vertices);
	glEnableVertexAttribArray( 0 );
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr );
	glEnableVertexAttribArray(1);
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)) );
	glBindVertexArray(0);
	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt( 
		glm::vec3( 0.0f, 2.*float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	proj = glm::perspective( 
		glm::radians( 30.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		// draw the idx-th horizontal line
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;
		
		// draw the idx-th vertical line
		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
	if (rotation_state == Rotation_State::Automatic) {
		rotation = glm::rotate(rotation, automatic_speed, vec3(0.0f, 1.0f, 0.0f));
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if (ImGui::Button( "Reset Maze")) {
			keyInputEvent(GLFW_KEY_R, GLFW_PRESS, 0);
		}
		if (ImGui::Button("Dig Maze")) {
			keyInputEvent(GLFW_KEY_D, GLFW_PRESS, 0);
		}
		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

		ImGui::PushID( 0 );
		ImGui::ColorEdit3( "Colour", blocks_colour );
		ImGui::SameLine();
		if( ImGui::RadioButton( "Maze blocks colour", &current_col, 0 ) ) {
		}

		ImGui::PopID();
		ImGui::PushID( 1 );
		ImGui::ColorEdit3( "Colour", floor_colour );
		ImGui::SameLine();
		if( ImGui::RadioButton( "Floor colour", &current_col, 1 ) ) {
		}

		ImGui::PopID();
		ImGui::PushID( 2 );
		ImGui::ColorEdit3( "Colour", avatar_colour );
		ImGui::SameLine();
		if( ImGui::RadioButton( "Avatar colour", &current_col, 2 ) ) {
		}
		ImGui::PopID();

/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );
	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( scaling * rotation * W ) );
		//glUniform3f(lightpos_uni, 0.0f, 2.*float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2);

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );
		// Draw the cubes here
		glBindVertexArray(m_block_vao);
		glUniform3f( col_uni, blocks_colour[0], blocks_colour[1], blocks_colour[2]);
		for (int k = 0; k < wall_height; ++k) {
			for (int i = 0; i < DIM; ++i) {
				for (int j = 0; j < DIM; ++j) {
					if (m.getValue(i, j) == 1) {
						mat4 mazeW = glm::translate(W, vec3(i, k, j));
						glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr(scaling * rotation * mazeW));
						glDrawArrays(GL_TRIANGLES, 0, 36);
					}
				}
			}
		}
		// Draw the floor
		glBindVertexArray( m_floor_vao );
		glUniform3f( col_uni, floor_colour[0], floor_colour[1], floor_colour[2]);
		glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(scaling * rotation * W));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		m_shader.disable();
		// Draw the avatar
		avatar_shader.enable();
		glUniform3f(avatar_shader.getUniformLocation("lightPos"), 0.0f, 2.*float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 - 40);
		glUniform3f(avatar_col_uni, avatar_colour[0], avatar_colour[1], avatar_colour[2]);
		glBindVertexArray( m_avatar_vao );
		glUniformMatrix4fv( avatar_M_uni, 1, GL_FALSE, value_ptr(scaling * rotation * glm::translate(W, avatar_pos)));
		glUniformMatrix4fv( avatar_P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( avatar_V_uni, 1, GL_FALSE, value_ptr( view ) );
		// Number of vertices drawn below depends on the smoothness level, it is 24 * 4 ^ smoothness_level
		glDrawArrays(GL_TRIANGLES, 0, 24 * (1 << ( smoothness_level << 1)));
		avatar_shader.disable();
	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
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
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);
	static double prevxPos, prevyPos;
	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so 
		// that you can rotate relative to the *change* in X.
		if (glfwGetTime()  - release_time <= rotation_window) {
			rotation_state = Rotation_State::Automatic;
			automatic_speed = radians((xPos - prevxPos) / 2);
		}
		if (rotation_state == Rotation_State::Manual) {
			rotation = glm::rotate(rotation, radians(float(xPos- prevxPos)) / 2, vec3(0.0f, 1.0f, 0.0f));
		}
		eventHandled = true;
		prevxPos = xPos;
	}
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (actions == GLFW_PRESS) {
				rotation_state = Rotation_State::Manual;
			}
			else if (actions == GLFW_RELEASE) {
				rotation_state = Rotation_State::Off;
				release_time = glfwGetTime();
			}
		}
		eventHandled = true;

	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);
	static float scaled_so_far = 1.0f;
	scaled_so_far *= (1 - yOffSet);
	scaled_so_far = glm::max((glm::min(scaled_so_far, max_scaling)), min_scaling);
	scaling = glm::scale(mat4(1.0f), vec3(scaled_so_far));
	eventHandled = true;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);


	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		if (key == GLFW_KEY_Q) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if (key == GLFW_KEY_R) {
			m = Maze(DIM);
			avatar_pos = avatar_default_pos;
			rotation = mat4(1.0f);
			scaling = mat4(1.0f);
			wall_height = 1;
			release_time = 0.0f;
			for (int i = 0; i < 3; ++i) {
				blocks_colour[i] = 0.0f;
				floor_colour[i] = 1.0f;
				avatar_colour[i] = i == 0 ? 1.0f : 0.0f;
			}
			rotation_state = Rotation_State::Off;
			eventHandled = true;
		}
		if (key == GLFW_KEY_D) {
			m.digMaze();
			avatar_pos = m.getStart();
			eventHandled = true;
		}
		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
			wall_removal = true;
			eventHandled = true;

		}
		if (key == GLFW_KEY_UP) {
			if (avatar_pos.z < 0) {
				return true;
			}
			if (wall_removal) {
				m.setValue(avatar_pos.x, avatar_pos.z - 1, 0);
			}
			avatar_pos.z -= (avatar_pos.z == 0 || !m.getValue(avatar_pos.x, avatar_pos.z - 1)) ? 1 : 0;
			eventHandled = true;
		}
		if (key == GLFW_KEY_DOWN) {
			if (avatar_pos.z > DIM - 1) {
				return true;
			}
			if (wall_removal) {
				m.setValue(avatar_pos.x, avatar_pos.z + 1, 0);
			}
			avatar_pos.z += (avatar_pos.z == DIM - 1 || !m.getValue(avatar_pos.x, avatar_pos.z + 1)) ? 1 : 0;
			eventHandled = true;
		}
		if (key == GLFW_KEY_LEFT) {
			if (avatar_pos.x < 0) {
				return true;
			}
			if (wall_removal) {
				m.setValue(avatar_pos.x - 1, avatar_pos.z, 0);
			}
			avatar_pos.x -= (avatar_pos.x == 0 || !m.getValue(avatar_pos.x - 1, avatar_pos.z)) ? 1 : 0;
			eventHandled = true;
		}
		if (key == GLFW_KEY_RIGHT) {
			if (avatar_pos.x > DIM - 1) {
				return true;
			}
			if (wall_removal) {
				m.setValue(avatar_pos.x + 1, avatar_pos.z, 0);
			}
			avatar_pos.x += (avatar_pos.x == DIM - 1 || !m.getValue(avatar_pos.x + 1, avatar_pos.z)) ? 1 : 0;
			eventHandled = true;
		}
		if (key == GLFW_KEY_SPACE) {
			wall_height += 1;
		}
		if (key == GLFW_KEY_BACKSPACE) {
			wall_height = glm::max(wall_height - 1, 0);
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
			wall_removal = false;
			eventHandled = true;
		}
	}

	return eventHandled;
}
