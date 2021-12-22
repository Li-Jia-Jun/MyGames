#include "terrain.h"
#include "mygl.h"
#include "cube.h"
#include "utils/noise.h"
#include "primitive.h"
#include <stdexcept>
#include <iostream>

Terrain::Terrain(OpenGLContext *context, MyGL* gl)
    : m_chunks(), m_generatedTerrain(), m_geomCube(context),
      m_lastPlayerTerrainZone(INT_MAX, INT_MAX),m_tryExpansionTimer(0.f),
      m_chunksWithBlockTypeData(new std::unordered_set<Chunk*>()),
      m_chunksWithNewVBOData(new std::unordered_set<VBOData*>),
      m_chunksWithBlockTypeLock(new QMutex),
      m_chunksWithNewVBODataLock(new QMutex),
      m_chunkLock(new QMutex),
      mp_context(context),
      m_gl(gl),
      footstepSound(new QSound(QDir::currentPath().append(QString(" ../../assignment_package/sound_files/footstep.wav"))))
{
    m_chunks.reserve(20000);
    footstepSound->setLoops(1);
}

Terrain::~Terrain() {
    m_geomCube.destroyVBOdata();
}

// Combine two 32-bit ints into one 64-bit int
// where the upper 32 bits are X and the lower 32 bits are Z
int64_t toKey(int x, int z) {
    int64_t xz = 0xffffffffffffffff;
    int64_t x64 = x;
    int64_t z64 = z;

    // Set all lower 32 bits to 1 so we can & with Z later
    xz = (xz & (x64 << 32)) | 0x00000000ffffffff;

    // Set all upper 32 bits to 1 so we can & with XZ
    z64 = z64 | 0xffffffff00000000;

    // Combine
    xz = xz & z64;
    return xz;
}

glm::ivec2 toCoords(int64_t k) {
    // Z is lower 32 bits
    int64_t z = k & 0x00000000ffffffff;
    // If the most significant bit of Z is 1, then it's a negative number
    // so we have to set all the upper 32 bits to 1.
    // Note the 8    V
    if(z & 0x0000000080000000) {
        z = z | 0xffffffff00000000;
    }
    int64_t x = (k >> 32);

    return glm::ivec2(x, z);
}

// Surround calls to this with try-catch if you don't know whether
// the coordinates at x, y, z have a corresponding Chunk
BlockType Terrain::getBlockAt(int x, int y, int z) const
{
    if(hasChunkAt(x, z)) {
        // Just disallow action below or above min/max height,
        // but don't crash the game over it.
        if(y < 0 || y >= 256) {
            return EMPTY;
        }
        const uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        return c->getBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                             static_cast<unsigned int>(y),
                             static_cast<unsigned int>(z - chunkOrigin.y));
    }
//    else {
//        throw std::out_of_range("Coordinates " + std::to_string(x) +
//                                " " + std::to_string(y) + " " +
//                                std::to_string(z) + " have no Chunk!");
//    }
}

BlockType Terrain::getBlockAt(glm::vec3 p) const {
    return getBlockAt(p.x, p.y, p.z);
}

int Terrain::getChunkSize()
{
    return this->m_chunks.size();
}

bool Terrain::hasChunkAt(int x, int z) const {
    // Map x and z to their nearest Chunk corner
    // By flooring x and z, then multiplying by 16,
    // we clamp (x, z) to its nearest Chunk-space corner,
    // then scale back to a world-space location.
    // Note that floor() lets us handle negative numbers
    // correctly, as floor(-1 / 16.f) gives us -1, as
    // opposed to (int)(-1 / 16.f) giving us 0 (incorrect!).
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.find(toKey(16 * xFloor, 16 * zFloor)) != m_chunks.end();
}

bool Terrain::hasChunkRendered(int x, int z) const {
    // Map x and z to their nearest Chunk corner
    // By flooring x and z, then multiplying by 16,
    // we clamp (x, z) to its nearest Chunk-space corner,
    // then scale back to a world-space location.
    // Note that floor() lets us handle negative numbers
    // correctly, as floor(-1 / 16.f) gives us -1, as
    // opposed to (int)(-1 / 16.f) giving us 0 (incorrect!).
    int xFloor = static_cast<int>(glm::floor(x / 64.f));
    int zFloor = static_cast<int>(glm::floor(z / 64.f));
    return m_terrainToRender.contains(toKey(64 * xFloor, 64 * zFloor));
}


