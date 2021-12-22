#include "chunkworkers.h"
#include "terrain.h"
#include <iostream>
#include <exception>


const static std::array<NeighborDirection, 6> neighborDirs =
{
    NeighborDirection(Direction::XPOS, glm::ivec3(1, 0, 0)),
    NeighborDirection(Direction::XNEG, glm::ivec3(-1, 0, 0)),
    NeighborDirection(Direction::YPOS, glm::ivec3(0, 1, 0)),
    NeighborDirection(Direction::YNEG, glm::ivec3(0, -1, 0)),
    NeighborDirection(Direction::ZPOS, glm::ivec3(0, 0, 1)),
    NeighborDirection(Direction::ZNEG, glm::ivec3(0, 0, -1))
};

BlockTypeWorker::BlockTypeWorker(int x, int z,
                std::unordered_set<Chunk*>* chunksCompleted,
                QMutex* chunksCompletedLock
                                 ,Terrain* terrain)
    :m_xCorner(x), m_zCorner(z),
    m_chunksCompleted(chunksCompleted), m_chunksCompletedLock(chunksCompletedLock)
  , m_terrain(terrain)
{
    setAutoDelete(true);
}

void BlockTypeWorker::run()
{
    try
    {
        for (int x = m_xCorner; x < m_xCorner + 64; x += 16)
        {
            for (int z = m_zCorner; z < m_zCorner + 64; z += 16) //Change!!!!!!!!!
            {
                uPtr<Chunk> c = mkU<Chunk>(m_terrain->getOpenGLContext());
                Chunk *cPtr = c.get();
                cPtr->setChunkPos(glm::ivec2(x, z));

//                cPtr->TempCreatePlaneShape(m_terrain);

                for (int xx = x; xx < x+16; ++xx)
                {
                    for (int zz = z; zz < z+16; ++zz)
                    {
                        this->m_terrain->generateBiome(cPtr, xx, zz);
                    }
                }

                m_terrain->AddChunkToTerrain(std::move(c));

                m_terrain->linkChunkNeighbor(x, z);

                // Inform neighbors to update VBO at zone border
                if(x == m_xCorner || x == m_xCorner + 48 ||
                   z == m_zCorner || z == m_zCorner + 48)
                {
                    cPtr->informNeighborUpdate();
                }

                this->m_chunksCompletedLock->lock();

                // Modify the set
                this->m_chunksCompleted->insert(cPtr);

                this->m_chunksCompletedLock->unlock();
            }
        }
    }
    catch(std::exception)
    {
        std::cerr << "blocktype worker exception" << std::endl;
    }
}

VBOWorker::VBOWorker(Chunk* chunkToRender,
                     std::unordered_set<VBOData*>* newVBOData,
                     QMutex* VBODataCompletedLock)
    : m_chunkToRender(chunkToRender),
      m_VBODataCompleted(newVBOData),
      m_VBODataCompletedLock(VBODataCompletedLock)
{
    setAutoDelete(true);
}

