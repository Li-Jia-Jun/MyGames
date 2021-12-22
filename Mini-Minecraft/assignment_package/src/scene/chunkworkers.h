#ifndef CHUNKWORKERS_H
#define CHUNKWORKERS_H
#include"chunk.h"
//#include"terrain.h"
#include <QRunnable>
#include<QMutex>
#include<unordered_set>

struct VBOData
{
    std::vector <glm::vec4> attrs;
    std::vector <GLint> idx;
    std::vector <glm::vec4>transAttrs;
    std::vector<GLint> transIdx;
    Chunk* associated_chunk;
};
class Terrain;

class BlockTypeWorker : public QRunnable
{
private:
    // Coords of the terrain zone being generated
    int m_xCorner, m_zCorner;
    std::vector<uPtr<Chunk>> m_chunksToFill;
    std::unordered_set<Chunk*>*m_chunksCompleted;
    QMutex *m_chunksCompletedLock;
    Terrain* m_terrain;
public:
    BlockTypeWorker(int x, int z,
                    std::unordered_set<Chunk*>* chunksCompleted,
                    QMutex* chunksCompletedLock
                    ,Terrain* terrain);
    void run() override;
};

class VBOWorker: public QRunnable
{
private:
    Chunk* m_chunkToRender;
    std::unordered_set<VBOData*>* m_VBODataCompleted;
    QMutex *m_VBODataCompletedLock;
public:
    VBOWorker(Chunk* chunkToRender,
              std::unordered_set<VBOData*>* newVBOData,
              QMutex* VBODataCompletedLock);
    void run() override;
};
#endif // CHUNKWORKERS_H
