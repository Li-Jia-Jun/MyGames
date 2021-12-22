#include "player.h"
#include"npc.h"
#include <QString>

Player::Player(glm::vec3 pos, const Terrain &terrain)
    : Entity(pos, true),  m_velocity(0,0,0), m_acceleration(0,0,0),
      m_camera(pos + glm::vec3(0, 2.f, 0)),  mcr_terrain(terrain),
      keyPressed(false), onGround(true),  mcr_posPrev(glm::vec3(0,0,0)),
      mcr_camera(m_camera), firstPerson(true),onDeer(false), totalRot(0.f),  m_deer(nullptr),
      cinematic(false), helixStartTime(2.f)
{}

Player::~Player()
{}

glm::vec3 Player::getVelocity()
{
    return this->m_velocity;
}
glm::vec3 Player::getForward()
{
    return this->m_forward;
}

glm::vec3 Player::getUp()
{
    return this->m_up;
}

void Player::cinematicMove(float dT)
{
    // Eagle view
//    this->rotateOnUpGlobal(0.06f);
//    m_velocity.x = this->m_forward.x;
//    m_velocity.z = this->m_forward.z;
//    m_velocity.y = 0.05f;
//    glm::vec3 direction = 18.f * m_velocity * dT;
//    this->moveAlongVector(direction);

    helixStartTime += dT;

    // Helix view
    this->rotateOnUpGlobal(0.5f);
    glm::vec3 tangent = glm::rotateY(this->m_forward,-(float)M_PI/2);
    m_velocity.x = 0.2 * helixStartTime * 12.5*tangent.x;
    m_velocity.z = 0.2 * helixStartTime * 12.5*tangent.z;
    m_velocity.y = 2.5f;
    glm::vec3 direction = m_velocity * dT;
    this->moveAlongVector(direction);
    return;
}

void Player::tick(float dT, InputBundle &input) {
    this->mcr_posPrev = this->mcr_position; // Save the previous position
    if (this->cinematic)
    {
        cinematicMove(dT);
        return;
    }
    processInputs(input);
    computePhysics(dT, mcr_terrain);
}

void Player::setKeyPressed(InputBundle &inputs){
    if (this->flightModeOn)
    {
        if (!inputs.wPressed && !inputs.sPressed && !inputs.dPressed && !inputs.aPressed
                && !inputs.ePressed && !inputs.qPressed)
        {
            this->keyPressed = false;
        }
        else
        {
            this->keyPressed = true;
        }
    }
    else
    {
        if (!inputs.wPressed && !inputs.sPressed && !inputs.dPressed && !inputs.aPressed
                && !inputs.spacePressed)
        {
            this->keyPressed = false;
        }
        else
        {
            this->keyPressed = true;
        }
    }
}

void Player::setOnGround(glm::vec3 *rayDirection, const Terrain &terrain)
{
    glm::ivec3 out_blockHit = glm::ivec3(0);
    float out_dist = 0.f;
    glm::vec3 corner = this->m_position - glm::vec3(0.5f, 0.f, 0.5f);

    for (int x = 0; x < 2; x++)
    {
           for (int z = 0; z < 2; z++)
           {

                    glm::vec3 rayOrigin = corner + glm::vec3(x, 0.f, z);
                    if (gridMarch(rayOrigin, *rayDirection, terrain, &out_dist, &out_blockHit)&& (out_dist < 0.01f))
                    {
                        this->onGround = true;
                        return;
                    }
           }
    }

        this->onGround =  false;
}

void Player::setDeer(NPC *deer)
{
    this->m_deer = deer;
}

