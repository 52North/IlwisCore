#include "coverage.h"
#include "layerdrawer.h"
#include "drawingcolor.h"
#include <QtGui/QOpenGLContext>
#include "rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

LayerDrawer::LayerDrawer(const QString &name, DrawerInterface *parentDrawer, RootDrawer *rootdrawer) : SpatialDataDrawer(name, parentDrawer, rootdrawer)
{
    valid(true); //TODO: this must move to the prepare once the correct call sequences is in place
 }

bool LayerDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options, QOpenGLContext *openglContext)
{

    if(!initShaders())
        return false;
    if(!initGeometry(openglContext))
        return false;

    //_shaders.setUniformValue("mvp",*(rootDrawer()->mvpMatrix()));
    _prepared |= DrawerInterface::ptGEOMETRY;
    return SpatialDataDrawer::prepare(prepType, options);

}

bool LayerDrawer::draw(QOpenGLContext *openglContext, const IOOptions& options) {
    if ( !openglContext){
        return ERROR2(QString("%1 : %2"),TR("Drawing failed"),TR("Invalid OpenGL context passed"));
    }

    if ( !isActive())
        return false;
    if (!isPrepared(DrawerInterface::ptGEOMETRY)){
        if (!prepare(DrawerInterface::ptGEOMETRY, options, openglContext)){
            return false;
        }
    }

    _shaders.bind();

    openglContext->functions()->glBindBuffer(GL_ARRAY_BUFFER,_vbo);

    int vertexLocation = _shaders.attributeLocation("aVertices");
    openglContext->functions()->glVertexAttribPointer(vertexLocation,3,GL_FLOAT,FALSE,0,0);
    openglContext->functions()->glEnableVertexAttribArray(vertexLocation);

   glDrawArrays(GL_LINE_STRIP,0,3);

   openglContext->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
   openglContext->functions()->glDisableVertexAttribArray(0);

   _shaders.release();

   return true;
}

bool LayerDrawer::initShaders() {

    _shaders.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                            "attribute highp vec4 aVertices;"
                                            "uniform mat4 mvp;"
                                            "void main() {"
                                            "    gl_Position =  mvp * aVertices;"
                                            "}");
    _shaders.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                            "void main() {"
                                            "    gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);"
                                            "}");





    _shaders.bindAttributeLocation("aVertices", 0);
    if(!_shaders.link()){
       return ERROR2(QString("%1 : %2"),TR("Drawing failed"),TR(_shaders.log()));
    }
    if (!_shaders.bind()){
        return ERROR2(QString("%1 : %2"),TR("Drawing failed"),TR(_shaders.log()));
    }

    return true;
}

bool LayerDrawer::initGeometry(QOpenGLContext *openglContext) {
    if ( !openglContext){
        return ERROR2(QString("%1 : %2"),TR("Drawing failed"),TR("Invalid OpenGL context passed"));
    }

    _shaders.setUniformValue("mvp",rootDrawer()->mvpMatrix());

    _shaders.enableAttributeArray("mvp");

    VertexPosition triangle[] = {
        {-2.0f, -2.0f, 0.0f},
        {2.0f, -2.0f, 0.0f},
        {0.0f,  2.0f, 0.0f},
    };

    openglContext->functions()->glGenBuffers (1, &_vbo);
    openglContext->functions()->glBindBuffer (GL_ARRAY_BUFFER, _vbo);
    openglContext->functions()->glBufferData (GL_ARRAY_BUFFER, sizeof (VertexPosition) * 3, triangle, GL_STATIC_DRAW);
    GLenum err =  glGetError();
    if ( err != 0) {
        return ERROR1(QString(TR("Drawing failed : OpenGL returned error code %1")),QString::number(err));
    }
    return true;
}



void LayerDrawer::setCoverage(const ICoverage &coverage)
{
    SpatialDataDrawer::setCoverage(coverage);
    // TODO initialize drawing color here
    _drawingColor.reset(new DrawingColor(this));
}

UPDrawingColor &LayerDrawer::drawingColor()
{
    return _drawingColor;
}

void LayerDrawer::cleanUp()
{
}


