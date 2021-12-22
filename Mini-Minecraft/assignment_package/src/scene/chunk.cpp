#include "chunk.h"
#include"chunkworkers.h"
#include <unordered_set>
#include <iostream>
#include "terrain.h"
#include "primitive.h"

Chunk::Chunk(OpenGLContext *mp_context) :
    TransparentDrawable(mp_context),
    m_blocks(), m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}}, vbo_data(nullptr),
    vboDataNeedUpdate(false), hasTransparentAtEdge(false)
{
    std::fill_n(m_blocks.begin(), 65536, EMPTY);
}

// Does bounds checking with at()
BlockType Chunk::getBlockAt(unsigned int x, unsigned int y, unsigned int z) const{
    return m_blocks.at(x + 16 * y + 16 * 256 * z);
}

// Exists to get rid of compiler warnings about int -> unsigned int implicit conversion
BlockType Chunk::getBlockAt(int x, int y, int z) const {
    return getBlockAt(static_cast<unsigned int>(x), static_cast<unsigned int>(y), static_cast<unsigned int>(z));
}

BlockType Chunk::getBlockAt(glm::ivec3 vec3)
{
    return getBlockAt(vec3[0], vec3[1], vec3[2]);
}

// Does bounds checking with at()
void Chunk::setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t)
{
    m_blocks.at(x + 16 * y + 16 * 256 * z) = t;

    if(!hasTransparentAtEdge && BlockTypeFunc::checkBlockTypeTransparent(t) &&
            (x == 0 || x == 15 || z == 0 || z == 15))
    {
        hasTransparentAtEdge = true;
    }
}

glm::ivec3 Chunk::getPosAtNeighborChunk(glm::ivec3 outPos)
{
    if(outPos[0] == -1)
    {
        outPos[0] = 15;
    }
    else if(outPos[0] == 16)
    {
        outPos[0] = 0;
    }
    if(outPos[2] == -1)
    {
        outPos[2] = 15;
    }
    else if(outPos[2] == 16)
    {
        outPos[2] = 0;
    }

    return outPos;
}

void Chunk::setChunkPos(glm::ivec2 pos)
{
    m_pos = pos;
}

glm::ivec2 Chunk::getChunkPos()
{
    return m_pos;
}
std::unordered_map<Direction, Chunk*, EnumHash> Chunk::getNeighbors()
{
    return m_neighbors;
}

float Chunk::getShade(int x, int y, int z)
{
    BlockType type = getBlockAt(x, y, z);

    float shade = 0;
    if(type == BlockType::GRASS || type == BlockType::REDFLOWER || type == BlockType::YELLOWFLOWER || type == BlockType::WEED)
    {
        for(int yy = y + 1; yy < 256 && yy < y + 20; yy++)
        {
            if(getBlockAt(x, yy, z) == BlockType::LEAF ||
                    (x-1 >= 0 && getBlockAt(x-1, yy, z) == BlockType::LEAF) ||
                    (z-1 >= 0 && getBlockAt(x, yy, z-1) == BlockType::LEAF) ||
                    (x+1 < 16 && getBlockAt(x+1, yy, z) == BlockType::LEAF) ||
                    (z+1 < 16 && getBlockAt(x, yy, z+1) == BlockType::LEAF))
            {
                shade = 1;
                break;
            }
        }
    }
    return shade;
}

const static std::unordered_map<Direction, Direction, EnumHash> oppositeDirection {
    {XPOS, XNEG},
    {XNEG, XPOS},
    {YPOS, YNEG},
    {YNEG, YPOS},
    {ZPOS, ZNEG},
    {ZNEG, ZPOS}
};

const static std::array<NeighborDirection, 6> neighborDirs =
{
    NeighborDirection(Direction::XPOS, glm::ivec3(1, 0, 0)),
    NeighborDirection(Direction::XNEG, glm::ivec3(-1, 0, 0)),
    NeighborDirection(Direction::YPOS, glm::ivec3(0, 1, 0)),
    NeighborDirection(Direction::YNEG, glm::ivec3(0, -1, 0)),
    NeighborDirection(Direction::ZPOS, glm::ivec3(0, 0, 1)),
    NeighborDirection(Direction::ZNEG, glm::ivec3(0, 0, -1))
};