bool Player::deerNear()
{
    float dist = abs(glm::distance(this->m_position, this->m_deer->getPos()));
    if (dist < 3.f)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Player::resetCamera()
{
    this->m_camera.reset(m_forward);
    this->m_up = glm::vec3(0.f, 1.f, 0.f);
    this->m_forward = glm::normalize(glm::vec3(m_forward.x, 0.f, m_forward.z));
    this->m_right = glm::cross(m_forward, m_up);
}

void Player::setCameraDirection(glm::vec3 d)
{
    this->m_camera.setDirection(d);
}

void Player::processInputs(InputBundle &inputs) {
    float acceleration = 5.38f;     // human average acceleration
    glm::vec3 direction = glm::vec3(0.f);

    // The acceleration should be 2/3 of normal in fluid
    if (isInWater() || isInLava()) acceleration /= 1.5f;

    // Rotating the camera
    float angles = 1.f;
    if (inputs.upPressed)
    {
        this->rotateOnRightLocal(-angles);
    }
    if (inputs.downPressed)
    {
        this->rotateOnRightLocal(angles);
    }
    if (inputs.rightPressed)
    {
        this->rotateOnUpGlobal(-angles);
    }
    if (inputs.leftPressed)
    {
        this->rotateOnUpGlobal(angles);
    }

    // Moving the player
    //  in Flight Mode
    if (this->flightModeOn)
    {
        if (inputs.wPressed)
        {
            direction += this->m_forward;
        }
        if (inputs.sPressed)
        {
            direction -= this->m_forward;
        }
        if (inputs.dPressed)
        {
            direction += this->m_right;
        }
        if (inputs.aPressed)
        {
            direction -= this->m_right;
        }
        if (inputs.ePressed)
        {
            direction += this->m_up;
        }
        if (inputs.qPressed)
        {
            direction -= this->m_up;
        }
           this->m_acceleration = acceleration * direction;
    }
    //  not in flight Mode
    else
    {
        // Disable continuous jump
       if (!this->onGround && !isInWater() && !isInLava())
       {
           inputs.spacePressed = false;
       }
       if (inputs.wPressed)
        {
            direction += glm::normalize(glm::vec3(this->m_forward.x, 0.f, this->m_forward.z));
        }
        if (inputs.sPressed)
        {
            direction -= glm::normalize(glm::vec3(this->m_forward.x, 0.f, this->m_forward.z));
        }
        if (inputs.dPressed)
        {
            direction += glm::normalize(glm::vec3(this->m_right.x, 0.f, this->m_right.z));
        }
        if (inputs.aPressed)
        {
            direction -=  glm::normalize(glm::vec3(this->m_right.x, 0.f, this->m_right.z));
        }
        if (inputs.spacePressed)
        {
            direction += this->m_up;
        }
        this->m_acceleration = acceleration * glm::normalize(direction);
    }
    this->setKeyPressed(inputs);    // Used to decide whether the player moves
    // This if makes sure that m_acceleration is not NAN
    if (direction == glm::vec3(0.f))
    {
        m_acceleration = glm::vec3(0.f);
    }
    if (inputs.spacePressed)
    {
        // if the player is in fluid, pressing space will raise height constantly
        if (isInWater() || isInLava())
        {
            m_acceleration.y = 10.81f;
        }
        else m_acceleration.y = 40.f * m_acceleration.y; // Need a force to overcome gravity
    }
}


void Player::computePhysics(float dT, const Terrain &terrain) {

    // Shoot a ray downwards to test whether player is on ground
    glm::vec3 downwards = glm::vec3(0.f,-1.f,0.f);
    this->setOnGround(&downwards, terrain);

    float friction = 0.983f;    // This makes the velocity approximate 5m/s
    glm::vec3 gravity = glm::vec3(0.f, -9.81f, 0.f);  // gravity is 9.81 m/s^2 pointing downwards

    if (!this->flightModeOn)
    {
        // Apply the gravity if not on the ground
        if(!this->onGround)
        {
            // TODO: in water or lava ??this->m_acceleration += gravity; twice?
            this->m_acceleration += gravity;
            this->m_velocity.x *= friction;
            this->m_velocity.z *= friction;

            this->m_velocity += this->m_acceleration  * dT;

            this->m_acceleration = 0.4f * gravity + m_acceleration;
        }
        else
        {
            if (keyPressed)
            {
                this->m_velocity *= friction;
                this->m_velocity += this->m_acceleration * dT;
            }
            else
            {
                this->m_velocity = glm::vec3(0.f);
            }
        }
    }
    // In flight mode
    else
    {
        if (keyPressed)
        {
            this->m_velocity += m_acceleration * dT;
            if (m_velocity.x > 20.f)
            {
                m_velocity.x = 20.f;
            }
            else if (m_velocity.x < -20.f)
            {
                m_velocity.x = -20.f;
            }
            if (m_velocity.z > 20.f)
            {
                m_velocity.z = 20.f;
            }
            else if (m_velocity.z < -20.f)
            {
                m_velocity.z = -20.f;
            }
        }
        else
        {
            this->m_velocity = glm::vec3(0.f);
        }
    }

    if (!keyPressed && mcr_terrain.getBlockAt(this->m_position) == WATER)
    {
        this->m_velocity.x -= 0.5f;
    }
    glm::vec3 direction = m_velocity * dT;

    // Do a collision detection if not in flight mode
    if (!this->flightModeOn)
    {
        collisionDetection(&direction, terrain);
    }
    this->moveAlongVector(direction);

}


bool Player::gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain, float *out_dist, glm::ivec3 *out_blockHit) {
    float maxLen = glm::length(rayDirection); // Farthest we search
    glm::ivec3 currCell = glm::ivec3(glm::floor(rayOrigin));
    rayDirection = glm::normalize(rayDirection); // Now all t values represent world dist.

    float curr_t = 0.f;
    while(curr_t < maxLen) {
        float min_t = glm::sqrt(3.f);
        float interfaceAxis = -1; // Track axis for which t is smallest
        for(int i = 0; i < 3; ++i) { // Iterate over the three axes
            if(rayDirection[i] != 0) { // Is ray parallel to axis i?
                float offset = glm::max(0.f, glm::sign(rayDirection[i])); // See slide 5
                // If the player is *exactly* on an interface then
                // they'll never move if they're looking in a negative direction
                if(currCell[i] == rayOrigin[i] && offset == 0.f) {
                    offset = -1.f;
                }
                int nextIntercept = currCell[i] + offset;
                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
                axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
                if(axis_t < min_t) {
                    min_t = axis_t;
                    interfaceAxis = i;
                }
            }
        }
        if(interfaceAxis == -1) {
            throw std::out_of_range("interfaceAxis was -1 after the for loop in gridMarch!");
        }

        this->m_interfaceAxis = interfaceAxis;    // store which axis collides

        curr_t += min_t; // min_t is declared in slide 7 algorithm
        rayOrigin += rayDirection * min_t;
        glm::ivec3 offset = glm::ivec3(0,0,0);
        // Sets it to 0 if sign is +, -1 if sign is -
        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));
        currCell = glm::ivec3(glm::floor(rayOrigin)) + offset;
        // If currCell contains something other than EMPTY, return
        // curr_t
        BlockType cellType = terrain.getBlockAt(currCell.x, currCell.y, currCell.z);
        // Add cellTypes that you don't want collision to happen for the next line
        if(cellType != EMPTY && cellType != WATER && cellType != LAVA && cellType != WEED &&
                cellType != REDFLOWER && cellType != YELLOWFLOWER && cellType != SUGARCANE) {
            *out_blockHit = currCell;
            *out_dist = glm::min(maxLen, curr_t);
            return true;
        }
    }
    *out_dist = glm::min(maxLen, curr_t);
    return false;
}

