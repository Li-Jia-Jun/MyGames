#pragma once
#include "smartpointerhelp.h"
#include "drawable.h"
#include "blockType.h"
#include"chunk.h"
#include <array>
#include <unordered_map>

class Terrain;

// One Chunk is a 16 x 256 x 16 section of the world,
// containing all the Minecraft blocks in that area.
// We divide the world into Chunks in order to make
// recomputing its VBO data faster by not having to
// render all the world at once, while also not having
// to render the world block by block.
class Chunk : public TransparentDrawable{
private:
    // All of the blocks contained within this Chunk
    std::array<BlockType, 65536> m_blocks;
    // This Chunk's four neighbors to the north, south, east, and west
    // The third input to this map just lets us use a Direction as
    // a key for this map.
    // These allow us to properly determine
    std::unordered_map<Direction, Chunk*, EnumHash> m_neighbors;

public:
    Chunk(OpenGLContext *mp_context);
    BlockType getBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getBlockAt(int x, int y, int z) const;
    BlockType getBlockAt(glm::ivec3 vec3);

    void informNeighborUpdate();

    std::unordered_map<Direction, Chunk*, EnumHash> getNeighbors();

    void setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t);
    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);

    void setChunkPos(glm::ivec2);
    glm::ivec2 getChunkPos();

    float getShade(int x, int y, int z);

    virtual void createVBOdata();
    virtual void destroyVBOdata();
    void sendVBOdata(struct VBOData*);
public:
    // Create a x-z plane chunk for testing
    void TempCreatePlaneShape(Terrain * terrain);

    // Transfer the out of bound block position of this chunk
    // to block position in neighbor chunk
    glm::ivec3 getPosAtNeighborChunk(glm::ivec3 outPos);

    bool checkFaceVisible(BlockType thisFace, BlockType nextFace);
    glm::ivec2 m_pos;
    VBOData* vbo_data;

    bool vboDataNeedUpdate;
    bool hasTransparentAtEdge;  // Marks whether it needs to update by its neighbor
};
