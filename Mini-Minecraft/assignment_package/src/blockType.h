#ifndef BLOCKTYPE_H
#define BLOCKTYPE_H

#include <cstddef>
#include <vector>
#include "glm_includes.h"

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.
enum BlockType : unsigned char
{
    // When adding a new type here, please do:
    // 1. Add texture mapping coordinate for new type face image in BlockTypeFunc::getBlockTypeFaceUV()
    // 2. Add texture mapping coordinate for new type item image in BlockTypeFunc::getBlockTypeItemUV()
    // 3. If new type is transparent, add it into BlockTypeFunc::checkBlockTypeTransparent()
    // 4. If new type is animateable, add it into BlockTypeFunc::checkBllokTypeAnimateable()
    // 5. If nwe type has cross face like flower, add it into BlockTypeFunc::checkBlockTypeHasCrossFace()
    EMPTY = 0, GRASS = 1, DIRT = 2, STONE = 3, WATER = 4,
    LAVA = 5, ICE = 6, SNOW = 7, SAND = 8, BEDROCK = 9,
    WEED = 10, REDFLOWER = 11, YELLOWFLOWER = 12, TRUNK = 13,
    LEAF = 14, CACTUS = 15, SUGARCANE = 16, METAL = 17,
    REDSTONEORE = 18, EMERALDORE = 19, DIAMONDORE = 20
};

// Lets us use any enum class as the key of a
// std::unordered_map
struct EnumHash {
    template <typename T>
    size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};

// The six cardinal directions in 3D space
enum Direction : unsigned char
{
    XPOS, XNEG, YPOS, YNEG, ZPOS, ZNEG, CROSS1, CROSS2
};

struct NeighborDirection
{
    Direction direction;
    glm::ivec3 vecDirection;

    NeighborDirection(Direction d, glm::ivec3 v) :
        direction(d), vecDirection(v) {}
};


// Function collections about BlockType
class BlockTypeFunc
{
public:
    static bool checkBlockTypeTransparent(BlockType type);
    static float checkBlockTypeAnimateable(BlockType type);
    static bool checkBlockTypeHasCrossFace(BlockType type);
    static glm::vec2 getBlockTypeFaceUV(BlockType type, Direction dir, glm::vec3 offset);
    static glm::vec2 getBlockTypeItemUV(BlockType type, glm::vec2 offset);

};

// Function collections about Direction
class DirectionFunc
{
public:
    static std::vector<glm::vec3> getDirectionOffset(Direction dir);
};

#endif // BLOCKTYPE_H
