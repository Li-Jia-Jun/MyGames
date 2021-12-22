#include "uiinventory.h"
#include "unordered_map"


UIInventory::UIInventory(OpenGLContext *context) : Drawable(context)
{}

void UIInventory::createVBOdata()
{
    // TODO::
    //  1. Create a row of item frames at screen bottom
    //  2. add item image according to stored block type
    //  3. Add num image accridubg to stored nums
}

void UIInventory::setData(std::vector<BlockType> types, std::vector<int> nums)
{
    m_types = types;
    m_nums = nums;
}
