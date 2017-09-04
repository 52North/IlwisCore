#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QVector>
#include "identity.h"
#include "kernel.h"
#include "resource.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "catalogexplorer.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalog.h"
#include "dataformat.h"
#include "mastercatalog.h"
#include "inifile.h"
#include "ilwis3catalogfileexplorer.h"

using namespace Ilwis;
using namespace Ilwis3;

REGISTER_CATALOGEXPLORER(Ilwis3CatalogFileExplorer)

Ilwis::CatalogExplorer *Ilwis3CatalogFileExplorer::create(const Resource &resource, const IOOptions &options)
{
    return new Ilwis3CatalogFileExplorer(resource, options);
}

Ilwis3CatalogFileExplorer::Ilwis3CatalogFileExplorer(const Ilwis::Resource &resource, const IOOptions &options) : CatalogExplorer(resource, options)
{
}
std::vector<Ilwis::Resource> Ilwis3CatalogFileExplorer::loadItems(const IOOptions &)
{
    QFileInfo containerInf = source().toLocalFile();
    if ( containerInf.isDir()){
        ERROR2(ERR_COULD_NOT_CONVERT_2,"container", "file" );
        return std::vector<Ilwis::Resource>();
    }
    std::vector<Ilwis::Resource> result;
    IniFile odf(containerInf);
    int nrOfMaps = odf.value("MapList","Maps").toInt();
    for(int i=0; i < nrOfMaps; ++i){
        QString mp = odf.value("MapList","Map" + IniFile::FormatElement(i));
        QString fullpath = containerInf.absolutePath() + "\\" + mp;
        QUrl url = QUrl::fromLocalFile(fullpath);
        Resource res = mastercatalog()->name2Resource(url.toString(), itRASTER);
        if ( res.isValid())
            result.push_back(res);
    }

    return result;
}

bool Ilwis3CatalogFileExplorer::canUse(const Resource &resource) const
{
    if ( hasType(resource.extendedType(),itCATALOG))
        return false;
    if (resource.url().scheme() == "ilwis")
        return false;
    QFileInfo fileInfo(resource.url(true).toLocalFile());
    if ( fileInfo.isFile()) {
        return fileInfo.suffix() == "mpl";
    }
    return false;
}

QString Ilwis3CatalogFileExplorer::provider() const
{
    return "ilwis3";
}

QFileInfo Ilwis3CatalogFileExplorer::toLocalFile(const QUrl &datasource) const
{
    return QFileInfo(datasource.toLocalFile());
}

CatalogExplorer::ExplorerType Ilwis3CatalogFileExplorer::explorerType() const
{
    return etIMPLICIT;
}




