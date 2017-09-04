#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QDir>
#include <QVector>
#include "identity.h"
#include "kernel.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "inifile.h"
#include "resource.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "ilwis3connector.h"
#include "catalogconnector.h"
#include "catalog.h"
#include "odfitem.h"
#include "ilwiscontext.h"
#include "dataformat.h"
#include "filecatalogconnector.h"
#include "ilwis3catalogconnector.h"

using namespace Ilwis;
using namespace Ilwis3;

ConnectorInterface *Ilwis3CatalogConnector::create(const Resource &resource, bool) {
    if ( resource.ilwisType() == itCATALOG ){
        return new Ilwis3CatalogConnector(resource);
    }
    return nullptr;

}

Ilwis3CatalogConnector::Ilwis3CatalogConnector(const Resource &resource) : FileCatalogConnector(resource)
{
}

inline uint qHash(const QFileInfo& inf ){
    return ::qHash(inf.canonicalFilePath());
}

bool Ilwis3CatalogConnector::loadItems()
{
    if (!isValid())
        return false;


    QVariantList filters = DataFormat::getFormatProperties(DataFormat::fpEXTENSION, itILWISOBJECT,"ilwis3");
    QStringList sfilters;
    for(QVariant& ext : filters)
        sfilters += "*." + ext.toString();
    std::vector<QUrl> files = sources(sfilters,CatalogConnector::foFULLPATHS | CatalogConnector::foEXTENSIONFILTER);

    QList<ODFItem> odfitems;
    QList<Resource> folders;
    QHash<QString, quint64> names;
    foreach(const QUrl& url, files) {
        QFileInfo file = containerConnector()->toLocalFile(url);
        IlwisTypes tp = Ilwis3Connector::ilwisType(file.fileName());
        if ( mastercatalog()->url2id(url, tp) == i64UNDEF) {
            if ( tp & itILWISOBJECT ) {
                ODFItem item(url, containerConnector());
                odfitems.push_back(item);
                names[url.toString().toLower()] = item.id();
            } /*else {
                folders.push_back(loadFolder(file, container, path, url));
            }*/
        }

    }

    bool ok = true;


    QList<Resource> finalList;
    for(int i = 0; i < odfitems.size(); ++i) {
        ODFItem &item = odfitems[i];
        ok = item.resolveNames(names);


        if ( ok) {
            finalList.push_back(item);
        }
    }
    mastercatalog()->addItems(finalList);
    mastercatalog()->addItems(folders);


    return ok;
}

bool Ilwis3CatalogConnector::canUse(const Resource &resource) const
{
        if ( resource.ilwisType() != itCATALOG)
            return false;
        if (resource.url().scheme() == "ilwis")
            return false;
        return true;
}

QString Ilwis3CatalogConnector::provider() const
{
    return "ilwis3";
}






