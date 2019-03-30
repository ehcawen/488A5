// Winter 2019

#include "A1.hpp"
#include "maze.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <sys/types.h>
#include <unistd.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



using namespace glm;
using namespace std;

static const size_t DIM = 16;

int ShiftHeld=0;
int MouseHeld=0;


//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col(0), 
	m(nullptr), 
	h(1.0f), 
	avatarX(0), 
	avatarY(0),
	floor_color(glm::vec3(1.0f, 1.0f, 1.0f)),
	maze_color(glm::vec3(0.7f, 0.7f, 0.7f)),
	avatar_color(glm::vec3(0.3f, 0.3f, 0.3f)),
	rotation(0.0f),
	scale(1.0f),
	movement(0.0f),
	persistence(0)
{
	colour[0] = 1.0f;
	colour[1] = 1.0f;
	colour[2] = 1.0f;
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{
	delete m;
}


//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Initialize random number generator
	int rseed = getpid();
	srandom(rseed);
	// Print random number seed in case we want to rerun with
	// same random numbers
	cout << "Random number seed = " << rseed << endl;

	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath("VertexShader.vs").c_str());
	m_shader.attachFragmentShader(
		getAssetFilePath("FragmentShader.fs").c_str());
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation("P");
	V_uni = m_shader.getUniformLocation("V");
	M_uni = m_shader.getUniformLocation("M");
	col_uni = m_shader.getUniformLocation("colour");

	initGrid();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);


	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt(
		glm::vec3(0.0f, 2. * float(DIM) * 2.0 * M_SQRT1_2, float(DIM) * 2.0 * M_SQRT1_2),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	proj = glm::perspective(
		glm::radians(30.0f),
		float(m_framebufferWidth) / float(m_framebufferHeight),
		1.0f, 1000.0f);
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM + 3);

	float *verts = new float[sz];
	size_t ct = 0;
	for (int idx = 0; idx < DIM + 3; ++idx)
	{
		verts[ct] = -1;
		verts[ct + 1] = 0;
		verts[ct + 2] = idx - 1;
		verts[ct + 3] = DIM + 1;
		verts[ct + 4] = 0;
		verts[ct + 5] = idx - 1;
		ct += 6;

		verts[ct] = idx - 1;
		verts[ct + 1] = 0;
		verts[ct + 2] = -1;
		verts[ct + 3] = idx - 1;
		verts[ct + 4] = 0;
		verts[ct + 5] = DIM + 1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays(1, &m_grid_vao);
	glBindVertexArray(m_grid_vao);

	// Create the cube vertex buffer
	glGenBuffers(1, &m_grid_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_grid_vbo);
	glBufferData(GL_ARRAY_BUFFER, sz * sizeof(float),
				 verts, GL_STATIC_DRAW);

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation("position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Reset state to prevent rogue code from messing with *my*
	// stuff!
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete[] verts;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
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

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100, 100), opacity, windowFlags);
	if (ImGui::Button("Quit Application"))
	{
		glfwSetWindowShouldClose(m_window, GL_TRUE);
	}

	if(ImGui::Button("Dig")){
		if(m != nullptr)delete m;
			m = new Maze(DIM);
			m->digMaze();
			avatarX = 0;
			avatarY = m->getStart();
	}

	if(ImGui::Button("Reset")){
		Reset();
	}

	// Eventually you'll create multiple colour widgets with
	// radio buttons.  If you use PushID/PopID to give them all
	// unique IDs, then ImGui will be able to keep them separate.
	// This is unnecessary with a single colour selector and
	// radio button, but I'm leaving it in as an example.

	// Prefixing a widget name with "##" keeps it from being
	// displayed.


	ImGui::ColorEdit3("##Colour", colour);

	if(current_col==0){
		floor_color.r = colour[0];
		floor_color.g = colour[1];
		floor_color.b = colour[2];
	}
	if(current_col==1){
		maze_color.r = colour[0];
		maze_color.g = colour[1];
		maze_color.b = colour[2];
	}
	if(current_col==2){
		avatar_color.r = colour[0];
		avatar_color.g = colour[1];
		avatar_color.b = colour[2];
	}

	if (ImGui::RadioButton("##Col1", &current_col, 0))
	{
		colour[0] = floor_color.r;
		colour[1] = floor_color.g;
		colour[2] = floor_color.b;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("##Col2", &current_col, 1))
	{
		colour[0] = maze_color.r;
		colour[1] = maze_color.g;
		colour[2] = maze_color.b;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("##Col3", &current_col, 2))
	{
		colour[0] = avatar_color.r;
		colour[1] = avatar_color.g;
		colour[2] = avatar_color.b;
	}

	/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in
		// shared/imgui/imgui_demo.cpp to see how it's done.*/
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}


	ImGui::Text("Framerate: %.1f FPS", ImGui::GetIO().Framerate);

	ImGui::End();

	if (showTestWindow)
	{
		ImGui::ShowTestWindow(&showTestWindow);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// check persistence
	if(persistence > 0) {
		rotation += movement * 0.005* persistence/60;
		persistence --;
	}
	// Create a global transformation for the model (centre it).
	mat4 W;
	W = glm::rotate(W, rotation, glm::vec3(0, 1, 0));
	W = glm::scale(W, glm::vec3(scale,scale,scale));
	W = glm::translate(W, vec3(-float(DIM) / 2.0f, 0, -float(DIM) / 2.0f));

	m_shader.enable();
	glEnable(GL_DEPTH_TEST);

	glUniformMatrix4fv(P_uni, 1, GL_FALSE, value_ptr(proj));
	glUniformMatrix4fv(V_uni, 1, GL_FALSE, value_ptr(view));
	glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(W));

	// Draw the floor
	DrawFloor();

	// Just draw the grid for now.
	glUniform3f(col_uni, 1.0f, 1.0f, 1.0f);
	glBindVertexArray(m_grid_vao);
	glDrawArrays(GL_LINES, 0, (3 + DIM) * 4);



	// Draw the cubes
	// for testing
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if(m!=nullptr)DrawCube(W);
	// End of draw maze cubes

	// Draw the avatar
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );

	// draw avatar in sphere shape
	DrawAvatar(W);

	// Highlight the active square.
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent(
	int entered)
{
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

	if (!ImGui::IsMouseHoveringAnyWindow())
	{
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so
		// that you can rotate relative to the *change* in X.
		if(MouseHeld == 1){
			movement = xPos - current_xPos;
			rotation += movement * 0.005;
			persistence = 60;
		}
		current_xPos = xPos;
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods)
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow())
	{
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
		if(button == GLFW_MOUSE_BUTTON_LEFT){
			if(actions == GLFW_PRESS) MouseHeld = 1;
			if(actions == GLFW_RELEASE)MouseHeld = 0;
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet)
{
	bool eventHandled(false);

	// Zoom in or out.

	if(scale >0.01f && scale < 10.0f){
		if(yOffSet>0)scale = scale * (0.005 * yOffSet + 1);
		if(yOffSet<0)scale = scale / (0.005 * (-yOffSet) +1);
	}



	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height)
{
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

void A1::Reset(){
	if(m != nullptr){
				delete m;
				m= nullptr;
			}
			persistence = 0;
			rotation = 0.0f;
			scale = 1.0f;
			current_col=0;
			colour[0] = 1.0f;
			colour[1] = 1.0f;
			colour[2] = 1.0f;
			avatarX = 0;
			avatarY = 0;
			floor_color = glm::vec3(1.0f, 1.0f, 1.0f);
			maze_color = glm::vec3(0.7f, 0.7f, 0.7f);
			avatar_color = glm::vec3(0.3f, 0.3f, 0.3f);
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods)
{
	bool eventHandled(false);

	// Fill in with event handling code...
	if (action == GLFW_PRESS)
	{
		// Respond to some key events.

		// Press Q to quit the application
		if (key == GLFW_KEY_Q)
		{
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
		if (key == GLFW_KEY_SPACE)
		{
			h += 1.0f;
		}

		if (key == GLFW_KEY_BACKSPACE)
		{
			if (h > 1.0f)
				h -= 1.0f;
		}

		if(key == GLFW_KEY_UP){
			MoveDown();
		}

		if(key == GLFW_KEY_DOWN){
			MoveUp();
		}

		if(key == GLFW_KEY_LEFT){
			MoveLeft();
		}

		if(key == GLFW_KEY_RIGHT){
			MoveRight();
		}

		if(key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT){
			ShiftHeld = 1;
		}

		if(key == GLFW_KEY_D){
			if(m != nullptr)delete m;
			m = new Maze(DIM);
			m->digMaze();
			avatarX = 0;
			avatarY = m->getStart();
		}
		if(key == GLFW_KEY_R){
			Reset();
		}
	}

	if(action == GLFW_RELEASE){
		if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT){
			ShiftHeld = 0;
		}
	}

	return eventHandled;
}

void A1::DrawFloor(){
	glUniform3f(col_uni, floor_color.r, floor_color.g, floor_color.b);
	float floor_vertices[] = {
		0.0f, -0.1f, 0.0f,
		16.0f, -0.1f, 0.0f,
		0.0f, -0.1f, 16.0f,

		0.0f, -0.1f, 16.0f,
		16.0f, -0.1f, 0.0f,
		16.0f, -0.1f, 16.0f
	};


	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void A1::DrawCube(glm::mat4 W)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glUniform3f(col_uni, maze_color.r, maze_color.g, maze_color.b);
	float vertices[] = {
			0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, h, 0.0f,
			0.0f, h, 0.0f,
			0.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f,
			1.0f, h, 1.0f,
			0.0f, h, 1.0f
	};

	unsigned int indices[] = {
			/* back */
			0, 1, 2,
			2, 3, 0,

			/* front */
			4, 5, 6,
			6, 7, 4,

			/* left */
			7, 3, 0,
			0, 4, 7,

			/* right */
			6, 2, 1,
			1, 5, 6,

			/* bottom */
			0, 1, 5,
			5, 4, 0,

			/* top */
			3, 2, 6,
			6, 7, 3
	};

	/*
	 * float vertices[] = {
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, h, 0.0f,
		1.0f, h, 0.0f,
		0.0f, h, 0.0f,
		0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, h, 1.0f,
		1.0f, h, 1.0f,
		0.0f, h, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, h, 1.0f,
		0.0f, h, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, h, 1.0f,

		1.0f, h, 1.0f,
		1.0f, h, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, h, 1.0f,

		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,

		0.0f, h, 0.0f,
		1.0f, h, 0.0f,
		1.0f, h, 1.0f,
		1.0f, h, 1.0f,
		0.0f, h, 1.0f,
		0.0f, h, 0.0f};
	 */

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 16; y++)
		{
			if (m->getValue(x, y))
			{


				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
				// position attribute
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
				glEnableVertexAttribArray(0);
				// maybe texture coord here
				W = glm::translate(W, vec3(float(x), 0, float(y)));
				glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(W));
				glBindVertexArray(VAO);
				glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
				W = glm::translate(W, vec3(-float(x), 0, -float(y)));
			}
		}
	}
}

