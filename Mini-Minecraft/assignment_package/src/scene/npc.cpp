#include "npc.h"
#include "player.h"


NPC::NPC(glm::vec3 pos, const Terrain &terrain, Player* player, int t, int type, OpenGLContext* context)
    : Entity(pos, false),  m_velocity(0,0,0), m_acceleration(0,0,0),
      m_prevPos(pos), type((ObjType)type), rot(0.f), bb(),
      justTurned(false), oid(setID()), tid(t),
      mcr_terrain(terrain), m_player(player), onGround(false),
      m_loader(new ModelLoader(context)),  m_context(context)
{}

NPC::~NPC()
{}

glm::vec3 NPC::getTranslation()
{
    return this->m_position - m_prevPos;
}

int NPC::getTid()
{
    return this->tid;
}


void NPC::tickNPC(float dT)
{
    if (type == LIFE_TREE)
    {
        this->m_position = glm::vec3(119.f, 105.f, 126.f);
        return;
    }
    computePhysics(dT, mcr_terrain);
}

void NPC::computePhysics(float dT, const Terrain &terrain)
{
    // Shoot a ray downwards to test whether player is on ground
    glm::vec3 downwards = glm::vec3(0.f,-1.f,0.f);
    this->setOnGround(&downwards, terrain);
    if (onGround)
    {
        // Static assets stop moving once on ground
        if (type == STATIC_ASSET)
        {
            return;
        }
    }
    else
    {
        glm::vec3 gravity = glm::vec3(0.f, -9.81f, 0.f);  // gravity is 9.81 m/s^2 pointing downwards
        this->m_velocity += gravity  * dT;
    }
    if (type == ME || type==ME_ON_DEER)
    {
        this->m_position = this->m_player->mcr_position;
        this->m_position.z += 1;
        float newRot = this->m_player->totalRot - this->rot;
        this->rotateOnUpGlobal(newRot);
        return;
    }
        if (type == ZOMBIE)
        {
           // Set up RNG
           std::mt19937_64 rng;
          // initialize the random number generator with time-dependent seed
          uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
          std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
          rng.seed(ss);
          // initialize a uniform distribution between 0 and 1
          std::uniform_real_distribution<double> getRandom(0, 1);
          // ready to generate random numbers

                double currentRandomNumber = getRandom(rng);
                if (onGround && currentRandomNumber > 0.5)
                {
                    double newRand = getRandom(rng);
                    if (newRand > 0.5)
                    {
                        this->rotateOnUpGlobal(30.f);
                    }
                    else
                    {
                        this->rotateOnUpGlobal(-30.f);
                    }
                }
                // Make them move slower
                m_velocity.x = this->m_forward.x * 0.8f;
                m_velocity.z = this->m_forward.z * 0.8f;
                if (onGround)
                {
                    m_velocity.y = 6.f;
                }
        }
        else if (type ==ROBOT)
        {
            if (onGround)
            {
                m_velocity = glm::normalize(glm::vec3(this->m_forward.x, 0.f, this->m_forward.z));
            }
        }
        else if (type ==BIRD)
        {
        // Set up RNG
           std::mt19937_64 rng;
          // initialize the random number generator with time-dependent seed
          uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
          std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
          rng.seed(ss);
          // initialize a uniform distribution between 0 and 1
          std::uniform_real_distribution<double> getRandom(0, 1);
          // ready to generate random numbers

                double currentRandomNumber = getRandom(rng);
                if (currentRandomNumber > 0.7)
                {
                    double newRand = getRandom(rng);
                    if (newRand > 0.4)
                    {
                        this->rotateOnUpGlobal(5.f);
                    }
                    else
                    {
                        this->rotateOnUpGlobal(-5.f);
                    }
                }

                m_velocity.x = this->m_forward.x;
                m_velocity.z = this->m_forward.z;
                m_velocity.y = 0.f;

                double newRandomNumber = getRandom(rng);

                if (newRandomNumber > 0.6f)
                {
                    double newRand = getRandom(rng);
                    if (newRand > 0.55f)
                    {
                        m_velocity.y = this->m_forward.y * 4.f;
                    }
                    else
                    {
                        m_velocity.y = -this->m_forward.y * 4.f;
                    }
                }
                glm::vec3 direction = m_velocity * dT;
                this->moveAlongVector(direction);
            return;
        }
        else if (type ==HUMAN)
        {
            // Set up RNG
               std::mt19937_64 rng;
              // initialize the random number generator with time-dependent seed
              uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
              std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
              rng.seed(ss);
              // initialize a uniform distribution between 0 and 1
              std::uniform_real_distribution<double> getRandom(0, 1);
              // ready to generate random numbers
             double currentRandomNumber = getRandom(rng);
            if (onGround && currentRandomNumber > 0.99)
            {
                double newRand = getRandom(rng);
                if (newRand > 0.5)
                {
                    this->rotateOnUpGlobal(30.f);
                }
                else
                {
                    this->rotateOnUpGlobal(-30.f);
                }
            }
            if (onGround)
            {
                m_velocity = glm::normalize(glm::vec3(this->m_forward.x * 1.5f, 0.f, this->m_forward.z * 1.5f));
            }
        }
        else if (type == EAGLE)
        {
            this->rotateOnUpGlobal(0.06f);
            m_velocity.x = this->m_forward.x;
            m_velocity.z = this->m_forward.z;
            m_velocity.y = 0.05f;
            glm::vec3 direction = 18.f * m_velocity * dT;
            this->moveAlongVector(direction);
            return;
        }
        else if (type == DEER)
        {
            if (m_player->onDeer)
            {
                this->m_position = m_player->mcr_position - glm::vec3(1.f, 0.f, 0.f);
                return;
            }
            if (onGround)
            {
                // Set up RNG
                   std::mt19937_64 rng;
                  // initialize the random number generator with time-dependent seed
                  uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
                  std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
                  rng.seed(ss);
                  // initialize a uniform distribution between 0 and 1
                  std::uniform_real_distribution<double> getRandom(0, 1);
                  // ready to generate random numbers

                        double currentRandomNumber = getRandom(rng);
                        if (currentRandomNumber > 0.9)
                        {
                            double newRand = getRandom(rng);
                            if (newRand > 0.5)
                            {
                                this->rotateOnUpGlobal(3.f);
                            }
                            else
                            {
                                this->rotateOnUpGlobal(-3.f);
                            }
                        }
                m_velocity = glm::normalize(glm::vec3(this->m_forward.x, 0.f, this->m_forward.z));
            }
        }
        else if (type == RABBIT)
        {
            // Set up RNG
            std::mt19937_64 rng;
           // initialize the random number generator with time-dependent seed
           uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
           std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
           rng.seed(ss);
           // initialize a uniform distribution between 0 and 1
           std::uniform_real_distribution<double> getRandom(0, 1);
           // ready to generate random numbers

                 double currentRandomNumber = getRandom(rng);
                 if (onGround && currentRandomNumber > 0.5)
                 {
                     double newRand = getRandom(rng);
                     if (newRand > 0.5)
                     {
                         this->rotateOnUpGlobal(30.f);
                     }
                     else
                     {
                         this->rotateOnUpGlobal(-30.f);
                     }
                 }
                 // Make them move slower
                 m_velocity.x = this->m_forward.x ;
                 m_velocity.z = this->m_forward.z ;
                 if (onGround)
                 {
                     m_velocity.y = 4.f;
                 }
        }
        else if (type == SNAKE)
        {
            if (onGround)
            {
                m_velocity = glm::normalize(glm::vec3(this->m_forward.x, 0.f, this->m_forward.z));
            }
        }
        else if (type == PIGGY)
                {
                    float angle = glm::degrees(glm::orientedAngle(
                                             glm::normalize(glm::vec2(m_player->mcr_position.x - this->m_position.x , m_player->mcr_position.z - this->m_position.z )),
                                                   glm::normalize(glm::vec2(this->m_forward.x,this->m_forward.z))));
                    this->rotateOnUpGlobal(0.01f * angle);
                    m_velocity = 1.5f * glm::normalize(m_player->mcr_position - glm::vec3(-1.f, -1.f, -1.f) -  this->m_position);
                }


    // Move the object
    glm::vec3 direction = m_velocity * dT;
    collisionDetection(&direction, terrain);
    this->moveAlongVector(direction);

}

