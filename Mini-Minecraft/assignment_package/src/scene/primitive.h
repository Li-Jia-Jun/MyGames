#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "chunk.h"

enum PrimitiveType : unsigned char
{
    BALLOONOAK = 0, LARGEOAK = 1
};


class Primitive
{
public:
    Primitive();

    /// Start at the bottom center of the primitive
    static void placeTree(Chunk* chunk, PrimitiveType obj, int x, int y, int z, int trunkHeight);

    /// Start at the bottom center of leaves
    static void setLeavesAt(Chunk* chunk, int x, int y, int z);
};

#endif // PRIMITIVE_H