void A1::DrawAvatar(glm::mat4 W){
    // todo: load avatar.lua here
	// glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glUniform3f(col_uni, avatar_color.r, avatar_color.g, avatar_color.b);
	W = glm::translate(W, vec3(float(avatarX)+ 0.5f, 0.5f, float(avatarY)+0.5f));
	
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	int m = 72;
	int n = 72;

	float delta_z = M_PI/m; // angle z
	float delta_xy = M_PI/n; // angle xy plane
	float r = 0.5f;

	float angle_z = 0.0f;
	float angle_xy = 0.0f;

	for(int i=0; i<m; i++){
		angle_z = i * delta_z;
		for(int j=0; j<n; j++){
			angle_xy = j * delta_xy;
			float x0, x1, x2, x3, y0, y1, y2, y3, z0, z1, z2, z3;
			x0 = r * sin(angle_z) * cos(angle_xy);
			y0 = r * sin(angle_z) * sin(angle_xy);
			z0 = r * cos(angle_z);

			x1 = r * sin(angle_z + delta_z) * cos(angle_xy);
			y1 = r * sin(angle_z + delta_z) * sin(angle_xy);
			z1 = r * cos(angle_z + delta_z);

			x0 = r * sin(angle_z + delta_z) * cos(angle_xy + delta_xy);
			y0 = r * sin(angle_z + delta_z) * sin(angle_xy + delta_xy);
			z0 = r * cos(angle_z + delta_z);

			x0 = r * sin(angle_z) * cos(angle_xy + delta_xy);
			y0 = r * sin(angle_z) * sin(angle_xy + delta_xy);
			z0 = r * cos(angle_z);

			float quad[]={
				x0, y0, z0,
				x1, y1, z1,
				x2, y2, z2,
				x2, y2, z2,
				x3, y3, z3,
				x0, y0, z0
			};
			glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
			glEnableVertexAttribArray(0);
			// maybe texture coord here
			glUniformMatrix4fv(M_uni, 1, GL_FALSE, value_ptr(W));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}

	
	W = glm::translate(W, vec3(-float(avatarX)-0.5f, -0.5f, -float(avatarY)-0.5f));
}

void A1::MoveUp(){
	if(avatarY< DIM - 1){
		if(m==nullptr){
			avatarY++;
			return;
		}
		if(ShiftHeld == 1){
			m->setValue(avatarX, avatarY+1, 0);
		}

		if(m->getValue(avatarX,avatarY+1)==0) avatarY++;
	}
	
}
void A1::MoveDown(){
	
	if(avatarY>0){
		if(m==nullptr){
			avatarY--;
			return;
		}
		if(ShiftHeld == 1){
			m->setValue(avatarX, avatarY-1, 0);
		}

		if(m->getValue(avatarX,avatarY-1)==0)avatarY--;
	}
}
void A1::MoveLeft(){
	if(avatarX>0){
		if(m==nullptr){
			avatarX--;
			return;
		}
		if(ShiftHeld == 1){
			m->setValue(avatarX-1,avatarY, 0);
		}
		if(m->getValue(avatarX-1,avatarY)==0)avatarX--;
	}
}
void A1::MoveRight(){
	if(avatarX < DIM - 1){
		if(m==nullptr){
			avatarX++;
			return;
		}
		if(ShiftHeld == 1){
			m->setValue(avatarX+1, avatarY, 0);
		}
		if(m->getValue(avatarX+1,avatarY)==0)avatarX++;
	}
}


