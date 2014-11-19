#ifndef GEODRAWER_H
#define GEODRAWER_H

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>

namespace Ilwis {
namespace Geodrawer{
class RootDrawer;
}
}
class GeoDrawer : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(GeoDrawer)

public:
    GeoDrawer(QQuickItem *parent = 0);
    ~GeoDrawer();

public slots:
    void paint();
    void cleanup();
    void sync();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    QOpenGLShaderProgram *_shaderprogram = 0;
    QMatrix4x4 _view,_projection, _model, _mvp;
    Ilwis::Geodrawer::RootDrawer *_rootDrawer = 0;

    void oldTest();
};

#endif // GEODRAWER_H

