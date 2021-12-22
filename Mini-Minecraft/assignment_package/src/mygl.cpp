#include "mygl.h"
#include <glm_includes.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>



MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_skyColor(glm::vec4(0.37f, 0.74f, 1.0f, 1)),
      m_geomQuad(this),
      m_skybox(this), m_progSkybox(this),
      m_worldAxes(this),
      m_progLambert(this), m_progFlat(this), m_progFlat2D(this), m_progPostprocessCurr(nullptr),
      m_frameBuffer(this, this->width(), this->height(), this->devicePixelRatio()), m_progCloud(this), m_cloud(this),
      m_rainShader(this), m_dirtShader(this), m_flyShader(this),
      m_progObj(this), m_ObjTexture1(this),m_ObjTexture2(this),m_ObjTexture3(this),
      m_textureAll(this), m_normalTextureAll(this), m_cloudTexture(this),
      uiCrosshair(this),
      m_terrain(this, this), terrainLoaded(false), m_player(glm::vec3(-170.f, 200.f, 60.f), m_terrain),
      m_objs(), m_weather(this),
      m_time(QDateTime::currentMSecsSinceEpoch()),
      m_sound(new QSound(QDir::currentPath().append(QString(" ../../assignment_package/sound_files/earthquake.wav")))),
      treeSoundPlayed(false)

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
    m_geomQuad.destroyVBOdata();
    m_frameBuffer.destroy();
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

    // Create the instance of the world axes
    m_worldAxes.createVBOdata();

//    m_frameBuffer.resize(this->width(), this->height(), this->devicePixelRatio());
    m_frameBuffer.create();
    m_geomQuad.createVBOdata();

    // Shaders
    createShaders();
    m_progPostprocessCurr = &m_progPostprocess[Postprocess::NOOPERATION];

    // Textures
    createTexture();
    bindTexture(m_progLambert);

    // Initialize Skybox
    m_skybox.createVBOdata();
    m_progSkybox.setDimensions(glm::ivec2(width(), height()));

    // Initialize Cloud plane
    m_progCloud.setModelMatrix(glm::mat4(1.f));
    m_progCloud.setTexture(2);
    m_cloudTexture.bind(2);
    m_cloud.setPlayerPos(m_player.mcr_camera.mcr_position);
    m_cloud.createVBOdata();

    m_weather.initialize();

    // Particles
    m_dirtParticleSystem.init(this, &m_player);
    m_flyParticleSystem.init(this, &m_player);

    // Load the models for the NPCs and buildings
    initObjs();

    m_ObjTexture1.bind(10);
    m_ObjTexture2.bind(11);
    m_ObjTexture3.bind(12);


    // Create UI
    uiCrosshair.createVBOdata();

    // Temp
    m_terrain.prog = &m_progLambert;

    // GameTime
    GameTime::time = 0;

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);
}

void MyGL::resizeGL(int w, int h) {
    //This code asets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_player.setCameraWidthHeight(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
    glm::mat4 viewproj = m_player.mcr_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)
    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);

    m_frameBuffer.destroy();
    m_frameBuffer.resize(w, h, this->devicePixelRatio());
    m_frameBuffer.create();
    for(auto &post : m_progPostprocess)
    {
        post.second.setDimensions(glm::ivec2(w * this->devicePixelRatio(), h * this->devicePixelRatio()));
    }


    m_progSkybox.setDimensions(glm::ivec2(w, h));

    printGLErrorLog();
}