uPtr<Chunk>& Terrain::getChunkAt(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks[toKey(16 * xFloor, 16 * zFloor)];
}


const uPtr<Chunk>& Terrain::getChunkAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.at(toKey(16 * xFloor, 16 * zFloor));
}

void Terrain::setBlockAt(int x, int y, int z, BlockType t)
{
    if(hasChunkAt(x, z)) {
        uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        c->setBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                      static_cast<unsigned int>(y),
                      static_cast<unsigned int>(z - chunkOrigin.y),
                      t);
    }
    else {
//        throw std::out_of_range("Coordinates " + std::to_string(x) +
//                                " " + std::to_string(y) + " " +
//                                std::to_string(z) + " have no Chunk!");
    }
}


glm::ivec2 Terrain::getZonePos(double x, double z) const
{
    int xFloor = static_cast<int>(glm::floor(x / 64.0));
    int zFloor = static_cast<int>(glm::floor(z / 64.0));

    return glm::ivec2(xFloor * 64, zFloor * 64);
}

void Terrain::linkChunkNeighbor(int x, int z)
{
    // Neighbors will update when current chunk blocktypes are all set

    Chunk *cPtr = m_chunks[toKey(x, z)].get();
    if(hasChunkAt(x, z + 16)) {
        auto &chunkNorth = m_chunks[toKey(x, z + 16)];
        cPtr->linkNeighbor(chunkNorth, ZPOS);
    }
    if(hasChunkAt(x, z - 16)) {
        auto &chunkSouth = m_chunks[toKey(x, z - 16)];
        cPtr->linkNeighbor(chunkSouth, ZNEG);
    }
    if(hasChunkAt(x + 16, z)) {
        auto &chunkEast = m_chunks[toKey(x + 16, z)];
        cPtr->linkNeighbor(chunkEast, XPOS);
    }
    if(hasChunkAt(x - 16, z)) {
        auto &chunkWest = m_chunks[toKey(x - 16, z)];
        cPtr->linkNeighbor(chunkWest, XNEG);
    }
}

void Terrain::AddChunkToTerrain(std::unique_ptr<Chunk> chunk)
{
    m_chunkLock->lock();
    m_chunks[toKey(chunk->m_pos[0], chunk->m_pos[1])] = std::move(chunk);
    m_chunkLock->unlock();
}

void Terrain::draw(ShaderProgram *shaderProgram)
{
    for (auto key: m_terrainToRender)
    {
        glm::ivec2 pos = toCoords(key);
        for(int x = pos[0]; x < pos[0] + 64; x += 16)
        {
            for(int z = pos[1]; z < pos[1] + 64; z += 16)
            {
                if(hasChunkAt(x, z))
                {
                    Chunk* chunk = getChunkAt(x, z).get();
                    shaderProgram->drawInterleavedVBO(*chunk);  // Draw opaque blocks
                }
            }
        }
    }
}

void Terrain::drawTransparent(ShaderProgram *shaderProgram)
{
     for (auto key: m_terrainToRender)
    {
        glm::ivec2 pos = toCoords(key);
        for(int x = pos[0]; x < pos[0] + 64; x += 16)
        {
            for(int z = pos[1]; z < pos[1] + 64; z += 16)
            {
                if(hasChunkAt(x, z))
                {
                    Chunk* chunk = getChunkAt(x, z).get();
                    shaderProgram->drawTransparentVBO(*chunk);  // Draw transparent blocks
                }
            }
        }
    }
}

