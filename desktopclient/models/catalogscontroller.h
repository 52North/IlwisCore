#ifndef CATALOGSCONTROLLER_H
#define CATALOGSCONTROLLER_H

#include <QQmlContext>
#include "kernel.h"
#include "catalog.h"
#include "mastercatalog.h"
#include "ilwiscontext.h"
#include "resourcemodel.h"

namespace Ilwis {
namespace Desktopclient {


class CatalogsController : public QObject
{
    Q_OBJECT
public:
    CatalogsController();
    ~CatalogsController();
    void changeCatalogForward( QQmlContext *qmlcontext , const QString& id);
    void changeCatalogBackward( QQmlContext *qmlcontext, const QString &sid);

private:
    Ilwis::Resource getResource(const QString &sid);
    void openCatalog( QQmlContext *qmlcontext, const Ilwis::Resource &resource);
    void getResourceList(const Ilwis::Resource &resource, const QString &query, QList<QObject *> &list);
    void makePathList(const Ilwis::Resource& resource)    ;
    bool _isInitial;
    bool _catalog1IsActive;

     QList<QObject *> pathList;
     QList<QObject *> catalogList1;
     QList<QObject *> catalogList2;
};
}
}

#endif // CATALOGSCONTROLLER_H