// MyGL's constructor links tick() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to perform
// all per-frame actions here, such as performing physics updates on all
// entities in the scene.
void MyGL::tick() {
    float dT = (QDateTime::currentMSecsSinceEpoch() - this->m_time) / 1000.f;

    m_terrain.multithreadedWork(m_player.mcr_position, m_player.mcr_posPrev, dT);

    if(GameTime::time > 9 && !treeSoundPlayed)
    {
        this->m_sound->play();
        this->treeSoundPlayed = true;
    }

    this->m_player.tick(dT, this->m_inputs);

    m_weather.update();

    // Particle systems update
    m_dirtParticleSystem.update();
    m_flyParticleSystem.update();

    // Computes the positions for the npcs
    glm::vec4 pos[200];
    float rot[200];
    for (unsigned int i=0; i<this->m_objs.size();++i)
    {
        if (this->m_terrain.hasChunkAt(m_objs.at(i).get()->getPos().x, m_objs.at(i).get()->getPos().z))
        {
            this->m_objs.at(i).get()->tickNPC(dT);
            pos[i] = glm::vec4(this->m_objs[i]->getPos(), 0.f);
            rot[i] = glm::radians(this->m_objs[i]->getRot());
        }
    }

    m_progObj.setObjPos(pos);
    m_progObj.setObjRot(rot);

    if (this->m_player.isInWater())
    {
        m_progPostprocessCurr = &m_progPostprocess[Postprocess::INWATER];
    }
    else if (this->m_player.isInLava())
    {
        m_progPostprocessCurr = &m_progPostprocess[Postprocess::INLAVA];;
    }
    else
    {
        m_progPostprocessCurr = &m_progPostprocess[Postprocess::NOOPERATION];
    }

    this->m_time = QDateTime::currentMSecsSinceEpoch();

    GameTime::time += dT;

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
void MyGL::paintGL()
{
    // Set up
    m_frameBuffer.bindFrameBuffer();
    //glReadPixels((int)10, (int)10, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    // Render on the whole framebuffer, complete from the lower left corner to the upper right
    glViewport(0,0,this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // First render scene
    renderSkybox();    
    render3DScene();
    renderCloudPlane();
    renderRain();
    renderParticles();

    performPostprocessRenderPass();

    // Then render UI
    renderUI();

    // World axis
    glDisable(GL_DEPTH_TEST);
    m_progFlat.setModelMatrix(glm::mat4());
    m_progFlat.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    // m_progFlat.draw(m_worldAxes);
    glEnable(GL_DEPTH_TEST);
}

void MyGL::renderSkybox()
{
    glDisable(GL_DEPTH_TEST);
    m_progSkybox.setViewProjMatrix(glm::inverse(m_player.mcr_camera.getViewProj()));
    m_progSkybox.setCamEye(glm::vec4(m_player.mcr_camera.mcr_position, 1.f));
    m_progSkybox.setGameTimeMin(GameTime::getGameTimeInMin());
    m_progSkybox.setOffset(m_weather.getSunDir());  // Use offset channel to pass sun direction
    m_progSkybox.setWeather(glm::vec4(m_weather.getWeatherState(), m_weather.getWeatherRatio(), 0, 0));
    m_progSkybox.draw(m_skybox);
    glEnable(GL_DEPTH_TEST);
}

void MyGL::renderCloudPlane()
{
    m_progCloud.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progCloud.setGameTimeMin(GameTime::getGameTimeInMin());
    m_progCloud.setCamEye(glm::vec4(m_player.mcr_camera.mcr_position, 1.f));
    m_progCloud.setWeather(glm::vec4(m_weather.getWeatherState(), m_weather.getWeatherRatio(), 0, 0));

    glm::vec3 offset = m_player.mcr_camera.mcr_position - m_cloud.getPlayerPos();
    m_progCloud.setOffset(glm::vec4(offset[0], 0, offset[2], 0));

    m_cloudTexture.bind(2);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_progCloud.drawTransparentVBO(m_cloud);

    glDisable(GL_BLEND);
}

void MyGL::render3DScene()
{
    m_progFlat.setViewProjMatrix(m_player.mcr_camera.getViewProj());

    m_progLambert.setViewProjMatrix(m_player.mcr_camera.getViewProj());

    m_progLambert.setTime(GameTime::time);
    m_progLambert.setGameTimeMin(GameTime::getGameTimeInMin());
    m_progLambert.setCamEye(glm::vec4(this->m_player.mcr_camera.mcr_position, 1));
    m_progLambert.setOffset(m_weather.getSunDir());
    m_progLambert.setGeometryColor(m_weather.getSunLightColor());
    m_progLambert.setFogColor(m_weather.getFogColor());
    m_progLambert.setWeather(glm::vec4(m_weather.getWeatherState(), m_weather.getWeatherRatio(), 0, 0));


    // For Objects
    m_progObj.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progObj.setTime(GameTime::time);
    m_progObj.setCamEye(glm::vec4(this->m_player.mcr_camera.mcr_position, 1));

    m_ObjTexture1.bind(10);
    m_ObjTexture2.bind(11);
    m_ObjTexture3.bind(12);

    // bind a different texture file here if needed to
    this->drawObjs(m_progObj);

    // First draw opaque blocks
    m_terrain.draw(&m_progLambert)  ;

    // Then draw transparent blocks
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_terrain.drawTransparent(&m_progLambert);

    glDisable(GL_BLEND);
}

void MyGL::performPostprocessRenderPass()
{
    // Render the frame buffer as a texture on a screen-size quad

    // Tell OpenGL to render to the viewport's frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, this->defaultFramebufferObject());
    // Render on the whole framebuffer, complete from the lower left corner to the upper right
    glViewport(0,0,this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Bind our texture in Texture Unit 0
    m_frameBuffer.bindToTextureSlot(5);

    m_progPostprocessCurr->setTime(GameTime::time);
    m_progPostprocessCurr->draw(m_geomQuad, 5);
}

void MyGL::renderRain()
{
    WeatherState state = m_weather.getWeatherState();

    if(state != WeatherState::Rainy && state != WeatherState::GoldenRain)
        return;

    m_rainShader.setModelMatrix(glm::inverse(m_player.mcr_camera.getView()));
    m_rainShader.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_rainShader.setWeather(glm::vec4(state, m_weather.getWeatherRatio(), 0, 0));
    m_rainShader.setTime(GameTime::time);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_weather.drawRain(m_rainShader, m_player.mcr_camera.mcr_position);

    glDisable(GL_BLEND);
}

void MyGL::renderParticles()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Dirt particle
//    m_dirtShader.setModelMatrix(m_player.mcr_camera.getView());
//    m_dirtShader.setViewProjMatrix(m_player.mcr_camera.getProj());
//    m_dirtShader.setTime(GameTime::time);
//    m_dirtParticleSystem.drawParticles(m_dirtShader);

    // Fly particle
    m_flyShader.setModelMatrix(m_player.mcr_camera.getView());
    m_flyShader.setViewProjMatrix(m_player.mcr_camera.getProj());
    m_flyShader.setTime(GameTime::time);
    m_flyParticleSystem.drawParticles(m_flyShader);

    glDisable(GL_BLEND);
}

void MyGL::renderUI()
{
    // If want to close, return here
    return;
    glDisable(GL_DEPTH_TEST);

    m_progFlat2D.draw(uiCrosshair);

    glEnable(GL_DEPTH_TEST);
}

void MyGL::createShaders()
{
    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    m_progLambert.setModelMatrix(glm::mat4(1.f));
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    // Skybox shader
    m_progSkybox.create(":/glsl/postprocess/passThrough.vert.glsl", ":/glsl/sky.frag.glsl");
    // Cloud plane shader
    m_progCloud.create(":/glsl/cloudPlane.vert.glsl", ":/glsl/cloudPlane.frag.glsl");
    // 2D shader
    m_progFlat2D.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    m_progFlat2D.setModelMatrix(glm::mat4(1.f));
    m_progFlat2D.setViewProjMatrix(glm::mat4(1.f)); // Set it to identity matrix so that it draws elements directly to screen

    // Rain shader
    m_rainShader.create(":/glsl/rain.vert.glsl", ":/glsl/rain.frag.glsl");
    m_rainShader.setTexture(0);

    // Dirt shader
    m_dirtShader.create(":/glsl/dirtParticle.vert.glsl", ":/glsl/dirtParticle.frag.glsl");
    m_dirtShader.setTexture(0);

    // Fly shader
    m_flyShader.create(":/glsl/flyParticle.vert.glsl", ":/glsl/flyParticle.frag.glsl");
    m_flyShader.setTexture(0);

    // OBJ shader
    // Create and set up the diffuse shader
    m_progObj.create(":/glsl/obj.vert.glsl", ":/glsl/obj.frag.glsl");
    m_progObj.setModelMatrix(glm::mat4(1.f));
    m_progObj.setFogColor(m_skyColor);

    ShaderProgram noOperation(this);
    noOperation.create(":/glsl/postprocess/passThrough.vert.glsl", ":/glsl/postprocess/noOperation.frag.glsl");
    m_progPostprocess.emplace(Postprocess::NOOPERATION, noOperation);

    ShaderProgram inWater(this);
    inWater.create(":/glsl/postprocess/passThrough.vert.glsl", ":/glsl/postprocess/inWater.frag.glsl");
    m_progPostprocess.emplace(Postprocess::INWATER, inWater);

    ShaderProgram inLava(this);
    inLava.create(":/glsl/postprocess/passThrough.vert.glsl", ":/glsl/postprocess/inLava.frag.glsl");
    m_progPostprocess.emplace(Postprocess::INLAVA, inLava);
}

void MyGL::createTexture()
{
    m_textureAll.create(":/texture/minecraft_textures_all.png");
    m_normalTextureAll.create(":/texture/minecraft_normals_all.png");
    m_cloudTexture.create(":/texture/cloud_2048.png");

    m_ObjTexture1.create(":/texture/obj1.png");
    m_ObjTexture2.create(":/texture/obj2.png");
    m_ObjTexture3.create(":/texture/obj3.png");



    m_textureAll.load(0);
    m_normalTextureAll.load(1);
    m_cloudTexture.load(2);

    m_ObjTexture1.load(10);
    m_ObjTexture2.load(11);
    m_ObjTexture3.load(12);

}

void MyGL::bindTexture(ShaderProgram &program)
{
    m_textureAll.bind(0);
    m_normalTextureAll.bind(1);
    program.setTexture(0);
    program.setNormalMap(1);
}

void MyGL::initObjs()
{
    glm::vec4 joints[200];
    glm::vec4 pos[200];
    float rot[200];

    // Me
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-172.f, 180.f, 60.f), m_terrain, &m_player,11,1, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/me.obj")), 2.f);
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-172.f, 180.f, 60.f), m_terrain, &m_player,11,1, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/me_as_a_god.obj")), 2.f);

    // Me on Deer
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-172.f, 200.f, 65.f), m_terrain, &m_player,12,10, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/me_on_deer.obj")), 2.f);

    // Deer
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-172.f, 200.f, 65.f), m_terrain, &m_player,12,7, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/deer.obj")), 2.f);
    // Bind deer to player
    this->m_player.setDeer(this->m_objs.back().get());

    // TREE
    this->m_objs.push_back(mkU<NPC>(glm::vec3(115.f, 180.f, 121.f), m_terrain, &m_player,10, 11, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/tree.obj")), 1.f);

    // Seed
    this->m_objs.push_back(mkU<NPC>(glm::vec3(119.f,200.f, 126.f), m_terrain, &m_player,10, 0, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/seed.obj")), 3.5f);

//    // Piggy
//    this->m_objs.push_back(mkU<NPC>(glm::vec3(-172.f,180.f, 65.f), m_terrain, &m_player,11, 9, this));
//    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/piggy.obj")), 2.f);

    // Buildings
    // Love
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-135.f, 160.f, 27.f), m_terrain, &m_player,11, 0, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/love.obj")), 2.5f);
    // House 1
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-159.f, 160.f, 66.f), m_terrain, &m_player,11, 0, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/house1.obj")), 2.f);
    // House 2
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-149.f, 160.f, 96.f), m_terrain, &m_player,11, 0, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/house2.obj")), 2.f);
    // Hall of Fame
    this->m_objs.push_back(mkU<NPC>(glm::vec3(48.f, 160.f, -285.f), m_terrain, &m_player,11, 0, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/hall_of_fame.obj")),4.f);
    // Grave
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-259.f, 160.f, 53.f), m_terrain, &m_player,11, 0, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/grave.obj")), 2.f);

    // ZOMBIES
    // Set up RNG
    std::mt19937_64 rng;
   // initialize the random number generator with time-dependent seed
   uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
   std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
   rng.seed(ss);
   // initialize a uniform distribution between 0 and 1
   std::uniform_real_distribution<double> getRandom(0, 360);
   // ready to generate random numbers
    // Zombie 1
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-256.f, 160.f, 53.f), m_terrain, &m_player,11, 2, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/zombie1.obj")), 2.f);
    this->m_objs.back()->rotateOnUpGlobal(getRandom(rng));
    // Zombie 2
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-256.f, 150.f, 53.f), m_terrain, &m_player,11, 2, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/zombie2.obj")), 2.f);
   this->m_objs.back()->rotateOnUpGlobal(getRandom(rng));
    // Zombie 3
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-256.f, 150.f, 53.f), m_terrain, &m_player,11, 2, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/zombie3.obj")), 2.f);
   this->m_objs.back()->rotateOnUpGlobal(getRandom(rng));
    // Zombie 4
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-256.f, 150.f, 53.f), m_terrain, &m_player,11, 2, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/zombie4.obj")), 2.f);
   this->m_objs.back()->rotateOnUpGlobal(getRandom(rng));
    // Zombie 5
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-256.f, 150.f, 53.f), m_terrain, &m_player,11, 2, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/zombie5.obj")), 2.f);
   this->m_objs.back()->rotateOnUpGlobal(getRandom(rng));
    // Zombie 6
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-256.f, 150.f, 53.f), m_terrain, &m_player,11, 2, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/zombie6.obj")), 2.f);
   this->m_objs.back()->rotateOnUpGlobal(getRandom(rng));
    // Zombie 7
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-256.f, 150.f, 53.f), m_terrain, &m_player,11, 2, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/zombie7.obj")), 2.f);
   this->m_objs.back()->rotateOnUpGlobal(getRandom(rng));
    // Zombie 8
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-256.f, 150.f, 53.f), m_terrain, &m_player, 11, 2, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/zombie8.obj")), 2.f);
   this->m_objs.back()->rotateOnUpGlobal(getRandom(rng));

    // BIRDS
    // Bird 1
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-170.f, 180.f, 40.f), m_terrain, &m_player, 11, 4, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/bird1.obj")), 2.f);
    // Bird 2
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-110.f, 170.f, 44.f), m_terrain, &m_player,11, 4, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/bird2.obj")), 2.f);
    // Bird 3
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-150.f, 180.f, 40.f), m_terrain, &m_player, 11, 4, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/bird3.obj")), 2.f);
    this->m_objs.back()->rotateOnUpGlobal(45.f);
    // Bird 4
    this->m_objs.push_back(mkU<NPC>(glm::vec3(-190.f, 160.f, 50.f), m_terrain, &m_player,11, 4, this));
    this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/bird4.obj")), 2.f);
    this->m_objs.back()->rotateOnUpGlobal(60.f);

   // Rabbits
   this->m_objs.push_back(mkU<NPC>(glm::vec3(-190.f, 160.f,48.f), m_terrain, &m_player,11, 8, this));
   this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/rabbit.obj")), 1.f);
   this->m_objs.back()->rotateOnUpGlobal(60.f);
   this->m_objs.push_back(mkU<NPC>(glm::vec3(-190.f, 160.f, 50.f), m_terrain, &m_player,11, 8, this));
   this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/rabbit.obj")), 1.f);
   this->m_objs.push_back(mkU<NPC>(glm::vec3(-188.f, 160.f, 50.f), m_terrain, &m_player,11, 8, this));
   this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/rabbit.obj")), 1.f);
   this->m_objs.back()->rotateOnUpGlobal(90.f);

   // Eagle
   this->m_objs.push_back(mkU<NPC>(glm::vec3(-125.f, 190.f, 30.f), m_terrain, &m_player,11, 6, this));
   this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/eagle.obj")), 3.f);

   // Human
     this->m_objs.push_back(mkU<NPC>(glm::vec3(-139.f, 180.f, 90.f), m_terrain, &m_player,11,5, this));
     this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/human1.obj")), 2.f);
     this->m_objs.push_back(mkU<NPC>(glm::vec3(-159.f, 180.f, 80.f), m_terrain, &m_player,11,5, this));
     this->m_objs.back()->rotateOnUpGlobal(30.f);
     this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/human2.obj")), 2.f);
     this->m_objs.push_back(mkU<NPC>(glm::vec3(-140.f, 180.f, 76.f), m_terrain, &m_player,11,5, this));
     this->m_objs.back()->rotateOnUpGlobal(60.f);
     this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/human3.obj")), 2.f);
     this->m_objs.push_back(mkU<NPC>(glm::vec3(-140.f, 180.f, 88.f), m_terrain, &m_player,11,5, this));
     this->m_objs.back()->rotateOnUpGlobal(60.f);
     this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/human4.obj")), 2.f);

   // Snakes
   this->m_objs.push_back(mkU<NPC>(glm::vec3(-451.f, 180.f, 223.f), m_terrain, &m_player,11,12, this));
   this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/snake.obj")), 2.f);
   this->m_objs.back()->rotateOnUpGlobal(-90.f);
   this->m_objs.push_back(mkU<NPC>(glm::vec3(-533.f, 180.f, 131.f), m_terrain, &m_player,11,12, this));
   this->m_objs.back()->rotateOnUpGlobal(180.f);
   this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/snake.obj")), 2.f);
   this->m_objs.push_back(mkU<NPC>(glm::vec3(-440.f, 180.f, 283.f), m_terrain, &m_player,11,12, this));
   this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/snake.obj")), 2.f);
   this->m_objs.back()->rotateOnUpGlobal(-30.f);
   this->m_objs.push_back(mkU<NPC>(glm::vec3(-500.f, 180.f, 300.f), m_terrain, &m_player,11,12, this));
   this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/snake.obj")), 2.f);
   this->m_objs.back()->rotateOnUpGlobal(45.f);



    for (unsigned int i=0; i<m_objs.size(); ++i)
    {
        joints[i] = glm::vec4( this->m_objs[i]->joint1, this->m_objs[i]->joint2, this->m_objs[i]->joint3, this->m_objs[i]->joint4);
        pos[i] = glm::vec4(this->m_objs[i]->getPos(), 0.f);
        rot[i] = 0.f;
    }
    m_progObj.setJoints(joints);   // send the positions of these objs to OpenGL
    m_progObj.setObjPos(pos);   // send the positions of these objs to OpenGL
    m_progObj.setObjRot(rot);
}

