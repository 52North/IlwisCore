#include "kernel.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "connectorfactory.h"
#include "catalogconnectorfactory.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "connectorinterface.h"
#include "catalogconnector.h"
#include "ilwiscontext.h"
#include "domain.h"
#include "dataformat.h"
#include "postgresqlobjectfactory.h"
#include "postgresqlconnector.h"
#include "postgresqlcatalogexplorer.h"
#include "postgresqlfeatureconnector.h"
#include "postgresqlrasterconnector.h"
#include "postgresqltableconnector.h"
#include "postgresqlmodule.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlModule::PostgresqlModule(QObject *parent) :
    Module(parent, "PostgresqlModule", "iv40","1.0")
{
}

PostgresqlModule::~PostgresqlModule()
{
}

QString PostgresqlModule::getInterfaceVersion() const
{
      return "iv40";
}

QString PostgresqlModule::getName() const
{
    return "PostgreSQL plugin";
}

QString PostgresqlModule::getVersion() const
{
    return "1.0";
}

void PostgresqlModule::prepare()
{
    PostgresqlObjectFactory *factory = new PostgresqlObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory);
    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return ;
    cfactory->addCreator("table", "postgresql", PostgresqlTableConnector::create);
    cfactory->addCreator("simplefeatures", "postgresql", PostgresqlFeatureConnector::create);
    cfactory->addCreator("map", "postgresql", PostgresqlRasterConnector::create);
    cfactory->addCreator(itTABLE, "postgresql", PostgresqlTableConnector::create);
    cfactory->addCreator(itFEATURE, "postgresql", PostgresqlFeatureConnector::create);
    cfactory->addCreator(itRASTER, "postgresql", PostgresqlRasterConnector::create);
    cfactory->addCreator(itCATALOG, "postgresql", CatalogConnector::create);

    QFileInfo ilw = context()->ilwisFolder();
    QString path = ilw.canonicalFilePath() + "/extensions/postgresqlconnector/resources";
    DataFormat::setFormatInfo(path + "/postgresformats.config","postgresql");

    kernel()->issues()->log("Loaded Postgresql Module", IssueObject::itMessage);
}
