#ifndef GEODRAWER_H
#define GEODRAWER_H

#include <QtQuick/QQuickFramebufferObject>
#include "kernel.h"
#include <deque>
#include "visualizationmanager.h"

struct DrawerIdTag {
    DrawerIdTag(const Resource& resource) : _resource(resource){}
    DrawerIdTag(const QString& name, bool ascode) : _drawerName(name),_asCode(ascode){}
    Resource _resource;
    quint64 _drawerid = iUNDEF;
    QString _drawerName;
    QString _drawerCode;
    bool _asCode;
};

class GeoRenderer;

class GeoDrawer : public QQuickFramebufferObject
{

    Q_OBJECT
public:
    friend class GeoRenderer;

    Q_INVOKABLE void addDataSource(const QString& url, const QString& typeName);
    //Q_INVOKABLE void addDrawer(const QString& drawercode, const QVariantMap& properties);
    Q_INVOKABLE void setAttribute(const QString &drawercode, const QVariantMap& value);
    Q_INVOKABLE void removeDrawer(const QString& namecode, bool ascode);




    Renderer *createRenderer() const;

private:
    std::vector<DrawerIdTag> _datasources;
    std::deque<DrawerIdTag> _removedDrawers;
    std::deque<std::pair<QString, QVariantMap>> _attributeQueue;
};

#endif // GEODRAWER_H

