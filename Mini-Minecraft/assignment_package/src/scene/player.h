#pragma once
#include "entity.h"
#include "camera.h"
#include "terrain.h"
class NPC;
class Player : public Entity {
private:
    glm::vec3 m_velocity, m_acceleration;
    Camera m_camera;
    const Terrain &mcr_terrain;
    bool keyPressed;
    bool onGround;
    int  m_interfaceAxis;  // which axis to collide
    void processInputs(InputBundle &inputs);
    void computePhysics(float dT, const Terrain &terrain);

    void setKeyPressed(InputBundle &);  // Check whether the player needs to move
    void setOnGround(glm::vec3 *rayDirection, const Terrain &terrain);  // Check whether the player is on the ground

    bool gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection,
                   const Terrain &terrain, float *out_dist, glm::ivec3 *out_blockHit);
    void collisionDetection(glm::vec3 *rayDirection, const Terrain &terrain);

public:
    // Player's previous position, for easy access from MyGL
    glm::vec3 mcr_posPrev;

    // Readonly public reference to our camera
    // for easy access from MyGL
    const Camera& mcr_camera;

    bool firstPerson; // T for first person, F for third person
    bool onDeer;      // T if player is riding the deer
    float totalRot;     // the total rotation degrees so far
    NPC* m_deer;        // Points to the player's deer
    bool cinematic;         // T if cinematic mode is on
    float helixStartTime;


    Player(glm::vec3 pos, const Terrain &terrain);
    virtual ~Player() override;

    glm::vec3 getForward();
    glm::vec3 getUp();
    glm::vec3 getVelocity();
    void setCameraWidthHeight(unsigned int w, unsigned int h);

    void setCameraDirection(glm::vec3 d);
    void setCameraFov(float z);
    void setDeer(NPC* deer);
    bool deerNear();    // T if the deer is near the player
    void resetCamera();
    void cinematicMove(float dT);


    void tick(float dT, InputBundle &input) override;

    void placeBlock(Terrain*);
    void removeBlock(Terrain*);

    //Player State Check
    bool isInWater();
    bool isInLava();
    bool isOnGround()
    {
        return onGround;
    }

    // Player overrides all of Entity's movement
    // functions so that it transforms its camera
    // by the same amount as it transforms itself.
    void moveAlongVector(glm::vec3 dir) override;
    void moveForwardLocal(float amount) override;
    void moveRightLocal(float amount) override;
    void moveUpLocal(float amount) override;
    void moveForwardGlobal(float amount) override;
    void moveRightGlobal(float amount) override;
    void moveUpGlobal(float amount) override;
    void rotateOnForwardLocal(float degrees) override;
    void rotateOnRightLocal(float degrees) override;
    void rotateOnUpLocal(float degrees) override;
    void rotateOnForwardGlobal(float degrees) override;
    void rotateOnRightGlobal(float degrees) override;
    void rotateOnUpGlobal(float degrees) override;    
    // For sending the Player's data to the GUI
    // for display
    QString posAsQString() const;
    QString velAsQString() const;
    QString speedAsQString() const;
    QString accAsQString() const;
    QString lookAsQString() const;
    QString modeAsQString() const;
    QString groundAsQString() const;

};
