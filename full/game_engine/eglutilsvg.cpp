#ifdef USE_OPENVG
#include <VG/openvg.h>
#endif
#include <QDebug>
#include <QString>
#include <QImage>

#include "eglutilsvg.h"
#include <stdio.h>

EGLUtilsVG::EGLUtilsVG() : EglUtils()
{
}

EGLUtilsVG::~EGLUtilsVG()
{
}

void EGLUtilsVG::clear(float r, float g, float b)
{
    VGfloat color[4] = { r, g, b, 1.0f };
    vgSetfv(VG_CLEAR_COLOR, 4, color);
    vgClear(0, 0, 1024, 1024);
}

int EGLUtilsVG::loadTexture(const char *fn)
{
    char testr[256];
    sprintf(testr, ":/%s", fn );
    QString filename = QString( testr );
    QImage image = QImage( filename );

    int w=image.width();
    int h=image.height();

    qDebug() << filename << "w:" << w << "h:" << h;

    VGImage texture = vgCreateImage(VG_sARGB_8888, w, h, VG_IMAGE_QUALITY_BETTER);
    qDebug("vgCreateImage handle %x", texture);
    if(texture==VG_INVALID_HANDLE)
    {
        VGErrorCode err = vgGetError();
        qDebug("loadVGImage '%s' error 0x%x", fn, err);
    }

    vgImageSubData(texture, (QRgb*)image.bits(), w*4, VG_sARGB_8888, 0, 0, w, h);
    return texture;
}

void EGLUtilsVG::releaseTexture(int textureHandle)
{
    if(textureHandle) vgDestroyImage(textureHandle);
}
