//
// Created by Zhihan Wen on 2019-03-28.
//

#include "A5.hpp"
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

using namespace std;
using namespace glm;

A5::World::World(glm::vec3 F_g, float ufs, float ufk)
        : F_g(F_g),
          ufs(ufs),
          ufk(ufk) {}

A5::A5() :
        world(glm::vec3(0, -12, 0), 0.1, 0.05)
{

}

A5::~A5() {

}

void A5::init() {

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

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A5::appLogic()
{
    // Place per frame, application logic here ...
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A5::guiLogic()
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

    // Eventually you'll create multiple colour widgets with
    // radio buttons.  If you use PushID/PopID to give them all
    // unique IDs, then ImGui will be able to keep them separate.
    // This is unnecessary with a single colour selector and
    // radio button, but I'm leaving it in as an example.

    // Prefixing a widget name with "##" keeps it from being
    // displayed.


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

void A5::draw() {

}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A5::cleanup()
{}


//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A5::cursorEnterWindowEvent (
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
bool A5::mouseMoveEvent(double xPos, double yPos)
{
    bool eventHandled(false);

    if (!ImGui::IsMouseHoveringAnyWindow()) {
        // Put some code here to handle rotations.  Probably need to
        // check whether we're *dragging*, not just moving the mouse.
        // Probably need some instance variables to track the current
        // rotation amount, and maybe the previous X position (so
        // that you can rotate relative to the *change* in X.
    }

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A5::mouseButtonInputEvent(int button, int actions, int mods) {
    bool eventHandled(false);

    if (!ImGui::IsMouseHoveringAnyWindow()) {
        // The user clicked in the window.  If it's the left
        // mouse button, initiate a rotation.
    }

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A5::mouseScrollEvent(double xOffSet, double yOffSet) {
    bool eventHandled(false);

    // Zoom in or out.

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A5::windowResizeEvent(int width, int height) {
    bool eventHandled(false);

    // Fill in with event handling code...

    return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A5::keyInputEvent(int key, int action, int mods) {
    bool eventHandled(false);

    // Fill in with event handling code...
    if( action == GLFW_PRESS ) {
        // Respond to some key events.
    }

    return eventHandled;
}

void A5::uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator){

}

void A5::initViewMatrix(){

}

void A5::initLightSources(){

}

void A5::initPerspectiveMatrix(){

}

void A5::initShaderProgram(ShaderProgram& program, const std::string& name) {
    program.generateProgramObject();
    program.attachVertexShader( getAssetFilePath((name + ".vsh").c_str()).c_str() );
    program.attachFragmentShader( getAssetFilePath((name + ".fsh").c_str()).c_str() );
    program.link();
}

bool A5::isKeyPressed(int key){
    return false;
}

GLuint A5::readTextureCubemap(std::string){
    return -1;
}

void A5::renderSkybox(const glm::mat4& Projection, const glm::mat4& View){

}
