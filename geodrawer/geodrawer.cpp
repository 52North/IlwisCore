#include "geodrawer.h"
#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>

GeoDrawer::GeoDrawer(QQuickItem *parent):
    QQuickItem(parent), _shaderprogram(0)
{
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), this, SLOT(handleWindowChanged(QQuickWindow*)));
}

GeoDrawer::~GeoDrawer()
{
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
    if (!_shaderprogram) {
        _shaderprogram = new QOpenGLShaderProgram();
        _shaderprogram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                           "attribute highp vec4 aVertices;"
                                           "attribute highp vec4 aColors;"
                                           "varying highp vec4 vColors;"
                                           "void main() {"
                                           "    gl_Position = aVertices;"
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

    _shaderprogram->enableAttributeArray(0);
    _shaderprogram->enableAttributeArray(1);

    float vertices[] = {
        -1, -1, //Diag bottom left to top right
        1, 1,
        -1, 1, //Diag top left to bottom right
        1, -1,
        -1, 0, //Horizontal line
        1, 0
    };
    float colors[] = {
        1, 1, 0, 1,
        1, 0, 1, 1,
        0, 1, 1, 1,
        1, 0, 0, 1,
        0, 0, 1, 1,
        0, 1, 0, 1
    };
    _shaderprogram->setAttributeArray(0, GL_FLOAT, vertices, 2); //3rd to 0, 4th to 1 by default
    _shaderprogram->setAttributeArray(1, GL_FLOAT, colors, 4);

    int w = width();
    QPointF pnt(x(), y());
    QPointF pnt2 =mapFromItem(window()->contentItem(),pnt);
    glViewport(-pnt2.x(), -pnt2.y(), width(), height());

    glDisable(GL_DEPTH_TEST);

    //glClear(GL_COLOR_BUFFER_BIT);

//    //Here I draw 3 lines, reduce to 2 instead of 6 to draw only one.
//    //Change second param to choose which line to draw
    glDrawArrays(GL_LINES, 0, 6);
    w = width();
    glViewport(0,0,w, height());

    _shaderprogram->disableAttributeArray(0);
    _shaderprogram->disableAttributeArray(1);
    _shaderprogram->release();
}

void GeoDrawer::cleanup()
{
    if (_shaderprogram) {
        delete _shaderprogram;
        _shaderprogram = 0;
    }
}
//! [6]

//! [9]
void GeoDrawer::sync()
{
}