bool Chunk::checkFaceVisible(BlockType thisFace, BlockType nextFace)
{
    bool thisTransparent = BlockTypeFunc::checkBlockTypeTransparent(thisFace);
    bool nextTransparent = BlockTypeFunc::checkBlockTypeTransparent(nextFace);
    bool nextHasCrossFace = BlockTypeFunc::checkBlockTypeHasCrossFace(nextFace);

    if(thisTransparent)
    {
//        if(nextFace == LAVA)
//            return true;
        return nextFace == BlockType::EMPTY || nextHasCrossFace;
    }
    else
    {
//        if(nextFace == LAVA)
//            return true;

        return nextFace == BlockType::EMPTY || nextTransparent || nextHasCrossFace;
    }
}

void Chunk::linkNeighbor(uPtr<Chunk> &neighbor, Direction dir)
{
    if(neighbor != nullptr)
    {
        this->m_neighbors[dir] = neighbor.get();
        neighbor->m_neighbors[oppositeDirection.at(dir)] = this;
    }  
}

void Chunk::createVBOdata()
{
    // Opaque VBO
    std::vector<glm::vec4> attrs;
    std::vector<GLint> idx;

    // Transparent VBO
    std::vector<glm::vec4> transAttrs;
    std::vector<GLint> transIdx;

    for(int x = 0; x < 16; ++x)
    {
        for(int z = 0; z < 16; ++z)
        {
            for(int y = 0; y < 256; ++y)
            {
                BlockType block = getBlockAt(x, y, z);

                // Skip empty block
                if(block == BlockType::EMPTY)
                {
                    continue;
                }

                // Special case: cross face vbo
                if(BlockTypeFunc::checkBlockTypeHasCrossFace(block))
                {
                    //Cross face 1
                    std::vector crossFace1Offset = DirectionFunc::getDirectionOffset(Direction::CROSS1);
                    for(auto offset : crossFace1Offset)
                    {
                        glm::vec4 worldPos = glm::vec4(offset + glm::vec3(x, y, z) + glm::vec3(m_pos[0], 0, m_pos[1]), 1);
                        glm::vec2 uv = BlockTypeFunc::getBlockTypeFaceUV(block, Direction::CROSS1, offset);

                        transAttrs.push_back(worldPos);
                        transAttrs.push_back(glm::vec4(uv, 0, 0));
                        transAttrs.push_back(glm::vec4(0, 1.f, 0, 0));
                    }

                    // Cross face 2
                    std::vector crossFace2Offset = DirectionFunc::getDirectionOffset(Direction::CROSS2);
                    for(auto offset : crossFace2Offset)
                    {
                        glm::vec4 worldPos = glm::vec4(offset + glm::vec3(x, y, z) + glm::vec3(m_pos[0], 0, m_pos[1]), 1);
                        glm::vec2 uv = BlockTypeFunc::getBlockTypeFaceUV(block, Direction::CROSS2, offset);

                        transAttrs.push_back(worldPos);
                        transAttrs.push_back(glm::vec4(uv, 0, 0));
                        transAttrs.push_back(glm::vec4(0, 1.f, 0, 0));
                    }

                    continue;
                }

                // Create VBO data face by face
                for(auto nd : neighborDirs)
                {
                    glm::ivec3 pos = glm::ivec3(x, y, z) + nd.vecDirection;
                    BlockType type = getBlockAt(x,y,z);

                    // Skip face that has neighbor(in this chunk or neighboring chunk)
                    bool Xout = pos[0] < 0 || pos[0] > 15;
                    bool Zout = pos[2] < 0 || pos[2] > 15;
                    bool Yout = pos[1] < 0 || pos[1] > 254;
                    if(Yout)
                    {
                        // Don't skip face out of Y direction
                    }
                    else if(Xout || Zout)
                    {
                        // Check neighboring chunk
                        if(m_neighbors[nd.direction] != nullptr)
                        {
                            BlockType nType = m_neighbors[nd.direction]->getBlockAt(getPosAtNeighborChunk(pos));
                            if(!checkFaceVisible(type, nType))
                            {
                                continue;
                            }
                        }
                    }
                    else if(!checkFaceVisible(type, getBlockAt(pos)))
                    {
                        continue;
                    }

                    float shade = 0;
                    if(nd.direction == Direction::YPOS)
                    {
                        shade = getShade(x, y, z);
                    }

                    std::vector offsets = DirectionFunc::getDirectionOffset(nd.direction);
                    for(auto offset : offsets)
                    {
                        glm::vec4 worldPos = glm::vec4(offset + glm::vec3(x, y, z) + glm::vec3(m_pos[0], 0, m_pos[1]), 1);
                        glm::vec2 uv = BlockTypeFunc::getBlockTypeFaceUV(block, nd.direction, offset);
                        bool isTrans = BlockTypeFunc::checkBlockTypeTransparent(type);
                        float isAnimt = BlockTypeFunc::checkBlockTypeAnimateable(type);
                        if(isTrans)
                        {
                            transAttrs.push_back(worldPos);                         // Position
                            transAttrs.push_back(glm::vec4(uv, isAnimt, 0));        // UV, animation flag
                            transAttrs.push_back(glm::vec4(nd.vecDirection, 0));    // Normal
                        }
                        else
                        {
                            attrs.push_back(worldPos);                      // Position
                            attrs.push_back(glm::vec4(uv, isAnimt, shade));     // UV, animation flag
                            attrs.push_back(glm::vec4(nd.vecDirection, 0)); // Normal
                        }
                    }
                }
            }
        }
    }

    int count = attrs.size() / 3;
    for(int i = 0; i < count; i += 4)
    {
        idx.push_back(i);
        idx.push_back(i + 1);
        idx.push_back(i + 2);

        idx.push_back(i);
        idx.push_back(i + 2);
        idx.push_back(i + 3);
    }
    m_count = idx.size();

    int transCount = transAttrs.size() / 3;
    for(int i = 0; i < transCount; i += 4)
    {
        transIdx.push_back(i);
        transIdx.push_back(i + 1);
        transIdx.push_back(i + 2);

        transIdx.push_back(i);
        transIdx.push_back(i + 2);
        transIdx.push_back(i + 3);
    }
    m_TransCount = transIdx.size();

    // Opaque interleaved VBO uses position channel
    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufPos);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, attrs.size() * sizeof(glm::vec4), attrs.data(), GL_STATIC_DRAW);

    // Transparent interleaved VBO uses color channel
    generateTransparentIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_TransBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, transIdx.size() * sizeof(GLuint), transIdx.data(), GL_STATIC_DRAW);

    generateCol();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufCol);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, transAttrs.size() * sizeof(glm::vec4), transAttrs.data(), GL_STATIC_DRAW);
}