void Terrain::generateBiome(Chunk *chunk, int x, int z)
{
    float biomeScale = 800.f;
    // Temperature range [0, 1]
    float temperature = Noise::fbm(glm::vec2(x, z) / biomeScale, 0.5f, 4, 1.f, 0.5f, Noise::perlin2D) * 0.5f + 0.5f;
    temperature = glm::smoothstep(0.25f, 0.75f, temperature);
    // Moisture range [0, 1]
    float moisture = Noise::fbm(glm::vec2(x, z) / biomeScale + 100.f, 0.5f, 4, 1.f, 0.5f, Noise::perlin2D) * 0.5f + 0.5f;


    // Blend the height generated for a biome
    float t = glm::smoothstep(0.4f, 0.6f, temperature);
    float m = glm::smoothstep(0.4f, 0.6f, moisture);

    float moistureHeight1 = glm::mix(getMountainHeight(x, z), getIslandHeight(x, z), m);
    float moistureHeight2 = glm::mix(getDesertHeight(x, z), getGrasslandHeight(x, z), m);

    float normalizedHeight = glm::mix(moistureHeight1, moistureHeight2, t);

    if (temperature <= 0.5f && moisture <= 0.5f)
    {
        generateMountain(chunk, x, z, normalizedHeight);
    }
    else if (temperature <= 0.5f && moisture <= 1.f)
    {
        generateIsland(chunk, x, z, normalizedHeight);
    }
    else if (temperature <= 1.f && moisture <= 0.5f)
    {
        generateDesert(chunk, x, z, normalizedHeight);
    }
    else if (temperature <= 1.f && moisture <= 1.0f)
    {
        generateGrassland(chunk, x, z, normalizedHeight);
    }

    glm::ivec2 pos = chunk->m_pos;
    chunk->setBlockAt(x - pos[0], maxUndergroundHeight, z - pos[1], STONE);

//    generateUndergroundLayer(chunk, x, z);
	
//    generateCave(chunk, x, z);
}

void Terrain::generateUndergroundLayer(Chunk *chunk, int x, int z)
{
    glm::ivec2 pos = chunk->m_pos;
    chunk->setBlockAt(x - pos[0], minHeight, z - pos[1], BEDROCK);
}

void Terrain::generateCave(Chunk *chunk, int x, int z)
{
    glm::ivec2 pos = chunk->m_pos;

    float sampleFrequency = 15.f;
    for (int y = minHeight + 1; y <= maxUndergroundHeight; ++y)
    {
        float isEmpty = Noise::perlin3D(glm::vec3((float)x, (float)y, (float)z) / sampleFrequency);
        if (isEmpty >= 0.f)
        {
            if (y >= 25) chunk->setBlockAt(x - pos[0], y, z - pos[1], EMPTY);
            else chunk->setBlockAt(x - pos[0], y, z - pos[1], LAVA);;
        }
        else
        {
            float redstoneNoise = Noise::random1(glm::vec3(x, y, z));
            float emeraldNoise = Noise::random1(glm::vec3(x, y, z) + 25.f);
            float diamondNoise = Noise::random1(glm::vec3(x, y, z) + 50.f);

            if (redstoneNoise > 0.996f)
            {
                chunk->setBlockAt(x - pos[0], y, z - pos[1], REDSTONEORE);
            }
            else if (emeraldNoise > 0.996f)
            {
                chunk->setBlockAt(x - pos[0], y, z - pos[1], EMERALDORE);
            }
            else if (diamondNoise > 0.996f)
            {
                chunk->setBlockAt(x - pos[0], y, z - pos[1], DIAMONDORE);
            }
            else
            {
                chunk->setBlockAt(x - pos[0], y, z - pos[1], STONE);
            }

        }
    }
}

void Terrain::generateGrassland(Chunk *chunk, int x, int z, float normalizedHeight)
{
    int heightMap = normalizedHeight * (maxHeight - minSurfaceHeight) + minSurfaceHeight;

    glm::ivec2 pos = chunk->m_pos;
    for (int y = minSurfaceHeight; y <= (heightMap >= maxWaterHeight ? heightMap : maxWaterHeight); ++y)
    {
        if (y < heightMap)
        {
            chunk->setBlockAt(x - pos[0], y, z - pos[1], DIRT);
        }
        else if (y == heightMap)
        {
            chunk->setBlockAt(x - pos[0], y, z - pos[1], GRASS);
        }
        else if (getBlockAt(x, y, z) == EMPTY)
        {
            chunk->setBlockAt(x - pos[0], y, z - pos[1], WATER);
        }
    }

    float treeNoise = Noise::random1(glm::vec2(x, z)) *
            (Noise::fbm(glm::vec2(x, z) / 80.f, 0.5f, 4, 1.f, 0.5f, Noise::perlin2D) * 0.5f + 0.5f);
    float grassNoise = Noise::random1(glm::vec2(x, z) + 25.f);
    float flowerNoise = Noise::random1(glm::vec2(x, z) + 50.f);

    if (treeNoise > 0.999f * 0.52f)
    {
        populateTrees(chunk, x, z, heightMap + 1);
    }
    if (grassNoise > 0.995f)
    {
        populateGrass(chunk, x, z, heightMap + 1);
    }
    if (flowerNoise > 0.997f)
    {
        populateFlowers(chunk, x, z, heightMap + 1);
    }

}

