#include "mygl.h"
#include <glm_includes.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_skyColor(glm::vec4(0.37f, 0.74f, 1.0f, 1)),
      m_worldAxes(this),
      m_progLambert(this), m_progFlat(this), m_progInstanced(this),
      m_textureAll(this), m_normalTextureAll(this),
      m_terrain(this), m_player(glm::vec3(48.f, 240.f, 48.f), m_terrain),
      m_time(QDateTime::currentMSecsSinceEpoch())
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    m_timer.start(16);
    setFocusPolicy(Qt::ClickFocus);

    setMouseTracking(true); // MyGL will track the mouse's movements even if a mouse button is not pressed
    setCursor(Qt::BlankCursor); // Make the cursor invisible
}

MyGL::~MyGL() {
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
}


void MyGL::moveMouseToCenter() {
    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    // Set the color with which the screen is filled at the start of each render call.   
    glClearColor(m_skyColor[0], m_skyColor[1], m_skyColor[2], m_skyColor[3]);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instance of the world axes
    m_worldAxes.createVBOdata();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    m_progInstanced.create(":/glsl/instanced.vert.glsl", ":/glsl/lambert.frag.glsl");

    m_progLambert.setFogColor(m_skyColor);

    createTexture();

    // Set a color with which to draw geometry.
    // This will ultimately not be used when you change
    // your program to render Chunks with vertex colors
    // and UV coordinates
    //m_progLambert.setGeometryColor(glm::vec4(0,1,0,1));

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

    //m_terrain.CreateTestScene();
}

void MyGL::resizeGL(int w, int h) {
    //This code asets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_player.setCameraWidthHeight(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
    glm::mat4 viewproj = m_player.mcr_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}


// MyGL's constructor links tick() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to perform
// all per-frame actions here, such as performing physics updates on all
// entities in the scene.
void MyGL::tick() {

    float dT = (QDateTime::currentMSecsSinceEpoch() - this->m_time) / 1000.f;
    this->m_time = QDateTime::currentMSecsSinceEpoch();

    // Step 1: See if needed to spawn more workers to operate on threads
    //     in order to generate more BlockType data for new Chunks
    // 1a: these worker threads will write their completed Chunks to a vector of Chunks*
    //  that MyGL checks every tick. For each Chunk in this vector, we perform step 2

    // Step 2: See if needed to spawn workers to compute the VBO data for chunks
    //     that have BlockType data but don't have VBO data
    // 2a: In addition to querying the vector written to in 1a, also check the Chunks
    //      around the player for lack of VBO data (add a flag to a Chunk to indicate this)

    // Step 3: Take all of the VBOData produced by step 2 threads and pass it to the GPU
    //  (can't be done from other threads because they do not have the correct OpenGL connection to the GPU)

    // struct VBOData {
    // std::vector <float> interleaveData;
    // std::vector <Gluint> indices;
    // Chunk* associated_chunk

    // Shared Resources:
    // std::vector <Chunk*> chunksWithBlockTypeData;
    // This is written to by each worker thread that computes the height field of terrain
    // Read by MyGL each tick(), clear()ed when it has been read
    // std::vector <VBOData> chunksWithNewVBOData;
    // This is written to by each worker thread that computes the VBO data for chunks
    // Read by MyGL each tick(), clear()ed when it has been read

    // Every 3x3 chunk area is a terrain zone
    // The 3x3 set of Terrain Zones around the player is always rendered in paintGL
    // The 5x5 set of Terrain Zones around the player always has its BlockType data and VBO data at hand
    // Any Chunks farther away than 20 Chunk widths are de-loaded (VBO deleted, BlockTypes preserved)


    glm::vec3 player_prev_pos = m_player.mcr_position;

    m_terrain.updateTerrain(m_player.mcr_position);

    this->m_player.tick(dT, this->m_inputs);
    update(); // Calls paintGL() as part of a larger QOpenGLWidget pipeline
    sendPlayerDataToGUI(); // Updates the info in the secondary window displaying player data
}

void MyGL::sendPlayerDataToGUI() const {
    emit sig_sendPlayerPos(m_player.posAsQString());
    emit sig_sendPlayerVel(m_player.velAsQString());
    emit sig_sendPlayerSpeed(m_player.speedAsQString());
    emit sig_sendPlayerAcc(m_player.accAsQString());
    emit sig_sendPlayerLook(m_player.lookAsQString());
    glm::vec2 pPos(m_player.mcr_position.x, m_player.mcr_position.z);
    glm::ivec2 chunk(16 * glm::ivec2(glm::floor(pPos / 16.f)));
    glm::ivec2 zone(64 * glm::ivec2(glm::floor(pPos / 64.f)));
    emit sig_sendPlayerChunk(QString::fromStdString("( " + std::to_string(chunk.x) + ", " + std::to_string(chunk.y) + " )"));
    emit sig_sendPlayerTerrainZone(QString::fromStdString("( " + std::to_string(zone.x) + ", " + std::to_string(zone.y) + " )"));
    emit sig_sendPlayerMode(m_player.modeAsQString());
    emit sig_sendPlayerGround(m_player.groundAsQString());

}

// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so paintGL() called at a rate of 60 frames per second.
void MyGL::paintGL() {
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_progFlat.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progLambert.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progInstanced.setViewProjMatrix(m_player.mcr_camera.getViewProj());

    m_progLambert.setModelMatrix(glm::mat4(1.f));

    m_progLambert.setTime(this->m_time);
    m_progLambert.setCamEye(glm::vec4(this->m_player.mcr_camera.mcr_position, 1));

    renderTerrain();

    glDisable(GL_DEPTH_TEST);
    m_progFlat.setModelMatrix(glm::mat4());
    m_progFlat.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progFlat.draw(m_worldAxes);
    glEnable(GL_DEPTH_TEST);
}


void MyGL::renderTerrain()
{
    bindTexture(m_progLambert);

    // First draw opaque blocks
    m_terrain.draw(&m_progLambert);

    // Then draw transparent blocks
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_terrain.drawTransparent(&m_progLambert);

    glDisable(GL_BLEND);
}

void MyGL::createTexture()
{
    m_textureAll.create(":/texture/minecraft_textures_all.png");
    m_normalTextureAll.create(":/texture/minecraft_normals_all.png");

    m_textureAll.load(0);
    m_normalTextureAll.load(1);
}

void MyGL::bindTexture(ShaderProgram &program)
{
    m_textureAll.bind(0);
    m_normalTextureAll.bind(1);
    program.setTexture(0);
    program.setNormalMap(1);
}

void MyGL::keyPressEvent(QKeyEvent *e) {
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
        if (e->key() == Qt::Key_Escape) {
            QApplication::quit();
        }
        if (e->key() == Qt::Key_Right) {
            m_inputs.rightPressed = true;
        }
        if (e->key() == Qt::Key_Left) {
            m_inputs.leftPressed = true;
        }
        if (e->key() == Qt::Key_Up) {
            m_inputs.upPressed = true;
        }
        if (e->key() == Qt::Key_Down) {
            m_inputs.downPressed = true;
        }
        if (e->key() == Qt::Key_W) {
            m_inputs.wPressed = true;
        }
        if (e->key() == Qt::Key_S) {
            m_inputs.sPressed = true;
        }
        if (e->key() == Qt::Key_D) {
            m_inputs.dPressed = true;
        }
        if (e->key() == Qt::Key_A) {
            m_inputs.aPressed = true;
        }
        if (e->key() == Qt::Key_Q) {
            m_inputs.qPressed = true;
        }
        if (e->key() == Qt::Key_E) {
            m_inputs.ePressed = true;
        }
        if (e->key() == Qt::Key_F) {
            if (!e->isAutoRepeat())
            {
                m_player.flightModeOn = !m_player.flightModeOn;
            }
        }
        if(e->key() == Qt::Key_Space) {
            if (!e->isAutoRepeat())
                {
                m_inputs.spacePressed = true;
                }
            }
}

void MyGL::keyReleaseEvent(QKeyEvent *e){
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    }
    if (e->key() == Qt::Key_Right) {
        m_inputs.rightPressed = false;
    }
    if (e->key() == Qt::Key_Left) {
        m_inputs.leftPressed = false;
    }
    if (e->key() == Qt::Key_Up) {
        m_inputs.upPressed = false;
    }
    if (e->key() == Qt::Key_Down) {
        m_inputs.downPressed = false;
    }
    if (e->key() == Qt::Key_W) {
        m_inputs.wPressed = false;
    }
    if (e->key() == Qt::Key_S) {
        m_inputs.sPressed = false;
    }
    if (e->key() == Qt::Key_D) {
        m_inputs.dPressed = false;
    }
    if (e->key() == Qt::Key_A) {
        m_inputs.aPressed = false;
    }
    if (e->key() == Qt::Key_Q) {
        m_inputs.qPressed = false;
    }
    if (e->key() == Qt::Key_E) {
        m_inputs.ePressed = false;
    }
    if(e->key() == Qt::Key_Space) {
        m_inputs.spacePressed = false;
    }
}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
        const float SENSITIVITY = 20.0;
        // x, y coords of the mouse
        float x = e->pos().x();
        float y = e->pos().y();
        float dx = (width() * 0.5 - x) / width();
        float dy = (height() * 0.5 - y) / height();
        if (dx != 0)
        {
            m_player.rotateOnUpGlobal(dx* SENSITIVITY);
        }
        if (dy != 0)
        {
            m_player.rotateOnRightLocal(dy * SENSITIVITY);
        }

        moveMouseToCenter();
}

void MyGL::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::LeftButton) {
         m_player.removeBlock(&m_terrain);
     } else if (e->button() == Qt::RightButton) {
         m_player.placeBlock(&m_terrain);
     }
}
