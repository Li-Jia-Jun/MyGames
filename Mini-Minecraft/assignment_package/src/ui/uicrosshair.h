#ifndef UICROSSHAIR_H
#define UICROSSHAIR_H

#include "drawable.h"

class UICrosshair : public Drawable
{
public:
    UICrosshair(OpenGLContext *context);

    virtual GLenum drawMode() override;

    virtual void createVBOdata() override;
};

#endif // UICROSSHAIR_H
