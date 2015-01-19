#ifndef GEODRAWER_H
#define GEODRAWER_H

#include "kernel.h"
#include "resourcemodel.h"
//#include "models/visualizationmanager.h"
#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>

namespace Ilwis {
class Resource;


namespace Geodrawer{
class RootDrawer;
}
}
class VisualizationManager;

class GeoDrawer : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(GeoDrawer)

public:
    GeoDrawer(QQuickItem *parent = 0);

    Q_INVOKABLE void addDataSource(const QString& url, const QString& typeName, VisualizationManager* manager);
    ~GeoDrawer();

public slots:
    void paint();
    void cleanup();
    void sync();

signals:
    void layerChanged();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    Ilwis::Geodrawer::RootDrawer *_rootDrawer = 0;
    QList<ResourceModel *> _datasources;

    void test1();
};

#endif // GEODRAWER_H

