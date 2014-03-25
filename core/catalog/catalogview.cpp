#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlRecord>
#include "identity.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "resource.h"
#include "catalogview.h"
#include "catalogquery.h"
#include "ilwiscontext.h"
#include "mastercatalog.h"

using namespace Ilwis;

CatalogView::CatalogView(QObject *parent) :
    QObject(parent)
{
}

CatalogView::CatalogView(const CatalogView &cat) : QObject(),
    Identity(cat.name(),cat.id(),cat.code(),cat.description()),
    _filter(cat._filter),
    _location(cat._location),
    _parent(cat._parent)
{
}

std::vector<Resource> CatalogView::items() const
{
    return mastercatalog()->select(_location, _filter);
}



bool CatalogView::prepare(const QUrl &resource, const QString& filter)
{

    bool ok = mastercatalog()->addContainer(resource);
    if (!ok)
        return false;

    _location = resource;
    CatalogQuery query;
    _filter = query.transformQuery(filter);

    QStringList parts = resource.path().split("/");
    QString cid = parts.back();

    Identity::prepare();

    setName(cid);

    return true;
}

QString CatalogView::type() const
{
    return "CatalogView";
}


bool CatalogView::isValid() const
{
    return _location.isValid(); // && _filter != "";
}

QUrl CatalogView::parentCatalogView() const
{
    return _parent;
}

void CatalogView::setParentCatalogView(const QUrl &url)
{
    _parent = url;
}















