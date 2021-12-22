#include "terrain.h"
#include "cube.h"
#include "utils/noise.h"
#include <stdexcept>
#include <iostream>

Terrain::Terrain(OpenGLContext *context)
    : m_chunks(), m_generatedTerrain(), m_geomCube(context),
      m_lastPlayerTerrainZone(INT_MAX, INT_MAX), m_tryExpansionTimer(0.f),
      mp_context(context)
{}

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
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

BlockType Terrain::getBlockAt(glm::vec3 p) const {
    return getBlockAt(p.x, p.y, p.z);
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
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}


glm::ivec2 Terrain::getZonePos(double x, double z) const
{
    int xFloor = static_cast<int>(glm::floor(x / 64.0));
    int zFloor = static_cast<int>(glm::floor(z / 64.0));

    return glm::ivec2(xFloor * 64, zFloor * 64);
}

Chunk* Terrain::createChunkAt(int x, int z)
{
    uPtr<Chunk> chunk = mkU<Chunk>(mp_context);
    Chunk *cPtr = chunk.get();
    cPtr->setChunkPos(glm::ivec2(x, z));
    m_chunks[toKey(x, z)] = move(chunk);

    return cPtr;
}
void Terrain::linkChunkNeighbor(int x, int z)
{
    Chunk *cPtr = m_chunks[toKey(x, z)].get();

    if(hasChunkAt(x, z + 16)) {
        auto &chunkNorth = m_chunks[toKey(x, z + 16)];
        cPtr->linkNeighbor(chunkNorth, ZPOS);
        chunkNorth->destroyVBOdata();
        chunkNorth->createVBOdata();
    }
    if(hasChunkAt(x, z - 16)) {
        auto &chunkSouth = m_chunks[toKey(x, z - 16)];
        cPtr->linkNeighbor(chunkSouth, ZNEG);
        chunkSouth->destroyVBOdata();
        chunkSouth->createVBOdata();
    }
    if(hasChunkAt(x + 16, z)) {
        auto &chunkEast = m_chunks[toKey(x + 16, z)];
        cPtr->linkNeighbor(chunkEast, XPOS);
        chunkEast->destroyVBOdata();
        chunkEast->createVBOdata();
    }
    if(hasChunkAt(x - 16, z)) {
        auto &chunkWest = m_chunks[toKey(x - 16, z)];
        cPtr->linkNeighbor(chunkWest, XNEG);
        chunkWest->destroyVBOdata();
        chunkWest->createVBOdata();
    }
}