void Player::collisionDetection(glm::vec3 *rayDirection, const Terrain &terrain)
{
    glm::ivec3 out_blockHit = glm::ivec3(0);
    float out_dist = 0.f;
    glm::vec3 corner = this->m_position - glm::vec3(0.5f, 0.f, 0.5f);

    for (int x = 0; x < 2; x++)
    {
           for (int z = 0; z < 2; z++)
           {
               for (int y = 0; y <= 2; y++)
               {
                   glm::vec3 rayOrigin = corner + glm::vec3(x, y, z);
                                     // Loop over the x, y, z axis indices of our velocity
                                     for(int i = 0; i < 3; ++i) {
                                         glm::vec3 tempDir(0.f);
                                         tempDir[i] = (*rayDirection)[i];
                                         if (gridMarch(rayOrigin, tempDir, terrain, &out_dist, &out_blockHit))
                                         {
                                             tempDir = (out_dist - 0.0001f) * glm::normalize(tempDir);
                                            (*rayDirection)[i] = tempDir[i];
                                             //*rayDirection = (out_dist - 0.0001f) * glm::normalize(*rayDirection);
                                         }
                                     }
               }
           }
    }
}

void Player::placeBlock(Terrain * terrain)
{
    glm::vec3 rayOrigin = m_camera.mcr_position;
    glm::vec3 rayDirection =  3.f * glm::normalize(this->m_forward);
    float outDist = 0.f;
    glm::ivec3 outBlockHit = glm::ivec3();
    if (gridMarch(rayOrigin, rayDirection, *terrain, &outDist, &outBlockHit)) {
        if (m_interfaceAxis == 0)
        {
            terrain->setBlockAt(outBlockHit.x - glm::sign(rayDirection.x), outBlockHit.y, outBlockHit.z, STONE);
            terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->destroyVBOdata();
            terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->createVBOdata();
        }
        else if (m_interfaceAxis == 1) {
            terrain->setBlockAt(outBlockHit.x, outBlockHit.y - glm::sign(rayDirection.y), outBlockHit.z , STONE);
            terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->destroyVBOdata();
            terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->createVBOdata();
        }
        else if (m_interfaceAxis == 2) {
            terrain->setBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z - glm::sign(rayDirection.z), STONE);

            terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->destroyVBOdata();
            terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->createVBOdata();
        }
    }
}

