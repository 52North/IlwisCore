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

class DrawerInterface;

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
    Q_INVOKABLE void addDrawer(const QString& drawercode, const QVariantMap& properties);
    Q_INVOKABLE void setAttribute(const QString &drawercode, const QVariantMap& value);
    Q_INVOKABLE void removeDrawer(const QString& namecode, bool ascode);
    Q_INVOKABLE double viewWidth() const;
    Q_INVOKABLE double viewHeight() const;
    ~GeoDrawer();

    //QSGNode *updatePaintNode(QSGNode *node, UpdatePaintNodeData *);
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