void Terrain::draw(ShaderProgram *shaderProgram)
{
    for(auto key : m_generatedTerrain)
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
    for(auto key : m_generatedTerrain)
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

void Terrain::updateTerrain(glm::vec3 playerPos)
{
    // 1. Check if terrain needs to update(when player moves cross zone border)
    glm::ivec2 currZone = getZonePos(playerPos[0], playerPos[2]);
    if(currZone == m_lastPlayerTerrainZone)
    {
        return;
    }
    m_lastPlayerTerrainZone = currZone;

    // 2. Store (2N+1)x(2N+1) terrain zone keys around player
    m_generatedTerrain.clear();
    for(int x = currZone[0] - 64 * N; x <= currZone[0] + 64 * N; x+=64)
    {
        for(int z = currZone[1] - 64 * N; z <= currZone[1] + 64 * N; z+=64)
        {
            m_generatedTerrain.insert(toKey(x,z));
        }
    }

    // 3. Make sure 4x4 chunks within each terrain zone are created
    for(auto key : m_generatedTerrain)
    {
        glm::ivec2 pos = toCoords(key);
        for(int x = pos[0]; x < pos[0] + 64; x += 16)
        {
            for(int z = pos[1]; z < pos[1] + 64; z += 16)
            {
                if(!hasChunkAt(x, z))
                {
                    // Store new generated chunk
                    Chunk *chunk = createChunkAt(x, z);
                    //chunk->TempCreatePlaneShape();
                    for(int newX = x; newX < x + 16; ++newX)
                    {
                        for(int newZ = z; newZ < z + 16; ++newZ)
                        {
                            generateBiome(newX, newZ);
                        }
                    }

                    linkChunkNeighbor(x, z);
                    chunk->createVBOdata();
                }
            }
        }
    }
}

void Terrain::generateBiome(int x, int z)
{
    float biomeScale = 255.f;

//    float moisture;
    // Temperature range [0, 1]
    float temperature = (Noise::perlin2D(glm::vec2(x / biomeScale, z / biomeScale)) + 1.f) / 2.f;
//    float temperature = Noise::voronoiMap(glm::vec2(x / biomeScale, z / biomeScale));
//    float temperature = Noise::warpedWorley(glm::vec2(x / biomeScale, z / biomeScale), 0.3f);


    float normalizedHeight = 0.f;

//    if (temperature <= 0.5f)
//    {
//        normalizedHeight = getMountainHeight(x, z);
//    }
//    else if (temperature <= 1.f)
//    {
//        normalizedHeight = getGrasslandHeight(x, z);
//    }

    // Blend the height generated for a biome

    float t = glm::smoothstep(0.4f, 0.6f, temperature);
    normalizedHeight = glm::mix(getMountainHeight(x, z), getGrasslandHeight(x, z), t);


    generateUndergroundLayer(x, z);

    if (temperature <= 0.5f)
    {
        generateMountain(x, z, normalizedHeight);
    }
    else if (temperature <= 1.f)
    {
        generateGrassland(x, z, normalizedHeight);
    }




}

void Terrain::generateUndergroundLayer(int x, int z)
{
    for (int y = minHeight; y <= maxUndergroundHeight; y++)
    {
        setBlockAt(x, y, z, STONE);
    }

}

void Terrain::generateGrassland(int x, int z, float normalizedHeight)
{
    int heightMap = normalizedHeight * (maxHeight - minSurfaceHeight) + minSurfaceHeight;
    for (int y = minSurfaceHeight; y <= (heightMap >= maxWaterHeight ? heightMap : maxWaterHeight); ++y)
    {
        if (y < heightMap) setBlockAt(x, y, z, DIRT);
        else if (y == heightMap) setBlockAt(x, y, z, GRASS);
        else if (getBlockAt(x, y, z) == EMPTY) setBlockAt(x, y, z, WATER);
    }

}

void Terrain::generateMountain(int x, int z, float normalizedHeight)
{
    int heightMap = normalizedHeight * (maxHeight - minSurfaceHeight) + minSurfaceHeight;
    for (int y = minSurfaceHeight; y <= (heightMap >= maxWaterHeight ? heightMap : maxWaterHeight); ++y)
    {
        if (y < heightMap) setBlockAt(x, y, z, STONE);
        else if (y == heightMap && y > 200) setBlockAt(x, y, z, SNOW);
        else if (y > heightMap && getBlockAt(x, y, z) == EMPTY) setBlockAt(x, y, z, WATER);
    }
}

float Terrain::getGrasslandHeight(int x, int z)
{
    float biomeScale = 255.f;
    float noiseHeight = 1.f - Noise::warpedWorley(glm::vec2(float(x) / biomeScale, float(z) / biomeScale), 0.3f);
    noiseHeight = glm::pow(Noise::fbm(noiseHeight, 0.45f, 4, 1.f, 0.2f), 0.85f);

    return noiseHeight;
}

float Terrain::getMountainHeight(int x, int z)
{
    float biomeScale = 60.f;
    float noiseHeight = 1.f - abs(Noise::perlin2D(glm::vec2((float)x / biomeScale, (float)z / biomeScale)));
//    noiseHeight = Noise::fbm(noiseHeight, 0.7f, 6, 2.f, 0.5f);
    noiseHeight = Noise::fbm(noiseHeight, 0.4f, 6, 1.f, 1.f);
    return glm::min(noiseHeight, 1.f);
}

void Terrain::multithreadedWork(glm::vec3 playerPos, glm::vec3 playerPosPrev, float dT)
{
    m_tryExpansionTimer += dT;
    // Only check for terrain expansion every second of real time or so
    if (m_tryExpansionTimer < 0.5f)
    {
        return;
    }
    tryExpansion(playerPos, playerPosPrev);

    // What does this do? Send data to VBO when all threads are finished?
    // 1) Query the set<Chunk*> that is written to by every BlockTypeWorker
    //    Iterate over each Chunk* in the set and pass it to a VBOWorker
    //    Then, clear the set since we have processed its contents
    // 2) Query the set<ChunkVBOdata> written to by the VBOWorkers
    // checkThreadResults();
    m_tryExpansionTimer = 0.f;
}


void Terrain::tryExpansion(glm::vec3 playerPos, glm::vec3 playerPosPrev)
{
    // Find the player's position relative to their current terrain zone
    glm::ivec2 currZone(64.f * glm::floor(playerPos.x / 64.f), 64.f * glm::floor(playerPos.z / 64.f));
    glm::ivec2 prevZone(64.f * glm::floor(playerPosPrev.x / 64.f), 64.f * glm::floor(playerPosPrev.z / 64.f));
    // Determine which terrain zones border our current position and our previous position

    // IDs are for zones
    QSet<int64_t> terrainZonesBorderingCurrPos = terrainZonesBorderingZone(currZone, N*2);
    QSet<int64_t> terrainZonesBorderingPrevPos = terrainZonesBorderingZone(prevZone, N*2);

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
                    auto &chunk = getChunkAt(x, z);
                    chunk->destroyVBOdata();
                }
            }
        }
    }
    // Check which terrain zones need VBOData
    // Send VBOData to VBOWorkers
    // If the chunks do not exist, send to HeightFieldWorker instead
    for (auto id : terrainZonesBorderingCurrPos)
    {
        // Where/How do I add them to chunksWithNewVBOData?
        // What about partially-exist zone?
        if (terrainZoneExists(id))
        {
            // Send VBO data to render only
            if (!terrainZonesBorderingPrevPos.contains(id))
            {
                glm::ivec2 coord = toCoords(id);
                for (int x=coord.x; x<coord.x+64;x+=16)
                {
                    for (int z = coord.y; z < coord.y + 64; z+= 16)
                    {
                        auto &chunk = getChunkAt(x, z);
                        // Not a constructor, it also takes the data from terrain
                        spawnVBOWorker(chunk.get());
                    }
                }
            }
        }
        // Both set block data and send VBO data to render
        else
        {
            spawnHeightFieldWorker(id);
        }
    }
}

