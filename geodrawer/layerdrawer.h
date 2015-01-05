#ifndef LAYERDRAWER_H
#define LAYERDRAWER_H

#include <QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include "colorlookup.h"
#include "representation.h"
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
    void unprepare(PreparationType prepType);

    void cleanUp();
    QString activeAttribute() const;

protected:
    bool initGeometry(QOpenGLContext *openglContext, const std::vector<VertexPosition>& vertices, const std::vector<VertexColor> &colors);
    virtual void setActiveAttribute(const QString& attr);
    GLuint _vboPosition;
    GLuint _vboColor;
    QOpenGLShaderProgram _shaders;

private:
    QString _activeAttribute = sUNDEF;


    bool initShaders();

};


}
}

#endif // LAYERDRAWER_H
