#pragma once
#include "entity.h"
#include "camera.h"
#include "terrain.h"
#include <QFile>
#include <QString>
#include<iostream>
#include "modelloader.h"
#include <random>
#include"blockType.h"
class Player;

// Type of the object
enum ObjType : unsigned char
{
    // When adding a new type here, please do:
    // 1. Add texture mapping coordinate for new type face image in BlockTypeFunc::getBlockTypeFaceUV()
    // 2. Add texture mapping coordinate for new type item image in BlockTypeFunc::getBlockTypeItemUV()
    // 3. If new type is transparent, add it into BlockTypeFunc::checkBlockTypeTransparent()
    // 4. if new type is animateable, add it into BlockTypeFunc::checkBllokTypeAnimateable()
    STATIC_ASSET = 0, ME = 1, ZOMBIE = 2, ROBOT = 3, BIRD = 4, HUMAN = 5, EAGLE = 6, DEER = 7,
    RABBIT = 8, PIGGY = 9,  ME_ON_DEER = 10, LIFE_TREE = 11, SNAKE = 12
};

struct BoundingBox
{
    glm::vec3 topLeftFront;
    glm::vec3 topRightFront;
    glm::vec3 bottomLeftFront;
    glm::vec3 bottomRightFront;
    glm::vec3 topLeftBack;
    glm::vec3 topRightBack;
    glm::vec3 bottomLeftBack;
    glm::vec3 bottomRightBack;
};

class NPC : public Entity {
private:
    glm::vec3 m_velocity, m_acceleration;
    glm::vec3 m_prevPos;  // position before the tick
    ObjType type;       // type of the object
    float rot;          // total rotation angle
    BoundingBox bb;     // Bounding box of this object
    bool justTurned;   // the npc just turned 180 degrees

    static int setID()    // counter for id
    {
        static int id=0;
        return id++;
    }
    const int oid;  // the id of the object
    int tid;  // indicate which texture to bind


    const Terrain &mcr_terrain;

    Player* m_player;

    bool onGround;
    int  m_interfaceAxis;  // which axis to collide
    void computePhysics(float dT, const Terrain &terrain);
    void setOnGround(glm::vec3 *rayDirection, const Terrain &terrain);  // Check whether the player is on the ground
    bool gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection,
                   const Terrain &terrain, float *out_dist, glm::ivec3 *out_blockHit);
    void collisionDetection(glm::vec3 *rayDirection, const Terrain &terrain);

public:

    NPC(glm::vec3 pos, const Terrain &terrain, Player* player, int t, int type, OpenGLContext* context);
    virtual ~NPC() override;


    float joint1;
    float joint2;
    float joint3;
    float joint4;

    ModelLoader* m_loader;
    void tick(float dT, InputBundle& inputs) override;
    void tickNPC(float dT) ;
    glm::vec3 getPos();
    float getRot();
    int getType();

    int getTid();
    void loadModel(const QString& fileName, float scale);
    glm::vec3 getTranslation(); // get the translation at each tick

    void rotateOnUpGlobal(float degrees) override;

private:
    OpenGLContext* m_context;   // Since Qt's OpenGL support is done through classes like QOpenGLFunctions_3_2_Core,
                            // we need to pass our OpenGL context to the Drawable in order to call GL functions
                            // from within this class.
};