void Player::removeBlock(Terrain * terrain)
{
    glm::vec3 rayOrigin = m_camera.mcr_position;
    glm::vec3 rayDirection = 3.f * glm::normalize(this->m_forward);
    float outDist = 0.f;
    glm::ivec3 outBlockHit = glm::ivec3();

    if (gridMarch(rayOrigin, rayDirection, *terrain, &outDist, &outBlockHit)) {
        // BEDROCK sohuld be unbreakable
        if (terrain->getBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z) == BEDROCK) return;

        terrain->setBlockAt(outBlockHit.x, outBlockHit.y, outBlockHit.z, EMPTY);
        terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->destroyVBOdata();
        terrain->getChunkAt(outBlockHit.x, outBlockHit.z).get()->createVBOdata();
    }
}

bool Player::isInWater()
{
    return mcr_terrain.getBlockAt(mcr_camera.mcr_position.x, mcr_camera.mcr_position.y, mcr_camera.mcr_position.z) == WATER;
}

bool Player::isInLava()
{
    return mcr_terrain.getBlockAt(mcr_camera.mcr_position.x, mcr_camera.mcr_position.y, mcr_camera.mcr_position.z) == LAVA;
}

void Player::setCameraWidthHeight(unsigned int w, unsigned int h) {
    m_camera.setWidthHeight(w, h);
}

