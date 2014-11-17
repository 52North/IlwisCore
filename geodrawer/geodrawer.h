#ifndef GEODRAWER_H
#define GEODRAWER_H

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>

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
    QOpenGLShaderProgram *_shaderprogram;
};

#endif // GEODRAWER_H

