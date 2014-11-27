#include "geodrawer.h"
#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>
#include "rootdrawer.h"

#include "coverage.h"
#include "layerdrawer.h"

GeoDrawer::GeoDrawer(QQuickItem *parent):
    QQuickItem(parent), _shaderprogram(0)
{
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
    _rootDrawer = new Ilwis::Geodrawer::RootDrawer(this);

    // for testing
    _rootDrawer->addDrawer(new Ilwis::Geodrawer::LayerDrawer("LayerDrawer",_rootDrawer,_rootDrawer));

}

GeoDrawer::~GeoDrawer()
{
    cleanup();
}

void GeoDrawer::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, SIGNAL(afterRendering()), this, SLOT(paint()), Qt::DirectConnection);
        connect(win, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);
//! [2]

        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.
//! [3]
       // win->setClearBeforeRendering(false);
    }
}

void GeoDrawer::paint()
{
    if ( !_rootDrawer)
        return;

    if ( !_rootDrawer->isActive())
        return ;

    int w = width();
    int h = height();
    _rootDrawer->rasterSize(Ilwis::Size<>(w,h,0));
    QPointF pnt(x(), y());
    QPointF pnt2 =mapFromItem(window()->contentItem(),pnt);
    glViewport(-pnt2.x(), -pnt2.y(), w, h);

    _rootDrawer->draw(window()->openglContext());

    glViewport(0,0,w, h);

}

void GeoDrawer::cleanup()
{
    if ( _rootDrawer){
        _rootDrawer->cleanUp();
    }
}
//! [6]

//! [9]
void GeoDrawer::sync()
{
}

void GeoDrawer::init() {
    _shaderprogram.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                            "attribute highp vec4 aVertices;"
                                            "uniform mat4 mvp;"
                                            "void main() {"
                                            "    gl_Position =  mvp * aVertices;"
                                            "}");
    _shaderprogram.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                            "void main() {"
                                            "    gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);"
                                            "}");





    _shaderprogram.bindAttributeLocation("aVertices", 0);
    if(!_shaderprogram.link()){
        qDebug() << _shaderprogram.log();
    }
    if (!_shaderprogram.bind()){
        qDebug() << _shaderprogram.log();
    }

    GLfloat triangle[] = {
        -2.0f, -2.0f, 0.0f,
        2.0f, -2.0f, 0.0f,
        0.0f,  2.0f, 0.0f,
    };
    _projection.ortho(-2.0, 2.0,-2.0,2.0, -1, 1);
    _mvp = _model * _view * _projection;
    _shaderprogram.setUniformValueArray("mvp",&_mvp,16);

    _shaderprogram.enableAttributeArray("mvp");

    window()->openglContext()->functions()->glGenBuffers (1, &vbo);
    window()->openglContext()->functions()->glBindBuffer (GL_ARRAY_BUFFER, vbo);
    window()->openglContext()->functions()->glBufferData (GL_ARRAY_BUFFER, sizeof (triangle), triangle, GL_STATIC_DRAW);


}

void GeoDrawer::test1() {
    _shaderprogram.bind();

    window()->openglContext()->functions()->glBindBuffer(GL_ARRAY_BUFFER,vbo);

    int vertexLocation = _shaderprogram.attributeLocation("aVertices");
    window()->openglContext()->functions()->glVertexAttribPointer(vertexLocation,3,GL_FLOAT,FALSE,0,0);
    window()->openglContext()->functions()->glEnableVertexAttribArray(vertexLocation);

   glDrawArrays(GL_LINE_STRIP,0,3);

   window()->openglContext()->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
   window()->openglContext()->functions()->glDisableVertexAttribArray(0);

  _shaderprogram.release();
}

//void GeoDrawer::dummmm2(){
//    if (!_shaderprogram) {

//        _shaderprogram = new QOpenGLShaderProgram();
//        _shaderprogram.addShaderFromSourceCode(QOpenGLShader::Vertex,
//                                                "attribute highp vec4 aVertices;"
//                                                "attribute highp vec4 aColors;"
//                                                //"varying highp vec4 vColors;"
//                                                "void main() {"
//                                                "    gl_Position = aVertices;"
//                                               // "    vColors= aColors;"
//                                                "}");
//        _shaderprogram.addShaderFromSourceCode(QOpenGLShader::Fragment,
//                                                //"varying highp vec4 vColors;"
//                                                "void main() {"
//                                                "    gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);"
//                                                "}");





//        _shaderprogram.bindAttributeLocation("aVertices", 0);
//        _shaderprogram.bindAttributeLocation("aColors", 1);
//        if(!_shaderprogram.link()){
//           qDebug() << _shaderprogram.log();
//        }
//        if (!_shaderprogram.bind()){
//           qDebug() << _shaderprogram.log();
//        }

//    }
//    GLfloat triangle[] = {
//        -1.0f, -1.0f, 0.0f,
//        1.0f, -1.0f, 0.0f,
//        0.0f,  1.0f, 0.0f,
//    };

//    GLuint vbo = 0;
//    window()->openglContext()->functions()->glGenBuffers (1, &vbo);
//    window()->openglContext()->functions()->glBindBuffer (GL_ARRAY_BUFFER, vbo);
//    window()->openglContext()->functions()->glBufferData (GL_ARRAY_BUFFER, sizeof (triangle), triangle, GL_STATIC_DRAW);

//    int vertexLocation = _shaderprogram.attributeLocation("aVertices");
//    window()->openglContext()->functions()->glVertexAttribPointer(vertexLocation,3,GL_FLOAT,FALSE,0,0);
//    window()->openglContext()->functions()->glEnableVertexAttribArray(vertexLocation);
//    //window()->openglContext()->functions()->glBindBuffer(GL_ARRAY_BUFFER,vbo);

//   glDrawArrays(GL_TRIANGLES,0,3);

//   window()->openglContext()->functions()->glBindBuffer(GL_ARRAY_BUFFER, 0);
//   window()->openglContext()->functions()->glDisableVertexAttribArray(0);
//   qDebug() << glGetError();

//   _shaderprogram.release();
//   delete _shaderprogram;
//   _shaderprogram = 0;


//}