glm::vec3 NPC::getPos()
{
    return this->m_position;
}

float NPC::getRot()
{
    return this->rot;
}

int NPC::getType()
{
    return this->type;
}

// Load the v/f/vt/vn info from the .obj file
void NPC::loadModel(const QString& fileName, float scale)
{
    // Load the file
      QFile File(fileName);
      std::vector<glm::vec3> p;   // pos of each vertex
      std::vector<std::vector<int>> f;  // vert in this face
      std::vector<std::vector<int>> fn;  // vert in this face
      std::vector<std::vector<int>> ft;  // vert in this face
      std::vector<glm::vec3> n; // normal
      std::vector<glm::vec2> t; // textures


      float maxX=0.f, maxY=0.f, maxZ=0.f, minX=100.f, minY=100.f, minZ=100.f;

      if (!File.open(QIODevice::ReadOnly | QIODevice::Text))
             return;
      QTextStream FileStream(&File);

      // Go through each line in the file
         while (!FileStream.atEnd()) {
             QString line = FileStream.readLine();
             // Load the vertices
             if (line.left(2) == QString("v "))
             {
                 QString xString = line.section(" ", 1, 1);
                 QString yString = line.section(" ", 2, 2);
                 QString zString = line.section(" ", 3, 3);
                 float x = xString.toFloat();
                 float y = yString.toFloat();
                 float z = -zString.toFloat();      // models from MagicaVoxels have different handness
                 p.push_back(glm::vec3(x,y,z));


                 if (x>maxX)
                 {
                     maxX = x;
                 }
                 if (y>maxY)
                 {
                     maxY = y;
                 }
                 if (z>maxZ)
                 {
                     maxZ = z;
                 }
                 if (x<minX)
                 {
                     minX = x;
                 }
                 if (y<minY)
                 {
                     minY = y;
                 }
                 if (z<minZ)
                 {
                     minZ = z;
                 }
             }
         // Load the faces
         if (line.left(2) == QString("f "))
         {
             QStringList v = line.split(QLatin1Char(' '));

              std::vector<int> vInThisLine;
              std::vector<int> nInThisLine;
              std::vector<int> tInThisLine;


             for (int i=1; i<v.size();i++)
             {
                  vInThisLine.push_back(v[i].section('/', 0, 0).toInt());
                  tInThisLine.push_back(v[i].section('/', 1, 1).toInt());
                  nInThisLine.push_back(v[i].section('/', 2, 2).toInt());
             }

            f.push_back(vInThisLine);
            ft.push_back(tInThisLine);
            fn.push_back(nInThisLine);

         }
         // Load the normals
         if (line.left(3) == QString("vn "))
         {
             QString xString = line.section(" ", 1, 1);
             QString yString = line.section(" ", 2, 2);
             QString zString = line.section(" ", 3, 3);
             float x = xString.toFloat();
             float y = yString.toFloat();
             float z = zString.toFloat();
             n.push_back(glm::vec3(x,y,z));
         }
         // Load the textures
         if (line.left(3) == QString("vt "))
         {
             QString uString = line.section(" ", 1, 1);
             QString vString = line.section(" ", 2, 2);
             float u = uString.toFloat();
             float v = vString.toFloat();
             t.push_back(glm::vec2(u,v));
         }
     }

      // Set the bounding box
      bb.topLeftFront = glm::vec3(minX, maxY, minZ);
      bb.topLeftBack = glm::vec3(minX, maxY, maxZ);
      bb.topRightFront = glm::vec3(maxX, maxY, minZ);
      bb.topRightBack = glm::vec3(maxX, maxY, maxZ);
      bb.bottomLeftFront = glm::vec3(minX, minY, minZ);
      bb.bottomLeftBack = glm::vec3(minX, minY, maxZ);
      bb.bottomRightFront = glm::vec3(maxX, minY, minZ);
      bb.bottomRightBack = glm::vec3(maxX, minY, maxZ);

      // appply scale to all the points
      for (unsigned int i=0; i<p.size();i++)
      {
          p.at(i) *= scale;
      }
      bb.topLeftFront *= scale;
      bb.topLeftBack *= scale;
      bb.topRightFront *= scale;
      bb.topRightBack *= scale;
      bb.bottomLeftFront *= scale;
      bb.bottomLeftBack *= scale;
      bb.bottomRightFront *= scale;
      bb.bottomRightBack *= scale;

      if (type == ME || type == HUMAN)
      {
          // To compute mid line
          joint1 = bb.bottomRightBack.x;
          joint2 = bb.bottomLeftBack.x;

         // To compute legs line and arm line
          joint3 = bb.topLeftFront.y;
          joint4 = bb.bottomLeftFront.y;
      }
      else if (type == ROBOT)
      {
          // middle line
          joint1 = (bb.bottomLeftBack + bb.bottomRightBack).x / 2.f;
         // legs line
          joint2 = (bb.topLeftFront - bb.bottomLeftFront).y * 0.4f;
      }
      else if (type ==BIRD)
      {
          joint1 = (bb.bottomLeftBack + bb.bottomRightBack).x / 2.f;
          // mid1
          joint2 = (bb.bottomLeftBack + bb.bottomRightBack).x / 2.f - 0.4f;
          // mid2
          joint3 = (bb.bottomLeftBack + bb.bottomRightBack).x / 2.f + 0.4f;
      }
      else if (type == EAGLE)
       {
          joint1 = bb.bottomLeftBack.x;
          joint2 =bb.bottomRightBack.x;
          joint3 = bb.topLeftFront.y;
          joint4 = bb.bottomLeftFront.y;
      }
      else if(type == DEER || type == ME_ON_DEER)
      {
          joint1 = bb.bottomLeftBack.z;
          joint2 = bb.bottomLeftFront.z;
          joint3 = bb.topLeftFront.y;
          joint4 = bb.bottomLeftFront.y;
      }
      else if (type == RABBIT)
      {
          joint1 = bb.bottomLeftFront.y;
          joint2 = bb.topLeftFront.y;
      }
      else if (type == SNAKE)
      {
          joint1 = bb.bottomLeftFront.z;
          joint2 = bb.bottomLeftBack.z;
      }
      else if (type == PIGGY)
      {
          // X center
            joint1 = (bb.topLeftFront + bb.topRightFront).x / 2.f;
            // Y center
            joint2 = (bb.topLeftBack + bb.bottomLeftBack).y /2.f;
            // Z center
            joint3 = (bb.topLeftBack + bb.topLeftFront).z /2.f;
      }

    m_loader->setData(p, f, n, fn, t, ft, this->m_position, this->oid, this->type);
    m_loader->createVBOdata();

}

