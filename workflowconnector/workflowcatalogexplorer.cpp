#include "kernel.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "catalogexplorer.h"
#include "foldercatalogexplorer.h"
#include "Workflowcatalogexplorer.h"

using namespace Ilwis;
using namespace WorkflowConnector;

REGISTER_CATALOGEXPLORER(WorkflowCatalogExplorer)

WorkflowCatalogExplorer::WorkflowCatalogExplorer(const Ilwis::Resource &resource, const IOOptions &options) : CatalogExplorer(resource, options)
{

}

std::vector<Ilwis::Resource> WorkflowCatalogExplorer::loadItems(const IOOptions &options)
{
    QStringList sfilters;
    sfilters << "*.workflow";
    std::vector<QUrl> files = FolderCatalogExplorer::loadFolders(source(),
                                                                 sfilters,
                                                                 CatalogConnector::foFULLPATHS | CatalogConnector::foEXTENSIONFILTER);

    kernel()->issues()->silent(true);
    std::vector<Resource> resources;
    for(const auto& file : files){
    }

     kernel()->issues()->log(QString(TR("Added %1 objects through the Workflow connector")).arg( resources.size()),IssueObject::itMessage);

     kernel()->issues()->silent(false);

     return resources;
}

bool WorkflowCatalogExplorer::canUse(const Resource &resource) const
{
    if ( resource.ilwisType() != itCATALOG)
        return false;
    if ( resource.url().scheme() != "file") // can't use anything marked as file
        return false;
    if (resource.url().scheme() == "ilwis")
        return false;
    return true;

}

QString WorkflowCatalogExplorer::provider() const
{
    return "Workflows";
}

QFileInfo WorkflowCatalogExplorer::toLocalFile(const QUrl &datasource) const
{
    return QFileInfo(datasource.toLocalFile());
}


Ilwis::CatalogExplorer *WorkflowCatalogExplorer::create(const Resource &resource, const IOOptions &options)
{
    return new WorkflowCatalogExplorer(resource, options) ;
}

