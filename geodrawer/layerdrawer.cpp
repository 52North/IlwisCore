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

bool LayerDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options)
{
    _shaderprogram = new QOpenGLShaderProgram();

    if(!initShaders())
        return false;
    if(!initGeometry())
        return false;

    _shaders.setUniformValue("mvp",*(rootDrawer()->mvpMatrix()));
    int err1 = glGetError();
    _prepared |= DrawerInterface::ptGEOMETRY;
    return SpatialDataDrawer::prepare(prepType, options);

}

bool LayerDrawer::draw(const IOOptions& options) {
    if ( !isActive())
        return false;
    if (!isPrepared(DrawerInterface::ptGEOMETRY)){
        if (!prepare(DrawerInterface::ptGEOMETRY, options)){
            return false;
        }
    }



    glBindBuffer(GL_ARRAY_BUFFER,_vboID[0]);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboID[1]);

    int vertexlocation = _shaders.attributeLocation("aVertices");
    int err2 = glGetError();
    _shaders.enableAttributeArray(vertexlocation);
    int err3 = glGetError();
    glVertexAttribPointer(vertexlocation,3, GL_FLOAT,GL_FALSE,0,0);
    int err4 = glGetError();
    //glDrawElements(GL_LINE,6,GL_UNSIGNED_SHORT,0);
    glDrawArrays(GL_LINE_STRIP,0,6);
    int err5 = glGetError();
    _shaders.disableAttributeArray(vertexlocation);
    glBindBuffer(GL_ARRAY_BUFFER,0);

    int err6 = glGetError();
    qDebug() << err2 << err3 << err4 << err5 << err6;

   // dummmm();


    return true;
}

bool LayerDrawer::initShaders() {
    setlocale(LC_NUMERIC, "C");

    _shaders.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                            "attribute highp vec4 aVertices;"
                                            //"attribute highp vec4 aColors;"
                                           // "varying highp vec4 vColors;"
                                            "uniform mat4 mvp;"
                                            "void main() {"
                                            "    gl_Position = mvp * aVertices;"
                                           // "    vColors= aColors;"
                                            "}");
    _shaders.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                            "varying highp vec4 vColors;"
                                            "void main() {"
                                            "    gl_FragColor = vec4(0,0,0,1.0);"
                                            "}");
    if (!_shaders.link()){
        return false;
    }

    if (!_shaders.bind()){
        return false;
    }

    setlocale(LC_ALL, "");

    return true;
}

bool LayerDrawer::initGeometry() {
    initializeOpenGLFunctions();
    glGenBuffers(2, _vboID);

    VertexPosition vertices[] = {
        {-200, -1}, //Diag bottom left to top right
        {2, 1},
        {-200, 1}, //Diag top left to bottom right
        {2, -1},
        {-200, 0}, //Horizontal line
        {2, 0}
    };

    GLushort indices[] = { 0,1,2,3,4,5,6};
    float colors[] = {
        1, 1, 0, 1,
        1, 0, 1, 1,
        0, 1, 1, 1,
        1, 0, 0, 1,
        0, 0, 1, 1,
        0, 1, 0, 1
    };

    glBindBuffer(GL_ARRAY_BUFFER, _vboID[0]);
    glBufferData(GL_ARRAY_BUFFER,6 * sizeof(VertexPosition), vertices, GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vboID[1]);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLushort),indices, GL_STATIC_DRAW);

    return true;


}

void LayerDrawer::dummmm(){
    if (!_shaderprogram) {

        _shaderprogram = new QOpenGLShaderProgram();
        _shaderprogram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                                "attribute highp vec4 aVertices;"
                                                "attribute highp vec4 aColors;"
                                                "varying highp vec4 vColors;"
                                                "uniform mat4 mvp;"
                                                "void main() {"
                                                "    gl_Position = mvp * aVertices;"
                                                "    vColors= aColors;"
                                                "}");
        _shaderprogram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                                "varying highp vec4 vColors;"
                                                "void main() {"
                                                "    gl_FragColor = vColors;"
                                                "}");





        _shaderprogram->bindAttributeLocation("aVertices", 0);
        _shaderprogram->bindAttributeLocation("aColors", 1);
        _shaderprogram->link();

    }

    _shaderprogram->bind();
    _shaderprogram->setUniformValueArray("mvp",rootDrawer()->mvpMatrix(),16);

    _shaderprogram->enableAttributeArray(0);
    _shaderprogram->enableAttributeArray(1);
    _shaderprogram->enableAttributeArray("mvp");

    VertexPosition vertices[] = {
        {-200, -1}, //Diag bottom left to top right
        {2, 1},
        {-200, 1}, //Diag top left to bottom right
        {2, -1},
        {-200, 0}, //Horizontal line
        {2, 0}
    };
    float colors[] = {
        1, 1, 0, 1,
        1, 0, 1, 1,
        0, 1, 1, 1,
        1, 0, 0, 1,
        0, 0, 1, 1,
        0, 1, 0, 1
    };


    _shaderprogram->setAttributeArray(0, GL_FLOAT, vertices, 3); //3rd to 0, 4th to 1 by default
    _shaderprogram->setAttributeArray(1, GL_FLOAT, colors, 4);


    glDisable(GL_DEPTH_TEST);

   // glClear(GL_COLOR_BUFFER_BIT);

    //    //Here I draw 3 lines, reduce to 2 instead of 6 to draw only one.
    //    //Change second param to choose which line to draw
    glDrawArrays(GL_LINES, 0, 6);


    _shaderprogram->disableAttributeArray(0);
    _shaderprogram->disableAttributeArray(1);
    _shaderprogram->release();
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
    if (_shaderprogram) {
        delete _shaderprogram;
        _shaderprogram = 0;
    }
}


