//
// Created by Zhihan Wen on 2019-03-28.
//

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include <glm/glm.hpp>
#include "maze.hpp"
#include "Animation.hpp"
#include "SceneNode.hpp"
#include "Player.hpp"

struct LightSource {
    glm::vec3 position;
    glm::vec3 rgbIntensity;
};


class A5 : public CS488Window  {
public:
    A5();
    virtual ~A5();

protected:
    virtual void init();
    virtual void appLogic();
    virtual void guiLogic();
    virtual void draw();
    virtual void cleanup();

    //-- Virtual callback methods
    virtual bool cursorEnterWindowEvent(int entered);
    virtual bool mouseMoveEvent(double xPos, double yPos);
    virtual bool mouseButtonInputEvent(int button, int actions, int mods);
    virtual bool mouseScrollEvent(double xOffSet, double yOffSet);
    virtual bool windowResizeEvent(int width, int height);
    virtual bool keyInputEvent(int key, int action, int mods);

    //-- One time initialization methods:
    //SceneNode *readLuaSceneFile(const std::string& filename);
    void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
    void initViewMatrix();
    void initLightSources();
    void initPerspectiveMatrix();
    void initShaderProgram(ShaderProgram& program, const std::string& name);

    glm::mat4 m_perpsective;
    glm::mat4 m_view;

    LightSource m_light;

    //-- GL resources for mesh geometry data:
    GLuint m_vao_meshData;
    GLuint m_vbo_vertexPositions;
    GLuint m_vbo_vertexNormals;
    GLuint m_vbo_uvCoords;
    ShaderProgram m_shader;

    // BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
    // object. Each BatchInfo object contains an index offset and the number of indices
    // required to render the mesh with identifier MeshId.
    BatchInfoMap m_batchInfoMap;

    std::string m_luaSceneFile;

    SceneNode * blockSceneNode;
    SceneNode * puppetSceneNode;

private:
    struct Mouse {
        double x;
        double y;
        double prevX;
        double prevY;
        bool isControllingMinimap = false;
        bool isRightButtonPressed = false;
        bool isLeftButtonPressed = false;
        bool isMiddleButtonPressed = false;
    } mouse;

    /* Texture */
    GLuint wallTexture;
    GLuint floorTexture;


    /* Animation */
    double animationStartTime;
    Animation playerWalkingAnimation;
    Animation playerStandingAnimation;
    Animation playerPreparingToJumpAnimation;
    Animation playerJumpingAnimation;
    Animation* currentAnimation;

    // std::set<int> keysPressed;

    Player player;

    bool isKeyPressed(int key);

    /* sky box */
    static GLuint readTextureCubemap(std::string);
    GLuint skyboxVAO;
    GLuint skyboxVBO;
    void renderSkybox(const glm::mat4& Projection, const glm::mat4& View);

    /* Sound */

    /* Collision */

    /* Physics */

    struct World {
        glm::vec3 F_g; // Gravitational force
        float ufs; // co-efficient of static friction
        float ufk; // co-efficient of kinetic friction
        World(glm::vec3, float, float);

    } world;

    /* Transparency */



};