void Terrain::generateMountain(Chunk *chunk, int x, int z, float normalizedHeight)
{
    int heightMap = normalizedHeight * (maxHeight - minSurfaceHeight) + minSurfaceHeight;

    glm::ivec2 pos = chunk->m_pos;
    for (int y = minSurfaceHeight; y <= (heightMap >= maxWaterHeight ? heightMap : maxWaterHeight); ++y)
    {
        if (y < heightMap)
        {
            chunk->setBlockAt(x - pos[0], y, z - pos[1], STONE);
        }
        else if (y == heightMap && y > 200)
        {
            chunk->setBlockAt(x - pos[0], y, z - pos[1], SNOW);
        }
        else if (y > heightMap && getBlockAt(x, y, z) == EMPTY)
        {
            chunk->setBlockAt(x - pos[0], y, z - pos[1], WATER);
        }
    }
}

void Terrain::generateDesert(Chunk *chunk, int x, int z, float normalizedHeight)
{
    int heightMap = normalizedHeight * (maxHeight - minSurfaceHeight) + minSurfaceHeight;

    glm::ivec2 pos = chunk->m_pos;
    for (int y = minSurfaceHeight; y <= heightMap; ++y)
    {
        chunk->setBlockAt(x - pos[0], y, z - pos[1], SAND);
    }

    float cactusNoise = Noise::random1(glm::vec2(x, z));
    if (cactusNoise > 0.9997f)
    {
        populateCacti(chunk, x, z, heightMap + 1);
    }
}

void Terrain::generateIsland(Chunk *chunk, int x, int z, float normalizedHeight)
{
    int heightMap = normalizedHeight * (maxHeight - minSurfaceHeight) + minSurfaceHeight;

    glm::ivec2 pos = chunk->m_pos;
    for (int y = minSurfaceHeight; y <= (heightMap >= maxWaterHeight ? heightMap : maxWaterHeight); ++y)
    {
        if (y < heightMap && y > maxWaterHeight + 3)
        {
            chunk->setBlockAt(x - pos[0], y, z - pos[1], DIRT);
        }
        else if (y < heightMap)
        {
            chunk->setBlockAt(x - pos[0], y, z - pos[1], SAND);
        }
        else if (y == heightMap && y > maxWaterHeight + 3)
        {
            chunk->setBlockAt(x - pos[0], y, z - pos[1], GRASS);
        }
        else if (y == heightMap)
        {
            chunk->setBlockAt(x - pos[0], y, z - pos[1], SAND);
        }
        else if (y > heightMap && getBlockAt(x, y, z) == EMPTY)
        {
            chunk->setBlockAt(x - pos[0], y, z - pos[1], WATER);
        }
    }

    float treeNoise = Noise::random1(glm::vec2(x, z));
    float grassNoise = Noise::random1(glm::vec2(x, z) + 25.f);
    float flowerNoise = Noise::random1(glm::vec2(x, z) + 50.f);
    float sugarcaneNoise = Noise::random1(glm::vec2(x, z) + 75.f);

    if (treeNoise > 0.999f)
    {
        populateTrees(chunk, x, z, heightMap + 1);
    }
    if (grassNoise > 0.995f)
    {
        populateGrass(chunk, x, z, heightMap + 1);
    }
    if (flowerNoise > 0.997f)
    {
        populateFlowers(chunk, x, z, heightMap + 1);
    }
    if (sugarcaneNoise > 0.997f)
    {
        populateSugarcanes(chunk, x, z, heightMap + 1);
    }
}