void Player::moveAlongVector(glm::vec3 dir) {
    Entity::moveAlongVector(dir);
    m_camera.moveAlongVector(dir);
}
void Player::moveForwardLocal(float amount) {
    Entity::moveForwardLocal(amount);
    m_camera.moveForwardLocal(amount);
}
void Player::moveRightLocal(float amount) {
    Entity::moveRightLocal(amount);
    m_camera.moveRightLocal(amount);
}
void Player::moveUpLocal(float amount) {
    Entity::moveUpLocal(amount);
    m_camera.moveUpLocal(amount);
}
void Player::moveForwardGlobal(float amount) {
    Entity::moveForwardGlobal(amount);
    m_camera.moveForwardGlobal(amount);
}
void Player::moveRightGlobal(float amount) {
    Entity::moveRightGlobal(amount);
    m_camera.moveRightGlobal(amount);
}
void Player::moveUpGlobal(float amount) {
    Entity::moveUpGlobal(amount);
    m_camera.moveUpGlobal(amount);
}
void Player::rotateOnForwardLocal(float degrees) {
    Entity::rotateOnForwardLocal(degrees);
    m_camera.rotateOnForwardLocal(degrees);
}
void Player::rotateOnRightLocal(float degrees) {
    if (this->firstPerson)
    {
        // Limit the rotate range
        float rad = glm::radians(degrees);
        glm::mat4 rot(glm::rotate(glm::mat4(), rad, m_right));
        if (abs(glm::vec3(rot * glm::vec4(m_forward, 0.f)).y) > 0.8)
        {
            return;
        }
        Entity::rotateOnRightLocal(degrees);
        m_camera.rotateOnRightLocal(degrees);
    }
    else
    {
        Entity::rotateOnRightLocal(degrees);
        m_camera.rotateOnUpPolar(-totalRot, this->m_position);

        m_camera.rotateOnRightPolar(degrees, this->m_position);
        m_camera.rotateOnUpPolar(totalRot, this->m_position);

    }
}
void Player::rotateOnUpLocal(float degrees) {
    Entity::rotateOnUpLocal(degrees);
    m_camera.rotateOnUpLocal(degrees);
}
void Player::rotateOnForwardGlobal(float degrees) {
    Entity::rotateOnForwardGlobal(degrees);
    m_camera.rotateOnForwardGlobal(degrees);
}
void Player::rotateOnRightGlobal(float degrees) {
    Entity::rotateOnRightGlobal(degrees);
    m_camera.rotateOnRightGlobal(degrees);
}
void Player::rotateOnUpGlobal(float degrees) {
    if (this->firstPerson)
    {
        Entity::rotateOnUpGlobal(degrees);
        m_camera.rotateOnUpGlobal(degrees);
        this->totalRot += degrees;

    }
    else
    {
        Entity::rotateOnUpGlobal(degrees);
        m_camera.rotateOnUpPolar(degrees, this->m_position);
        this->totalRot += degrees;
    }
}

QString Player::posAsQString() const {
    std::string str("( " + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ", " + std::to_string(m_position.z) + ")");
    return QString::fromStdString(str);
}
QString Player::velAsQString() const {
    std::string str("( " + std::to_string(m_velocity.x) + ", " + std::to_string(m_velocity.y) + ", " + std::to_string(m_velocity.z) + ")");
    return QString::fromStdString(str);
}
QString Player::speedAsQString() const {
    std::string str("( " + std::to_string(glm::length(m_velocity)) + ")");
    return QString::fromStdString(str);
}
QString Player::accAsQString() const {
    std::string str("( " + std::to_string(m_acceleration.x) + ", " + std::to_string(m_acceleration.y) + ", " + std::to_string(m_acceleration.z) + ")");
    return QString::fromStdString(str);
}
QString Player::lookAsQString() const {
    std::string str("( " + std::to_string(m_forward.x) + ", " + std::to_string(m_forward.y) + ", " + std::to_string(m_forward.z) + ")");
    return QString::fromStdString(str);
}
QString Player::modeAsQString() const {
    std::string mode;
    if (this->flightModeOn)
    {
        mode = "on";
    }
    else
    {
        mode = "off";
    }
    std::string str("( " + mode + ")");
    return QString::fromStdString(str);
}
QString Player::groundAsQString() const {
    std::string mode;
    if (this->onGround)
    {
        mode = "true";
    }
    else
    {
        mode = "false";
    }
    std::string str("( " + mode + ")");
    return QString::fromStdString(str);
}

void Player::setCameraFov(float fov)
{
    this->m_camera.setFov(fov);
}
