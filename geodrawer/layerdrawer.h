#ifndef LAYERDRAWER_H
#define LAYERDRAWER_H

#include <QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include "spatialdatadrawer.h"

class QOpenGLShaderProgram;

namespace Ilwis {
namespace Geodrawer{

class DrawingColor;
typedef std::unique_ptr<DrawingColor> UPDrawingColor;

class LayerDrawer : public SpatialDataDrawer
{
public:
    LayerDrawer(const QString& name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer);

    bool prepare(PreparationType prepType, const IOOptions& options,QOpenGLContext *openglContext=0);
    void setCoverage(const ICoverage& coverage);

    UPDrawingColor& drawingColor();

     void cleanUp();
protected:
    bool initGeometry(QOpenGLContext *openglContext, const std::vector<VertexPosition>& vertices, const std::vector<VertexColor> &colors);
    GLuint _vboPosition;
    GLuint _vboColor;
    QOpenGLShaderProgram _shaders;

private:
    UPDrawingColor _drawingColor;


    bool initShaders();

};


}
}

#endif // LAYERDRAWER_H