void Terrain::populateTrees(Chunk *chunk, int x, int z, int rootHeight)
{
    glm::ivec2 pos = chunk->m_pos;
    float randNum = Noise::random1(glm::vec2(x, z) + 100.f);
    PrimitiveType treeType;
    int trunkHeight;
    float heightNoise = Noise::random1(glm::vec2(x, z) + 125.f);
    if (randNum < 0.8f)
    {
        treeType = BALLOONOAK;
        trunkHeight = 7;
        trunkHeight += int(glm::floor(heightNoise * 3.99f));
    }
    else
    {
        treeType = LARGEOAK;
        trunkHeight = 17;
        trunkHeight += int(glm::floor(heightNoise * 4.99f));
    }
    Primitive::placeTree(chunk, treeType, x - pos[0], rootHeight, z - pos[1], trunkHeight);
}

void Terrain::populateFlowers(Chunk *chunk, int x, int z, int y)
{
    glm::ivec2 pos = chunk->m_pos;
    if (chunk->getBlockAt(x - pos[0], y - 1, z - pos[1]) != GRASS) return;
    float randNum = Noise::random1(glm::vec2(x, z) + 100.f);
    BlockType flowerType;
    if (randNum < 0.5f) flowerType = REDFLOWER;
    else flowerType = YELLOWFLOWER;
    if (chunk->getBlockAt(x - pos[0], y, z - pos[1]) == EMPTY) chunk->setBlockAt(x - pos[0], y, z - pos[1], flowerType);
}

void Terrain::populateGrass(Chunk *chunk, int x, int z, int y)
{
    glm::ivec2 pos = chunk->m_pos;
    if (chunk->getBlockAt(x - pos[0], y - 1, z - pos[1]) != GRASS) return;
    if (chunk->getBlockAt(x - pos[0], y, z - pos[1]) == EMPTY) chunk->setBlockAt(x - pos[0], y, z - pos[1], WEED);
}

void Terrain::populateCacti(Chunk *chunk, int x, int z, int y)
{
    glm::ivec2 pos = chunk->m_pos;
    if (chunk->getBlockAt(x - pos[0], y - 1, z - pos[1]) != SAND) return;

    float heightNoise = Noise::random1(glm::vec2(x, z) + 100.f);
    int cactusHeight = glm::floor(heightNoise * 3.f) + 2.f;


    for (int i = 0; i < cactusHeight; i++)
    {
        if (chunk->getBlockAt(x - pos[0], y + i, z - pos[1]) == EMPTY)
            chunk->setBlockAt(x - pos[0], y + i, z - pos[1], CACTUS);
    }
}

void Terrain::populateSugarcanes(Chunk *chunk, int x, int z, int y)
{
    glm::ivec2 pos = chunk->m_pos;
    if (chunk->getBlockAt(x - pos[0], y - 1, z - pos[1]) != SAND ||
        chunk->getBlockAt(x - pos[0], y, z - pos[1]) != EMPTY) return;

    float heightNoise = Noise::random1(glm::vec2(x, z) + 100.f);
    int cactusHeight = glm::floor(heightNoise * 3.f) + 2.f;


    for (int i = 0; i < cactusHeight; i++)
    {
        if (chunk->getBlockAt(x - pos[0], y + i, z - pos[1]) == EMPTY)
            chunk->setBlockAt(x - pos[0], y + i, z - pos[1], SUGARCANE);
    }
}

float Terrain::getGrasslandHeight(int x, int z)
{
    float sampleFrequency = 700.f;
    float noiseHeight = 1.f - Noise::warpedWorley(glm::vec2((float)x, (float)z) / sampleFrequency, 0.3f);
    noiseHeight = noiseHeight * 0.25f;
    return noiseHeight;
}

float Terrain::getMountainHeight(int x, int z)
{
    float sampleFrequency = 60.f;
    float noiseHeight = Noise::HybridMultiFractal(glm::vec2((float)x, (float)z) / sampleFrequency,
                                                  0.25f, 1.f, 6.5f, -0.12f);
    noiseHeight = (noiseHeight - 0.65f) * 0.7f + 0.65f;
    return glm::min(noiseHeight, 1.f);
}

float Terrain::getDesertHeight(int x, int z)
{
    float noiseHeight = 11.f / 128.f;
    return noiseHeight;
}