bool NPC::gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain, float *out_dist, glm::ivec3 *out_blockHit) {
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
                cellType != REDFLOWER && cellType != YELLOWFLOWER) {
            *out_blockHit = currCell;
            *out_dist = glm::min(maxLen, curr_t);
            return true;
        }
    }
    *out_dist = glm::min(maxLen, curr_t);
    if (onGround)
    {
        justTurned = false;
    }
    return false;
}

void NPC::collisionDetection(glm::vec3 *rayDirection, const Terrain &terrain)
{
    glm::ivec3 out_blockHit = glm::ivec3(0);
    glm::vec3 originalRayDirection = *rayDirection;
    float out_dist = 0.f;
    glm::vec3 boundingBox[4] = {
                              this->m_position + this->bb.bottomLeftBack,
//                              this->m_position + this->bb.bottomLeftFront,
                              this->m_position + this->bb.bottomRightBack,
//                              this->m_position + this->bb.bottomRightFront,
                                this->m_position + this->bb.topLeftBack,
//                                this->m_position + this->bb.topLeftFront,
                                this->m_position + this->bb.topRightBack,
//                                this->m_position + this->bb.topRightFront
                             };
    for (int i = 0; i < 4; ++i)
    {
        glm::vec3 rayOrigin = boundingBox[i];

         // Loop over the x, y, z axis indices of our velocity
        for(int j = 0; j < 3; ++j) {
        glm::vec3 tempDir(0.f);
        tempDir[j] = (*rayDirection)[j];
        if (gridMarch(rayOrigin, tempDir, terrain, &out_dist, &out_blockHit))
         {
             tempDir = (out_dist - 0.001f) * glm::normalize(tempDir);
            (*rayDirection)[j] = tempDir[j];
            *rayDirection = (out_dist - 0.001f) * glm::normalize(*rayDirection);
            if (j!=1)
            {
                if (this->type == HUMAN)
                {
                   // Set up RNG
                       std::mt19937_64 rng;
                      // initialize the random number generator with time-dependent seed
                      uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
                      std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
                      rng.seed(ss);
                      // initialize a uniform distribution between 0 and 1
                      std::uniform_real_distribution<double> getRandom(0, 1);
                      // ready to generate random numbers

                            double currentRandomNumber = getRandom(rng);
                                if (currentRandomNumber > 0.6f && onGround)
                                {
                                    originalRayDirection.y += 1.2;
                                    *rayDirection = originalRayDirection;
                                    justTurned = true;
                                }
                                else
                                {
                                    if (!justTurned)
                                    {
                                        this->rotateOnUpGlobal(180.f);
                                        *rayDirection = -*rayDirection;
                                        justTurned = true;
                                    }
                                }
                                return;
                }
                else
                {
                    if (!justTurned)
                    {
                        this->rotateOnUpGlobal(180.f);
                        *rayDirection = -*rayDirection;
                        justTurned = true;
                    }
                }

            }
         }

        }
    }
}

void NPC::setOnGround(glm::vec3 *rayDirection, const Terrain &terrain)
{
    glm::ivec3 out_blockHit = glm::ivec3(0);
    float out_dist = 0.f;

    glm::vec3 bottomBox[4] = {
                              this->m_position + this->bb.bottomLeftBack,
                              this->m_position + this->bb.bottomLeftFront,
                              this->m_position + this->bb.bottomRightBack,
                              this->m_position + this->bb.bottomRightFront
                             };
    for (int i = 0; i < 4; ++i)
    {
                    glm::vec3 rayOrigin = bottomBox[i];
                    if (gridMarch(rayOrigin, *rayDirection, terrain, &out_dist, &out_blockHit)&& (out_dist < 0.01f))
                    {
                        this->onGround = true;
                        return;
                    }
    }
        this->onGround =  false;
}

void NPC::tick(float dT, InputBundle& inputs)
{
    tickNPC(dT);
}

void NPC::rotateOnUpGlobal(float degrees)
{

    Entity::rotateOnUpGlobal(degrees);
    rot += degrees;
}
