#include "primitive.h"

Primitive::Primitive()
{

}

void Primitive::placeTree(Chunk *chunk, PrimitiveType obj, int x, int y, int z, int trunkHeight)
{
    if (chunk->getBlockAt(glm::ivec3(x, y, z)) != EMPTY) return;
    switch (obj)
    {
    case BALLOONOAK:
    {
        x = glm::clamp(x, 1, 14);
        z = glm::clamp(z, 1, 14);
        if (chunk->getBlockAt(glm::ivec3(x, y - 1, z)) != GRASS) break;

        for (int i = 0; i < trunkHeight; i++)
        {
            chunk->setBlockAt(x, y + i, z, TRUNK);
        }

        setLeavesAt(chunk, x, y + trunkHeight - 3, z);
        break;
    }

    case LARGEOAK:
    {
        x = glm::clamp(x, 2, 13);
        z = glm::clamp(z, 2, 13);
        if (chunk->getBlockAt(glm::ivec3(x, y - 1, z)) != GRASS) break;

        for (int i = 0; i < trunkHeight; i++)
        {
            chunk->setBlockAt(x, y + i, z, TRUNK);
        }

        setLeavesAt(chunk, glm::clamp(x, 1, 14), y + trunkHeight - 3, glm::clamp(z, 1, 14));
        setLeavesAt(chunk, glm::clamp(x + 1, 1, 14), y + trunkHeight - 7, glm::clamp(z + 1, 1, 14));
        setLeavesAt(chunk, glm::clamp(x - 1, 1, 14), y + trunkHeight - 5, glm::clamp(z - 2, 1, 14));
        setLeavesAt(chunk, glm::clamp(x - 2, 1, 14), y + trunkHeight - 5, glm::clamp(z + 3, 1, 14));
        break;
    }

    }
}

void Primitive::setLeavesAt(Chunk *chunk, int x, int y, int z)
{
    for (int i = -2; i <= 2; i++)
    {
        for (int j = -2; j <= 2; j++)
        {
            if (abs(i) == 2 && (abs(i) == abs(j))) continue;
            for (int h = y; h < y + 3; h++)
            {
                int newX = glm::clamp(x + i, 0, 15);
                int newZ = glm::clamp(z + j, 0, 15);
                if (chunk->getBlockAt(glm::ivec3(newX, h, newZ)) == EMPTY)
                    chunk->setBlockAt(newX, h, newZ, LEAF);
            }
        }
    }
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (abs(i) == abs(j) && i != 0) continue;
            int newX = glm::clamp(x + i, 0, 15);
            int newZ = glm::clamp(z + j, 0, 15);
            if (chunk->getBlockAt(glm::ivec3(newX, y + 3, newZ)) == EMPTY)
                chunk->setBlockAt(newX, y + 3, newZ, LEAF);
        }
    }
}