void Chunk::destroyVBOdata()
{
    TransparentDrawable::destroyVBOdata();
}

void Chunk::sendVBOdata(VBOData* v)
{        
    // Opaque interleaved VBO uses position channel
    this->m_count = v->idx.size();

    this->generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v->associated_chunk->m_bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, v->idx.size() * sizeof(GLuint), v->idx.data(), GL_STATIC_DRAW);

    this->generatePos();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v->associated_chunk->m_bufPos);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, v->attrs.size() * sizeof(glm::vec4), v->attrs.data(), GL_STATIC_DRAW);

    // Transparent interleaved VBO uses color channel
    this->m_TransCount = v->transIdx.size();

    this->generateTransparentIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, v->associated_chunk->m_TransBufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, v->transIdx.size() * sizeof(GLuint), v->transIdx.data(), GL_STATIC_DRAW);

    this->generateCol();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufCol);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, v->transAttrs.size() * sizeof(glm::vec4), v->transAttrs.data(), GL_STATIC_DRAW);
}

void Chunk::TempCreatePlaneShape(Terrain *terrain)
{
    for(int x = 0; x < 16; ++x)
    {
        for(int z = 0; z < 16; ++z)
        {
            for(int y = 0; y < 256; ++y)
            {
                BlockType type = BlockType::EMPTY;
                if(y == 121)
                {
                    if(x == 6 && z == 6)
                    {
                        type = BlockType::WATER;
                    }
                    else if(x == 9 && z == 9)
                    {
                        type = BlockType::WEED;
                    }
                    else if(x == 12 && z == 12)
                    {
                        type = BlockType::REDFLOWER;
                    }
                }
                if(y == 120)
                {
                    type = BlockType::DIRT;
                }

                //terrain->setBlockAt(m_pos[0] + x, y, m_pos[1] + z, type);
                 setBlockAt(x, y, z, type);

            }
        }
    }
//    Primitive::placeTree(this, LARGEOAK, 8, 200, 8);
}

void Chunk::informNeighborUpdate()
{
    for(auto n : m_neighbors)
    {
        if(n.second->hasTransparentAtEdge)
        {
            n.second->vboDataNeedUpdate = true;
        }
    }
}
