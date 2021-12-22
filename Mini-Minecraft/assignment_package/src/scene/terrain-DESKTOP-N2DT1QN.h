#pragma once
#include "smartpointerhelp.h"
#include "glm_includes.h"
#include "chunk.h"
#include <array>
#include <unordered_map>
#include <unordered_set>
#include "shaderprogram.h"
#include "cube.h"

#include<QRunnable>
#include<QMutex>
#include<QThreadPool>
#include"chunkworkers.h"
//using namespace std;

// Helper functions to convert (x, z) to and from hash map key
int64_t toKey(int x, int z);
glm::ivec2 toCoords(int64_t k);

// The container class for all of the Chunks in the game.
// Ultimately, while Terrain will always store all Chunks,
// not all Chunks will be drawn at any given time as the world
// expands.
class Terrain {
private:

    // Render (2N+1)x(2N+1) terrain zones around player
    // Each terrain zone consists of 4x4 chunks
    // Draw radius = 3*3
    // Create radius = 5*5
    const int N = 1;

    // Stores every Chunk according to the location of its lower-left corner
    // in world space.
    // We combine the X and Z coordinates of the Chunk's corner into one 64-bit int
    // so that we can use them as a key for the map, as objects like std::pairs or
    // glm::ivec2s are not hashable by default, so they cannot be used as keys.
    std::unordered_map<int64_t, uPtr<Chunk>> m_chunks;

    // We will designate every 64 x 64 area of the world's x-z plane
    // as one "terrain generation zone". Every time the player moves
    // near a portion of the world that has not yet been generated
    // (i.e. its lower-left coordinates are not in this set), a new
    // 4 x 4 collection of Chunks is created to represent that area
    // of the world.
    // The world that exists when the base code is run consists of exactly
    // one 64 x 64 area with its lower-left corner at (0, 0).
    // When milestone 1 has been implemented, the Player can move around the
    // world to add more "terrain generation zone" IDs to this set.
    // While only the 3 x 3 collection of terrain generation zones
    // surrounding the Player should be rendered, the Chunks
    // in the Terrain will never be deleted until the program is terminated.
    std::unordered_set<int64_t> m_generatedTerrain;

    // TODO: DELETE ALL REFERENCES TO m_geomCube AS YOU WILL NOT USE
    // IT IN YOUR FINAL PROGRAM!
    // The instance of a unit cube we can use to render any cube.
    // Presently, Terrain::draw renders one instance of this cube
    // for every non-EMPTY block within its Chunks. This is horribly
    // inefficient, and will cause your game to run very slowly until
    // milestone 1's Chunk VBO setup is completed.
    Cube m_geomCube;

    // Player's current terrain zone in Last update
    glm::ivec2 m_lastPlayerTerrainZone;
    // Timer for tryExpansion
    float m_tryExpansionTimer;
    std::unordered_set<Chunk*>*m_chunksCompleted;
    QMutex *m_chunksCompletedLock;

    OpenGLContext* mp_context;

    static const int minHeight = 0;
    static const int maxHeight = 255;
    static const int maxUndergroundHeight = 128;
    static const int minSurfaceHeight = maxUndergroundHeight + 1;
    static const int maxWaterHeight = maxUndergroundHeight + 10;

public:
    Terrain(OpenGLContext *context);
    ~Terrain();

    // Do these world-space coordinates lie within
    // a Chunk that exists?
    bool hasChunkAt(int x, int z) const;
    // Assuming a Chunk exists at these coords,
    // return a mutable reference to it
    uPtr<Chunk>& getChunkAt(int x, int z);
    // Assuming a Chunk exists at these coords,
    // return a const reference to it
    const uPtr<Chunk>& getChunkAt(int x, int z) const;
    // Given a world-space coordinate (which may have negative
    // values) return the block stored at that point in space.
    BlockType getBlockAt(int x, int y, int z) const;
    BlockType getBlockAt(glm::vec3 p) const;
    // Given a world-space coordinate (which may have negative
    // values) set the block at that point in space to the
    // given type.
    void setBlockAt(int x, int y, int z, BlockType t);

    // Get terrain zone position
    glm::ivec2 getZonePos(double x, double z) const;

    Chunk* createChunkAt(int x, int z);
    void linkChunkNeighbor(int x, int z);

    void draw(ShaderProgram *shaderProgram);

    void drawTransparent(ShaderProgram *shaderProgram);

    // Update terrain based on player position
    void updateTerrain(glm::vec3 playerPos); 


    void generateBiome(int x, int z);

    void generateUndergroundLayer(int x, int z);
    void generateGrassland(int x, int z, float normalizedHeight);
    void generateMountain(int x, int z, float normalizedHeight);


    float getGrasslandHeight(int x, int z);
    float getMountainHeight(int x, int z);

    void multithreadedWork(glm::vec3 playerPos, glm::vec3 playerPosPrev, float dT);
    void tryExpansion(glm::vec3 playerPos, glm::vec3 playerPosPrev);
    QSet<int64_t> terrainZonesBorderingZone(glm::ivec2, float);
    bool terrainZoneExists(int64_t);
    void spawnHeightFieldWorker(int64_t zoneToGenerate);
    void spawnHeightFieldWorker(const QSet<int64_t> &zonesToGenerate);
    void spawnVBOWorker(Chunk*);
};