void Terrain::spawnHeightFieldWorker(int64_t zoneToGenerate)
{
    m_generatedTerrain.insert(zoneToGenerate);
    std::vector<Chunk*> chunksForWorker;
    glm::ivec2 coords = toCoords(zoneToGenerate);
    for (int x = coords.x; x < coords.x + 64; x += 16)
    {
        for (int z = coords.x; x < coords.x + 64; x += 16)
        {
             Chunk* c = createChunkAt(x,z);
             // Why zero?
             c->setTransCount(0);
             c->setCount(0);
             chunksForWorker.push_back(c);
        }
    }
    // ChunksCompleted = chunksWithBlockTypeData?
    // ChunksCompleted always empty at each tick?
//    HeightFieldWorker *worker = new HeightFieldWorker(coords.x, coords.y, chunksForWorker,
//                                                      &m_chunksCompleted, &m_chunksCompletedLock)
    //QThreadPool::globalInstance()->start(worker);
}

void Terrain::spawnHeightFieldWorker(const QSet<int64_t> &zonesToGenerate)
{
    // Spawn worker threads to generate more Chunks
    for (int64_t zone: zonesToGenerate)
    {
        spawnHeightFieldWorker(zone);
    }
}

QSet<int64_t> Terrain::terrainZonesBorderingZone(glm::ivec2 zone, float radius)
{

}

bool Terrain::terrainZoneExists(int64_t id)
{

}