float Terrain::getIslandHeight(int x, int z)
{
    float sampleFrequency = 100.f;
    float noiseHeight = Noise::fbm(glm::vec2((float)x, (float)z) / sampleFrequency, 0.5f, 4, 1.f, 0.5f, Noise::perlin2D) * 0.5f + 0.5f;
    noiseHeight = glm::smoothstep(0.45f, 0.55f, noiseHeight) * 0.10f;
    return noiseHeight;
}

// The main function for multithreaded terrain generation
void Terrain::multithreadedWork(glm::vec3 playerPos, glm::vec3 playerPosPrev, float dT)
{
    m_tryExpansionTimer += dT;
    // Only check for terrain expansion every second of real time or so
    if (m_tryExpansionTimer < 0.5f)
    {
        if (playerPos != playerPosPrev && this->m_gl->playerOnGround() && footstepSound->isFinished())
        {
            footstepSound->play();
        }
        return;
    }
    tryExpansion(playerPos, playerPosPrev);
    checkThreadResults();
    m_tryExpansionTimer = 0.f;
}

// Try to expand the terrain, and prepare the needed data
void Terrain::tryExpansion(glm::vec3 playerPos, glm::vec3 playerPosPrev)
{
    // Find the player's position relative to their current terrain zone
    glm::ivec2 currZone = getZonePos(playerPos.x, playerPos.z);
    glm::ivec2 prevZone = getZonePos(playerPosPrev.x, playerPosPrev.z);

    // Determine which terrain zones border our current position and our previous position
    QSet<int64_t> terrainZonesBorderingCurrPos = terrainZonesBorderingZone(currZone, 2*N);
    QSet<int64_t> terrainZonesBorderingPrevPos = terrainZonesBorderingZone(prevZone, 2*N);

    m_terrainToRender = terrainZonesBorderingZone(currZone, N);


    // Check which terrain zones need to be destroy()ed
    // by determining which terrain zones were previously in our radius and are now not
    for (auto id : terrainZonesBorderingPrevPos)
    {
        if (!terrainZonesBorderingCurrPos.contains(id))
        {
            glm::ivec2 coord = toCoords(id);
            for (int x=coord.x; x<coord.x+64;x+=16)
            {
                for (int z = coord.y; z < coord.y + 64; z+= 16)
                {
                    if(m_chunks.find(toKey(x, z)) != m_chunks.end())
                    {
                        auto &chunk = getChunkAt(x, z);
                        chunk->destroyVBOdata();
                    }
                }
            }
        }
    }

    // Check which terrain zones need VBOData
    // Send VBOData to VBOWorkers
    // If the chunks do not exist, send to BlockTypeWorker instead
    for (auto id : terrainZonesBorderingCurrPos)
    {
        if (terrainZoneExists(id))
        {
            glm::ivec2 coord = toCoords(id);

            // Send VBO data to render only
            if (!terrainZonesBorderingPrevPos.contains(id))
            {              
                for (int x=coord.x; x<coord.x+64;x+=16)
                {
                    for (int z = coord.y; z < coord.y + 64; z+= 16)
                    {
                        if(m_chunks.find(toKey(x, z)) != m_chunks.end())
                        {
                            auto &chunk = getChunkAt(x, z);
                            spawnVBOWorker(chunk.get());
                        }
                    }
                }
            }
            else
            {                
                // For exsiting chunks in zone, check if update VBO is needed
                for (int x=coord.x; x<coord.x+64;x+=16)
                {
                    for (int z = coord.y; z < coord.y + 64; z+= 16)
                    {
                        if(m_chunks.find(toKey(x, z)) != m_chunks.end())
                        {
                            auto &chunk = getChunkAt(x, z);
                            if(chunk->vboDataNeedUpdate)
                            {
                                chunk->vboDataNeedUpdate = false;
                                chunk->destroyVBOdata();
                                spawnVBOWorker(chunk.get());
                            }
                        }
                    }
                }
            }
        }
        // Both set block data and send VBO data to render
        else
        {
            spawnBlockTypeWorker(id);

            // Randomly placing birds at each zone
            vec2 pos = toCoords(id);
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

                if (currentRandomNumber > 0.8f)
                {
                     glm::vec3 birdPos = glm::vec3(pos.x, 185.f + (currentRandomNumber - 0.5f)* 10.f, pos.y);
                     double newRandomNumber = getRandom(rng);

                     if (newRandomNumber < 0.2)
                     {
                     this->m_gl->addObj(QDir::currentPath().append(QString("../../assignment_package/obj_files/bird5.obj")),
                                       birdPos, 11, 4, 2.f, newRandomNumber * 100.f);
                     }
                     else if(newRandomNumber < 0.4)
                     {
                         this->m_gl->addObj(QDir::currentPath().append(QString("../../assignment_package/obj_files/bird6.obj")),
                                           birdPos, 11, 4, 2.f, newRandomNumber * 100.f);
                     }
                     else if(newRandomNumber < 0.6)
                     {
                         this->m_gl->addObj(QDir::currentPath().append(QString("../../assignment_package/obj_files/bird1.obj")),
                                           birdPos, 11, 4, 2.f, newRandomNumber * 100.f);
                     }
                     else if(newRandomNumber < 0.8)
                     {
                         this->m_gl->addObj(QDir::currentPath().append(QString("../../assignment_package/obj_files/bird2.obj")),
                                           birdPos, 11, 4, 2.f, newRandomNumber * 100.f);
                     }
                     else
                     {
                         this->m_gl->addObj(QDir::currentPath().append(QString("../../assignment_package/obj_files/bird3.obj")),
                                           birdPos, 11, 4, 2.f, newRandomNumber * 100.f);
                     }
                }
        }
    }
}

