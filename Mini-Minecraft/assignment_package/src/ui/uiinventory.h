#ifndef UIINVENTORY_H
#define UIINVENTORY_H


#include "drawable.h"
#include "blockType.h"
//#include <vector>

class UIInventory : public Drawable
{
public:
    UIInventory(OpenGLContext *context);

    virtual void createVBOdata() override;

    void setData(std::vector<BlockType> types, std::vector<int> nums);

private:
    std::vector<BlockType> m_types;
    std::vector<int> m_nums;
};

#endif // UIINVENTORY_H
