#include "blockType.h"
#include <iostream>
#include <unordered_map>


bool BlockTypeFunc::checkBlockTypeTransparent(BlockType type)
{
    if(type == BlockType::WATER || type == BlockType::ICE)
    {
        return true;
    }
    return false;
}

float BlockTypeFunc::checkBlockTypeAnimateable(BlockType type)
{
    if(type == BlockType::WATER )
    {
        return 1;
    }

    if(type == BlockType::LAVA)
    {
        return 2;
    }

    return false;
}

bool BlockTypeFunc::checkBlockTypeHasCrossFace(BlockType type)
{
    return type == BlockType::WEED || type == BlockType::REDFLOWER || type == BlockType::YELLOWFLOWER ||
           type == BlockType::SUGARCANE;
}

glm::vec2 BlockTypeFunc::getBlockTypeFaceUV(BlockType type, Direction dir, glm::vec3 offset)
{
    // 1. Get face uv offset from 3d offset
    glm::vec2 uvOffset;
    switch(dir)
    {
    case Direction::XPOS:
    case Direction::XNEG:
        uvOffset = glm::vec2(offset[2], offset[1]);
        break;
    case Direction::YPOS:
    case Direction::YNEG:
        uvOffset = glm::vec2(offset[0], offset[2]);
        break;
    case Direction::ZPOS:
    case Direction::ZNEG:
        uvOffset = glm::vec2(offset[0], offset[1]);
        break;
    case Direction::CROSS1:
        uvOffset = glm::vec2(offset[0], offset[1]);
        break;
    case Direction::CROSS2:
        uvOffset = glm::vec2(1 - offset[0], offset[1]);
        break;
    }

    // 2. Get actual face uv by uv offset and coordinate in texture map
    //  For example, (8,13)/16 is the grid coordinate of grass top face in texture map
    glm::vec2 uv;
    switch(type)
    {
    case BlockType::GRASS:
        if(dir == Direction::YPOS)      uv = (glm::vec2(8, 13) + uvOffset) / 16.f;
        else if(dir == Direction::YNEG) uv = (glm::vec2(2, 15) + uvOffset) / 16.f;
        else                            uv = (glm::vec2(3, 15) + uvOffset) / 16.f;
        break;
    case BlockType::DIRT:
        uv = (glm::vec2(2, 15) + uvOffset) / 16.f;
        break;
    case BlockType::STONE:
        uv = (glm::vec2(1, 15) + uvOffset) / 16.f;
        break;
    case BlockType::WATER:
        uv = (glm::vec2(13, 3) + uvOffset) / 16.f;
        break;
    case BlockType::LAVA:
        uv = (glm::vec2(13, 1) + uvOffset) / 16.f;
        break;
    case BlockType::ICE:
        uv = (glm::vec2(3, 11) + uvOffset) / 16.f;
        break;
    case BlockType::SNOW:
        uv = (glm::vec2(2, 11) + uvOffset) / 16.f;
        break;
    case BlockType::SAND:
        uv = (glm::vec2(2, 14) + uvOffset) / 16.f;
        break;
    case BlockType::BEDROCK:
        uv = (glm::vec2(1, 14) + uvOffset) / 16.f;
        break;
    case BlockType::WEED:
        uv = (glm::vec2(7, 13) + uvOffset) / 16.f;
        break;
    case BlockType::REDFLOWER:
        uv = (glm::vec2(12, 15) + uvOffset) / 16.f;
        break;
    case BlockType::YELLOWFLOWER:
        uv = (glm::vec2(13, 15) + uvOffset) / 16.f;
        break;
    case BlockType::TRUNK:
        if(dir == Direction::YPOS || dir == Direction::YNEG) uv = (glm::vec2(5, 14) + uvOffset) / 16.f;
        else uv = (glm::vec2(4, 14) + uvOffset) / 16.f;
        break;
    case BlockType::LEAF:
        uv = (glm::vec2(5, 12) + uvOffset) / 16.f;
        break;
    case BlockType::CACTUS:
        if(dir == Direction::YPOS)      uv = (glm::vec2(5, 11) + uvOffset) / 16.f;
        else if(dir == Direction::YNEG) uv = (glm::vec2(7, 11) + uvOffset) / 16.f;
        else                            uv = (glm::vec2(6, 11) + uvOffset) / 16.f;
        break;
    case BlockType::SUGARCANE:
        uv = (glm::vec2(9, 11) + uvOffset) / 16.f;
        break;
    case BlockType::METAL:
        uv = (glm::vec2(5, 15) + uvOffset) / 16.f;
        break;
    case BlockType::REDSTONEORE:
        uv = (glm::vec2(7, 1) + uvOffset) / 16.f;
        break;
    case BlockType::EMERALDORE:
        uv = (glm::vec2(8, 1) + uvOffset) / 16.f;
        break;
    case BlockType::DIAMONDORE:
        uv = (glm::vec2(9, 1) + uvOffset) / 16.f;
        break;
    default:
        //std::cerr << "block type [" << type << "] texture coordinate is not defined in code yet.";
        //std::cerr << "uv;";
        uv = glm::vec2(0, 0);
        break;
    }

    return uv;
}

glm::vec2 BlockTypeFunc::getBlockTypeItemUV(BlockType type, glm::vec2 offset)
{
    // TODO:: mapping blocktype to uv coordinates
    return glm::vec2(0, 0);
}

std::vector<glm::vec3> DirectionFunc::getDirectionOffset(Direction dir)
{
    const static std::unordered_map<Direction, std::vector<glm::vec3>> facePosOffset =
    {
        {Direction::XNEG, {glm::vec3(0,0,0), glm::vec3(0,0,1), glm::vec3(0,1,1), glm::vec3(0,1,0)}},
        {Direction::XPOS, {glm::vec3(1,0,0), glm::vec3(1,0,1), glm::vec3(1,1,1), glm::vec3(1,1,0)}},
        {Direction::YNEG, {glm::vec3(0,0,0), glm::vec3(1,0,0), glm::vec3(1,0,1), glm::vec3(0,0,1)}},
        {Direction::YPOS, {glm::vec3(0,1,0), glm::vec3(1,1,0), glm::vec3(1,1,1), glm::vec3(0,1,1)}},
        {Direction::ZNEG, {glm::vec3(0,0,0), glm::vec3(1,0,0), glm::vec3(1,1,0), glm::vec3(0,1,0)}},
        {Direction::ZPOS, {glm::vec3(0,0,1), glm::vec3(1,0,1), glm::vec3(1,1,1), glm::vec3(0,1,1)}},
        {Direction::CROSS1, {glm::vec3(0,0,0), glm::vec3(1,0,1), glm::vec3(1,1,1), glm::vec3(0,1,0)}},
        {Direction::CROSS2, {glm::vec3(0,0,1), glm::vec3(1,0,0), glm::vec3(1,1,0), glm::vec3(0,1,1)}}
    };

    return facePosOffset.at(dir);
}