void MyGL::addObj(QString fileName, glm::vec3 pos, int tid, int oid, float scale, float rot)
{
    glm::vec4 joints[200];
    glm::vec4 positions[200];
    float rotations[200];

    this->m_objs.push_back(mkU<NPC>(pos, m_terrain, &m_player,tid, oid, this));
    this->m_objs.back()->loadModel(fileName, scale);
    this->m_objs.back()->rotateOnUpGlobal(rot);

    for (unsigned int i=0; i<m_objs.size(); ++i)
    {
        joints[i] = glm::vec4( this->m_objs[i]->joint1, this->m_objs[i]->joint2, this->m_objs[i]->joint3, this->m_objs[i]->joint4);
        positions[i] = glm::vec4(this->m_objs[i]->getPos(), 0.f);
        rotations[i] = 0.f;
    }
    m_progObj.setJoints(joints);   // send the positions of these objs to OpenGL
    m_progObj.setObjPos(positions);   // send the positions of these objs to OpenGL
    m_progObj.setObjRot(rotations);
}

void MyGL::drawObjs(ShaderProgram &program)
{   
    program.setCamEye(glm::vec4(this->m_player.mcr_camera.mcr_position, 1));
    program.setOffset(m_weather.getSunDir());
    program.setGeometryColor(m_weather.getSunLightColor());
    program.setFogColor(m_weather.getFogColor());
    program.setWeather(glm::vec4(m_weather.getWeatherState(), m_weather.getWeatherRatio(), 0, 0));

    if (!this->m_player.firstPerson)
    {
    if (!this->m_player.onDeer)
    {
        program.setTexture(this->m_objs.at(0).get()->getTid());
        if (this->m_player.flightModeOn)
        {
            if (m_player.getVelocity().x == 0.f && m_player.getVelocity().z == 0.f)
            {
                program.setOffset(glm::vec4(0.f,0.f,0.f,0.f));
            }
            else
            {
                program.setOffset(glm::vec4(1.f,0.f,0.f,0.f));
            }
            m_progObj.draw(*(this->m_objs.at(1).get()->m_loader));
        }
        else
        {
            if ((m_player.getVelocity().x == 0.f && m_player.getVelocity().z == 0.f)
                    || (m_player.mcr_position.y - m_player.mcr_posPrev.y) < 0.f)
            {
                program.setOffset(glm::vec4(0.f,1.f,0.f,0.f));
            }
            else
            {
                program.setOffset(glm::vec4(1.f,1.f,0.f,0.f));
            }
            m_progObj.draw(*(this->m_objs.at(0).get()->m_loader));
        }
        // Only render if it is in a rendered Chunk
                if (m_terrain.hasChunkRendered(this->m_objs.at(3).get()->mcr_position.x, this->m_objs.at(3).get()->mcr_position.z))
        {
            // Draw deer if the player is not riding it
            program.setTexture(this->m_objs.at(2).get()->getTid());
            m_progObj.draw(*(this->m_objs.at(3).get()->m_loader));
        }
    }
        else
        {
            if ((m_player.getVelocity().x == 0.f && m_player.getVelocity().z == 0.f)
                    || (m_player.mcr_position.y - m_player.mcr_posPrev.y) < 0.f)
            {
                program.setOffset(glm::vec4(0.f,1.f,0.f,0.f));
            }
            else
            {
                program.setOffset(glm::vec4(1.f,1.f,0.f,0.f));
            }
            program.setTexture(this->m_objs.at(2).get()->getTid());
            m_progObj.draw(*(this->m_objs.at(2).get()->m_loader));
        }
    }
    else
    {
        if (m_player.onDeer)
        {
            program.setTexture(this->m_objs.at(2).get()->getTid());
            m_progObj.draw(*(this->m_objs.at(2).get()->m_loader));
        }
        else
        {
            // Draw deer if the player is not riding it
            if (m_terrain.hasChunkRendered(this->m_objs.at(3).get()->mcr_position.x, this->m_objs.at(3).get()->mcr_position.z))
            {
                program.setTexture(this->m_objs.at(2).get()->getTid());
                m_progObj.draw(*(this->m_objs.at(3).get()->m_loader));
            }
        }
    }

    // Always draw the world tree, because it's the world tree
    program.setTexture(this->m_objs.at(4).get()->getTid());
    m_progObj.draw(*(this->m_objs.at(4).get()->m_loader));


    for (unsigned int i=5; i<this->m_objs.size();++i)
    {
        if (this->m_objs.at(i)->getType() == 2)
        {
            if (m_terrain.hasChunkRendered(this->m_objs.at(i).get()->mcr_position.x, this->m_objs.at(i).get()->mcr_position.z))
        {
            int minInDay = GameTime::getGameTimeInMin() % 1440;

            if(minInDay < 300 || minInDay > 1300)
            {
                program.setTexture(this->m_objs.at(i).get()->getTid());
                m_progObj.draw(*(this->m_objs.at(i).get()->m_loader));
            }
        }
        }
        else
        {
            if (m_terrain.hasChunkRendered(this->m_objs.at(i).get()->mcr_position.x, this->m_objs.at(i).get()->mcr_position.z))
                {
                    program.setTexture(this->m_objs.at(i).get()->getTid());
                    m_progObj.draw(*(this->m_objs.at(i).get()->m_loader));
                }
        }
    }
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
            if (m_player.isInWater() || m_player.isInLava())
            {
                m_inputs.spacePressed = true;
            }
            else if (!e->isAutoRepeat())
            {
                m_inputs.spacePressed = true;
            }
        }
        if (e->key() == Qt::Key_T)
        {
            if (m_player.firstPerson == true)
            {
                m_player.resetCamera();
                glm::vec3 forw = glm::normalize(glm::vec3(m_player.getForward().x, 0.f, m_player.getForward().z));

                    m_player.setCameraDirection(-10.f *forw );
                    m_player.setCameraDirection(glm::vec3(0.f, 3.f, 0.f));

                    m_player.setCameraFov(75.f);
                    m_player.firstPerson = false;
            }
            else
            {
                glm::vec3 forw = glm::normalize(glm::vec3(m_player.getForward().x, 0.f, m_player.getForward().z));

                    m_player.setCameraDirection(10.f *forw );
                    m_player.setCameraDirection(glm::vec3(0.f, -3.f, 0.f));

                    m_player.setCameraFov(45.f);
                    m_player.firstPerson = true;
            }
        }
        if (e->key() == Qt::Key_O)
        {
            m_weather.startRain();
        }
        if (e->key() == Qt::Key_P)
        {
            m_weather.startRain(true);
        }
        if (e->key() == Qt::Key_R)
        {
            if (m_player.deerNear() && !m_player.firstPerson)
            {
                m_player.onDeer = !m_player.onDeer;
            }
        }
        if(e->key() == Qt::Key_L)
        {
            int x = -794;    // target x
            int z = 343;    // target z

             loadTerrain(x, z);
        }
        if (e->key() == Qt::Key_C)
        {
            this->m_player.cinematic = !this->m_player.cinematic;
            m_player.helixStartTime = 2.f;
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

void MyGL::loadTerrain(int x, int z)
{
    if (terrainLoaded)
    {
        return;
    }

       std::string l_string = LStringGenerator(2, "-YF","XFX-YF-YF+FX+FX-YF-YFFX+YF+FXFXYF-FX+YF+FXFX+YF-FXYF-YF-FX+FX+YFYF-",
                                               "+FXFX-YF-YF+FX+FXYF+FX-YFYF-FX-YF+FXYFYF-FX-YFFX+FX+YF-YF-FX+FX+YFY");
        // Load
        for (int i=140; i<160; i++)
        {
            LSystemParser(l_string, x, i, z);
        }

            for (int i=x; i >= (x-320); i-=16)
                        {
                                for (int j=z; j <= z+320; j+=16)
                                {
                                    m_terrain.getChunkAt(i, j)->destroyVBOdata();
                                    m_terrain.getChunkAt(i, j)->createVBOdata();
                                }
                        }

       this->terrainLoaded = true;

            this->m_objs.push_back(mkU<NPC>(glm::vec3(-877.f, 175.f, 441.f), m_terrain, &m_player,11, 3, this));
            this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/robot1.obj")), 5.f);


            this->m_objs.push_back(mkU<NPC>(glm::vec3(-878.f, 175.f, 397.f), m_terrain, &m_player,11, 3, this));
            this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/robot2.obj")), 5.f);

            this->m_objs.push_back(mkU<NPC>(glm::vec3(-884.f, 175.f, 393.f), m_terrain, &m_player,11, 3, this));
            this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/robot1.obj")), 5.f);


            this->m_objs.push_back(mkU<NPC>(glm::vec3(-869.f, 175.f, 393.f), m_terrain, &m_player,11, 3, this));
            this->m_objs.back()->loadModel(QDir::currentPath().append(QString("../../assignment_package/obj_files/robot2.obj")), 5.f);
}
void MyGL::LSystemParser(std::string str, int x, int y, int z)
{
    int length = 4;
    int sign = -1;
    int xz[2] = {x, z};
    int direction = 0;
    for (unsigned int i=0; i<str.length(); ++i)
    {
        if (str.at(i) == 'X')
        {
            for (int i=0; i<length; i++)
            {
                xz[direction] += sign * 1;

                m_terrain.setBlockAt(xz[0], y, xz[1], METAL);

             }
        }
        else if (str.at(i) == 'Y')
        {
            for (int i=0; i<length; i++)
            {
                xz[direction] += sign * 1;

                m_terrain.setBlockAt(xz[0], y, xz[1], METAL);

             }
        }
        else if (str.at(i) == 'F')
        {
            for (int i=0; i<length; i++)
            {
                xz[direction] += sign * 1;

                m_terrain.setBlockAt(xz[0], y, xz[1], METAL);

             }
        }
        else if (str.at(i) == '+')
        {
            if(direction == 1)
            {
                direction = 0;
                sign = -sign;
            }
            else
            {
                direction = 1;
            }
        }
        else if (str.at(i) == '-')
        {
            if(direction == 0)
            {
                direction = 1;
                sign = -sign;
            }
            else
            {
                direction = 0;
            }
        }
    }
}

std::string LStringGenerator(int iteration, std::string startStr, std::string ruleX, std::string ruleY )
{
    std::string str = startStr;
    std::string newStr = "";
    for (int i=0; i<iteration; ++i)
    {
        newStr = getLString(str,ruleX, ruleY);
        str = newStr;
    }
    return newStr;
}


std::string getLString(std::string startStr, std::string ruleX, std::string ruleY )
{
    std::string newStr = "";

    for (char c : startStr)
    {
        newStr.append(applyRule(c, ruleX,ruleY));
    }
    return newStr;
}

std::string applyRule(char c, std::string ruleX, std::string ruleY )
{
    std::string newStr = "";
        if (c=='X')
        {
            newStr = ruleX;
        }
        else if (c=='Y')
        {
            newStr = ruleY;
        }
        else
        {
            newStr = c;
        }
    return newStr;
}

bool MyGL::playerOnGround()
{
    if(this->m_player.isOnGround())
    {
        return true;
    }
    else
    {
        return false;
    }
}
