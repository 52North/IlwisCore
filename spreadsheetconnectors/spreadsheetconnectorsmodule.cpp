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
#include "spreadsheetconnectorsobjectfactory.h"
#include "connectorinterface.h"
#include "ilwiscontext.h"
#include "spreadsheettableconnector.h"
#include "spreadsheetconnectorsmodule.h"
#include "dataformat.h"


using namespace Ilwis;
using namespace SpreadSheetConnectors;

SpreadSheetConnectorsModule::SpreadSheetConnectorsModule(QObject *parent) : Module(parent, "SpreadSheetConnectorsModule", "iv40","1.0")
{

}

SpreadSheetConnectorsModule::~SpreadSheetConnectorsModule()
{

}

QString SpreadSheetConnectorsModule::getInterfaceVersion() const
{
      return "iv40";
}

QString SpreadSheetConnectorsModule::getName() const
{
    return "Spread sheet plugin";
}

QString SpreadSheetConnectorsModule::getVersion() const
{
    return "1.0";
}

void SpreadSheetConnectorsModule::prepare()
{
    SpreadSheetConnectorsObjectFactory *factory = new SpreadSheetConnectorsObjectFactory();
    factory->prepare();
    kernel()->addFactory(factory );

    ConnectorFactory *cfactory = kernel()->factory<ConnectorFactory>("ilwis::ConnectorFactory");
    if (!cfactory)
        return ;
    cfactory->addCreator(itTABLE,"spreadsheets",SpreadSheetTableConnector::create);

    cfactory->addCreator("spreadsheet","spreadsheets",SpreadSheetTableConnector::create);

    QFileInfo ilw = context()->ilwisFolder();
    QString path = ilw.canonicalFilePath() + "/extensions/spreadsheetconnectors/resources";
    DataFormat::setFormatInfo(path + "/spreadsheetformats.config","spreadsheets");

    QVariantList names = DataFormat::getFormatProperties(DataFormat::fpCODE,itTABLE,"spreadsheets");
    for(const QVariant& name : names)
        cfactory->addCreator(name.toString(),"spreadsheets", SpreadSheetTableConnector::create);

  kernel()->issues()->log("Loaded spreadsheet module",IssueObject::itMessage);

}
