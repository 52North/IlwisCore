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
    LayerDrawer(const QString &name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer);

    bool prepare(PreparationType prepType, const IOOptions& options);
    void setCoverage(const ICoverage& coverage);

    UPDrawingColor& drawingColor();

     void cleanUp();

private:
    UPDrawingColor _drawingColor;
    QOpenGLShaderProgram *_shaderprogram;
    QOpenGLShaderProgram _shaders;
    GLuint _vboID[2];

    bool draw(const IOOptions &options=IOOptions()) ;
    void dummmm();
    bool initGeometry();
    bool initShaders();
};


}
}

#endif // LAYERDRAWER_H
