#include <QSqlQuery>
#include <QStringList>
#include <QSqlError>
#include <QHash>

#include "kernel.h"
#include "ilwiscontext.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "connectorfactory.h"
#include "spreadsheettableconnector.h"
#include "spreadsheetconnectorsobjectfactory.h"

using namespace Ilwis;
using namespace SpreadSheetConnectors;

SpreadSheetConnectorsObjectFactory::SpreadSheetConnectorsObjectFactory() : IlwisObjectFactory("IlwisObjectFactory","SpreadSheetConnectors","")
{
}


bool SpreadSheetConnectorsObjectFactory::canUse(const Resource &resource) const
{
    if ( resource.url().scheme() == "ilwis") // can't use anything marked as internal
        return false;

    if ( resource.url().scheme() != "file") // can't use anything marked as file
        return false;

    if ( !hasType(resource.ilwisType(), itTABLE))
        return false;

    QString filename = resource.url(true).toLocalFile();
    if ( filename == "")
        return false;

    QFileInfo info(resource.container(true).toLocalFile()); // possible case that the container is a gdal catalog
    if (info.isFile() && SpreadSheetTableConnector::knownSuffix(info.suffix()))
        return true;
    else {
        QFileInfo info (resource.toLocalFile());
        return SpreadSheetTableConnector::knownSuffix(info.suffix());
    }
    return false;
}

IlwisObject *SpreadSheetConnectorsObjectFactory::create(const Resource &resource, const IOOptions &options) const
{
    const ConnectorFactory *factory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    QFileInfo inf(resource.toLocalFile());
    QFileInfo container(inf.absolutePath());
    IlwisObjectConnector *connector = 0;

    if ( SpreadSheetTableConnector::knownSuffix(inf.suffix()) || SpreadSheetTableConnector::knownSuffix(container.suffix()))
        connector = factory->createFromResource<IlwisObjectConnector>(resource, "spreadsheets", options);

   if(!connector) {
       kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",resource.name()));
       return 0;
   }
   IlwisObject *object = createObject(connector, options);
   if ( object)
       return object;

   delete connector;
   return 0;
}