void VBOWorker::run()
{
    try
    {      
        VBOData* newVBODataPtr = new VBOData;

        for(int x = 0; x < 16; ++x)
        {
            for(int z = 0; z < 16; ++z)
            {
                for(int y = 0; y < 256; ++y)
                {
                    BlockType block = this->m_chunkToRender->getBlockAt(x, y, z);

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
                            glm::vec4 worldPos = glm::vec4(offset + glm::vec3(x, y, z) +
                                                           glm::vec3(m_chunkToRender->m_pos[0], 0,
                                                           m_chunkToRender->m_pos[1]), 1);
                            glm::vec2 uv = BlockTypeFunc::getBlockTypeFaceUV(block, Direction::CROSS1, offset);

                            newVBODataPtr->transAttrs.push_back(worldPos);
                            newVBODataPtr->transAttrs.push_back(glm::vec4(uv, 0, 0));
                            newVBODataPtr->transAttrs.push_back(glm::vec4(0, 1.f, 0, 0));
                        }

                        // Cross face 2
                        std::vector crossFace2Offset = DirectionFunc::getDirectionOffset(Direction::CROSS2);
                        for(auto offset : crossFace2Offset)
                        {
                            glm::vec4 worldPos = glm::vec4(offset + glm::vec3(x, y, z) +
                                                           glm::vec3(m_chunkToRender->m_pos[0], 0,
                                                           m_chunkToRender->m_pos[1]), 1);
                            glm::vec2 uv = BlockTypeFunc::getBlockTypeFaceUV(block, Direction::CROSS2, offset);

                            newVBODataPtr->transAttrs.push_back(worldPos);
                            newVBODataPtr->transAttrs.push_back(glm::vec4(uv, 0, 0));
                            newVBODataPtr->transAttrs.push_back(glm::vec4(0, 1.f, 0, 0));
                        }

                        continue;
                    }


                    // Create VBO data face by face
                    for(auto nd : neighborDirs)
                    {
                        glm::ivec3 pos = glm::ivec3(x, y, z) + nd.vecDirection;
                        BlockType type = this->m_chunkToRender->getBlockAt(x,y,z);

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
                            if(m_chunkToRender->getNeighbors()[nd.direction] != nullptr)
                            {
                                BlockType nType = m_chunkToRender->getNeighbors()[nd.direction]->getBlockAt(m_chunkToRender->getPosAtNeighborChunk(pos));
                                if(!m_chunkToRender->checkFaceVisible(type, nType))
                                {
                                    continue;
                                }
                            }
                        }
                        else if(!m_chunkToRender->checkFaceVisible(type, m_chunkToRender->getBlockAt(pos)))
                        {
                            continue;
                        }

                        float shade = 0;
                        if(nd.direction == Direction::YPOS)
                        {
                            shade = m_chunkToRender->getShade(x, y, z);
                        }

                        std::vector offsets = DirectionFunc::getDirectionOffset(nd.direction);
                        for(auto offset : offsets)
                        {
                            glm::vec4 worldPos = glm::vec4(offset + glm::vec3(x, y, z) + glm::vec3(m_chunkToRender->m_pos[0], 0, m_chunkToRender->m_pos[1]), 1);
                            glm::vec2 uv = BlockTypeFunc::getBlockTypeFaceUV(block, nd.direction, offset);
                            bool isTrans = BlockTypeFunc::checkBlockTypeTransparent(type);
                            float isAnimt = BlockTypeFunc::checkBlockTypeAnimateable(type);
                            if(isTrans)
                            {
                                 newVBODataPtr->transAttrs.push_back(worldPos);                         // Position
                                 newVBODataPtr->transAttrs.push_back(glm::vec4(uv, isAnimt, 0));        // UV, animation flag
                                 newVBODataPtr->transAttrs.push_back(glm::vec4(nd.vecDirection, 0));    // Normal
                            }
                            else
                            {
                                 newVBODataPtr->attrs.push_back(worldPos);                      // Position
                                 newVBODataPtr->attrs.push_back(glm::vec4(uv, isAnimt, shade));     // UV, animation flag
                                 newVBODataPtr->attrs.push_back(glm::vec4(nd.vecDirection, 0)); // Normal
                            }
                        }
                    }
                }
            }
        }

        int count = newVBODataPtr->attrs.size() / 3;
        for(int i = 0; i < count; i += 4)
        {
            newVBODataPtr->idx.push_back(i);
            newVBODataPtr->idx.push_back(i + 1);
            newVBODataPtr->idx.push_back(i + 2);

            newVBODataPtr->idx.push_back(i);
            newVBODataPtr->idx.push_back(i + 2);
            newVBODataPtr->idx.push_back(i + 3);
        }

        int transCount = newVBODataPtr->transAttrs.size() / 3;
        for(int i = 0; i < transCount; i += 4)
        {
            newVBODataPtr->transIdx.push_back(i);
            newVBODataPtr->transIdx.push_back(i + 1);
            newVBODataPtr->transIdx.push_back(i + 2);

            newVBODataPtr->transIdx.push_back(i);
            newVBODataPtr->transIdx.push_back(i + 2);
            newVBODataPtr->transIdx.push_back(i + 3);
        }

       // VBOData *oldVBO = m_chunkToRender->vbo_data;
        newVBODataPtr->associated_chunk = this->m_chunkToRender;
        newVBODataPtr->associated_chunk->vbo_data = newVBODataPtr;
        //delete oldVBO;

        // Lock before modifying the newVBOData set
        this->m_VBODataCompletedLock->lock();
        // Insert the new data
        this->m_VBODataCompleted->insert(newVBODataPtr);
        // Unlock after finishes
        this->m_VBODataCompletedLock->unlock();

    }
    catch(std::exception e)
    {
        std::cerr << "vbo worker exception" << e.what() << std::endl;
    }
}