void Terrain::spawnBlockTypeWorker(int64_t zoneToGenerate)
{
    m_generatedTerrain.insert(zoneToGenerate);
    glm::ivec2 coords = toCoords(zoneToGenerate);

    BlockTypeWorker *worker = new BlockTypeWorker(coords.x, coords.y,
                                                   m_chunksWithBlockTypeData, m_chunksWithBlockTypeLock, this);
    QThreadPool::globalInstance()->start(worker);
}

// Generate more height
void Terrain::spawnBlockTypeWorker(const QSet<int64_t> &zonesToGenerate)
{
    // Spawn worker threads to generate more Chunks
    for (int64_t zone: zonesToGenerate)
    {       
        spawnBlockTypeWorker(zone);
    }   
}

void Terrain::spawnVBOWorker(Chunk * chunk)
{
    VBOWorker *worker = new VBOWorker(chunk, m_chunksWithNewVBOData, m_chunksWithNewVBODataLock);
    QThreadPool::globalInstance()->start(worker);
}

// return the set of terrain zones
QSet<int64_t> Terrain::terrainZonesBorderingZone(glm::ivec2 zone, float radius)
{
    QSet<int64_t> borderingZone;
    // Compute and append all the (2N+1)x(2N+1) keys
    for (int x=-radius; x<=radius; x++)
    {
        for (int z=-radius; z<=radius; z++)
        {
            int64_t key = toKey(zone.x + 64 * x, zone.y + 64 * z);
            borderingZone.insert(key);
        }
    }
    return borderingZone;
}

// Check whether a terrain zone is already created
bool Terrain::terrainZoneExists(int64_t id)
{
    return m_generatedTerrain.find(id) != m_generatedTerrain.end();
}

// 1) Query the set<Chunk*> that is written to by every BlockTypeWorker
//    Iterate over each Chunk* in the set and pass it to a VBOWorker
//    Then, clear the set since we have processed its contents
// 2) Query the set<ChunkVBOdata> written to by the VBOWorkers
void Terrain::checkThreadResults()
{
    this->m_chunksWithBlockTypeLock->lock();
    for (auto c: *m_chunksWithBlockTypeData)
    {
        // CreateVBO workers to deal with the chunks with only BlockType data
        //this->m_chunksWithBlockTypeLock->lock();
        spawnVBOWorker(c);
        //this->m_chunksWithBlockTypeLock->unlock();
    }

    // Clear the processed data
    m_chunksWithBlockTypeData->clear();
    this->m_chunksWithBlockTypeLock->unlock();

    this->m_chunksWithNewVBODataLock->lock();
    for (auto v: *m_chunksWithNewVBOData)
    {
        // Send the VBOData bundles
        v->associated_chunk->sendVBOdata(v);
    }
    // Clear the processed data
    m_chunksWithNewVBOData->clear();
    this->m_chunksWithNewVBODataLock->unlock();
}
