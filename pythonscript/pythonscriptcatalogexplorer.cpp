#include "kernel.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "catalogexplorer.h"
#include "foldercatalogexplorer.h"
#include "pythonscriptcatalogexplorer.h"

using namespace Ilwis;
using namespace PythonScript;

REGISTER_CATALOGEXPLORER(PythonScriptCatalogExplorer)

Ilwis::PythonScript::PythonScriptCatalogExplorer::PythonScriptCatalogExplorer(const Ilwis::Resource &resource, const IOOptions &options) :
    CatalogExplorer(resource, options)
{

}

std::vector<Ilwis::Resource> PythonScriptCatalogExplorer::loadItems(const IOOptions &options)
{
    QStringList sfilters;
    sfilters << "*.py" ;
    std::vector<QUrl> files = FolderCatalogExplorer::loadFolders(source(),
                                                                 sfilters,
                                                                 CatalogConnector::foFULLPATHS | CatalogConnector::foEXTENSIONFILTER);

    kernel()->issues()->silent(true);
    std::vector<Resource> resources;
    for(const auto& file : files){
        if(!toLocalFile(file).isDir()){
            Resource res(file, itSCRIPT);
            resources.push_back(res);
        }
    }

     kernel()->issues()->log(QString(TR("Added %1 objects through the spreadsheet connector")).arg( resources.size()),IssueObject::itMessage);

     kernel()->issues()->silent(false);

     return resources;
}

bool PythonScriptCatalogExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if ( resource.url().scheme() != "file") // can't use anything marked as file
        return false;
    if (resource.url().scheme() == "ilwis")
        return false;
    if ( QFileInfo(resource.url(true).toLocalFile()).isFile()) // no file can be an py folder catalog.
        return false;
    return true;

}

QString PythonScriptCatalogExplorer::provider() const
{
    return "python";
}

QFileInfo PythonScriptCatalogExplorer::toLocalFile(const QUrl &datasource) const
{
    return QFileInfo(datasource.toLocalFile());
}

Ilwis::CatalogExplorer *PythonScriptCatalogExplorer::create(const Resource &resource, const IOOptions &options)
{
    return new PythonScriptCatalogExplorer(resource, options) ;
}

