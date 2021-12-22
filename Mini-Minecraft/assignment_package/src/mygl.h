#ifndef MYGL_H
#define MYGL_H

#include "openglcontext.h"
#include "shaderprogram.h"
#include "scene/worldaxes.h"
#include "scene/camera.h"
#include "scene/terrain.h"
#include "scene/player.h"
#include "scene/npc.h"
#include "texture.h"
#include "ui/uicrosshair.h"
#include "scene/quad.h"
#include "unordered_map"
#include "framebuffer.h"
#include "scene/cloudplane.h"
#include "game/gameTime.h"
#include "game/weather.h"
#include "scene/particlesystem.h"

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <smartpointerhelp.h>

#include<QDateTime>
#include<QRunnable>
#include<QMutex>
#include<QDir>
#include<QtMultimedia/QSound>

class MyGL : public OpenGLContext
{
    Q_OBJECT

private:
    glm::vec4 m_skyColor;

private:
    /// The screen-space quadrangle used to draw
    /// the scene with the post-process shaders.
    Quad m_geomQuad;

    Quad m_skybox;
    ShaderProgram m_progSkybox;

    WorldAxes m_worldAxes; // A wireframe representation of the world axes. It is hard-coded to sit centered at (32, 128, 32).
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)
    ShaderProgram m_progFlat2D; // Shader program that draws elements directly to screen
    //ShaderProgram m_progInstanced;// A shader program that is designed to be compatible with instanced rendering
    std::unordered_map<Postprocess, ShaderProgram, EnumHash> m_progPostprocess;
    ShaderProgram* m_progPostprocessCurr;
    FrameBuffer m_frameBuffer;

    ShaderProgram m_progCloud;
    CloudPlane m_cloud;

    ShaderProgram m_rainShader;

    // Dirt particle
    DirtParticleSystem m_dirtParticleSystem;
    ShaderProgram m_dirtShader;

    // Fly particle
    FlySparkParticleSystem m_flyParticleSystem;
    ShaderProgram m_flyShader;

    ShaderProgram m_progObj;    // shader for objects of npcs/buildings
    Texture m_ObjTexture1;
    Texture m_ObjTexture2;
    Texture m_ObjTexture3;

    Texture m_textureAll;
    Texture m_normalTextureAll;
    Texture m_cloudTexture;


    // UI elements
    UICrosshair uiCrosshair;

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Terrain m_terrain; // All of the Chunks that currently comprise the world.
    bool terrainLoaded; // T if the terrain is already re-written by the L-System

    Player m_player; // The entity controlled by the user. Contains a camera to display what it sees as well.

    std::vector<uPtr<NPC>> m_objs;  // a pointer to all npcs/buildings in the world

    InputBundle m_inputs; // A collection of variables to be updated in keyPressEvent, mouseMoveEvent, mousePressEvent, etc.

    QTimer m_timer; // Timer linked to tick(). Fires approximately 60 times per second.

    void moveMouseToCenter(); // Forces the mouse position to the screen's center. You should call this
                              // from within a mouse move event after reading the mouse movement so that
                              // your mouse stays within the screen bounds and is always read.

    void sendPlayerDataToGUI() const;

public:

    Weather m_weather;
    qint64 m_time;       // the currentMSecsSinceEpoch of the current frame
    QSound* m_sound;
    bool treeSoundPlayed;

    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    // Called once when MyGL is initialized.
    // Once this is called, all OpenGL function
    // invocations are valid (before this, they
    // will cause segfaults)
    void initializeGL() override;
    // Called whenever MyGL is resized.
    void resizeGL(int w, int h) override;
    // Called whenever MyGL::update() is called.
    // In the base code, update() is called from tick().
    void paintGL() override;

    void initObjs();    // initialize the npcs/buildings
    void drawObjs(ShaderProgram &program);    // draw all the objects in the vector
    void addObj(QString fileName, glm::vec3 pos, int tid, int oid, float scale, float rot);

    void renderSkybox();

    void render3DScene();
    void performPostprocessRenderPass();

    void renderCloudPlane();

    void renderParticles();

    void renderRain();

    void renderUI();

    void createShaders();
    void createTexture();
    void bindTexture(ShaderProgram &);

    void loadTerrain(int x, int z);

    void LSystemParser(std::string, int, int, int);
    bool playerOnGround();


protected:
    // Automatically invoked when the user
    // presses a key on the keyboard
    void keyPressEvent(QKeyEvent *e) override;
    // releases a key on the keyboard
    void keyReleaseEvent(QKeyEvent *e) override;
    // Automatically invoked when the user
    // moves the mouse
    void mouseMoveEvent(QMouseEvent *e) override;
    // Automatically invoked when the user
    // presses a mouse button
    void mousePressEvent(QMouseEvent *e) override;


private slots:
    void tick(); // Slot that gets called ~60 times per second by m_timer firing.

signals:
    void sig_sendPlayerPos(QString) const;
    void sig_sendPlayerVel(QString) const;
    void sig_sendPlayerSpeed(QString) const;
    void sig_sendPlayerAcc(QString) const;
    void sig_sendPlayerLook(QString) const;
    void sig_sendPlayerChunk(QString) const;
    void sig_sendPlayerTerrainZone(QString) const;
    void sig_sendPlayerMode(QString) const;
    void sig_sendPlayerGround(QString) const;
};

std::string LStringGenerator(int iteration, std::string startStr, std::string ruleX, std::string ruleY );
std::string getLString(std::string startStr, std::string ruleX, std::string ruleY );
std::string applyRule(char c, std::string ruleX, std::string ruleY );
#endif // MYGL_H
