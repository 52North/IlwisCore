#include <QString>
#include <QStringList>
#include "kernel.h"
#include "factory.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "catalogexplorer.h"
#include "connectorfactory.h"


using namespace Ilwis;

std::vector<createCatalogExplorer> ConnectorFactory::_explorers;

uint Ilwis::qHash(const ConnectorFilter& filter ){

    return ::qHash(filter._provider);
}

bool Ilwis::operator==(const ConnectorFilter& filter1, const ConnectorFilter& filter2 ){
    bool typeOk = (filter1._objectTypes & filter2._objectTypes) != 0;
    //bool providerOk = filter1._provider == filter2._provider;
    QRegExp regexpr(filter1._provider,Qt::CaseInsensitive, QRegExp::Wildcard);
    bool providerOk = regexpr.indexIn(filter2._provider) != -1;
    return  typeOk && providerOk;
}

bool Ilwis::operator<(const ConnectorFilter& filter1, const ConnectorFilter& filter2 ){
    return filter1._provider < filter2._provider;
}
//---------------------------------------------------------------
uint Ilwis::qHash(const ConnectorFormatSelector& filter ){

    return ::qHash(filter._format);
}

bool Ilwis::operator==(const ConnectorFormatSelector& filter1, const ConnectorFormatSelector& filter2 ){
    bool ok = filter1._format.toLower() == filter2._format.toLower();
    if (  filter1._provider == sUNDEF || filter2._provider == sUNDEF)
        return ok;
     return  ok && filter1._provider.toLower() == filter2._provider.toLower();
}


bool Ilwis::operator<(const ConnectorFormatSelector& filter1, const ConnectorFormatSelector& filter2 ){
    return filter1._provider.toLower() < filter2._provider.toLower();
}

ConnectorFactory::ConnectorFactory() : AbstractFactory("ConnectorFactory","ilwis","Creates all object connectors" )
{
}

void ConnectorFactory::addCreator(quint64 objecttypes, const QString& provider, ConnectorCreate func)
{
    if ( objecttypes == 0 || func == 0) {
        kernel()->issues()->log(TR(ERR_INVALID_INIT_FOR_2).arg("parameters","Connector Factory"));
        return;
    }
    ConnectorFilter filter(objecttypes, provider);
    if (!_creatorsPerObject.contains(filter)) {
        _creatorsPerObject.insert(filter, func );
    }
}

void ConnectorFactory::addCreator(const QString& format,const QString &provider,  ConnectorCreate func){
    if ( provider == "" || format == "" || provider == sUNDEF || format == sUNDEF){
        kernel()->issues()->log(TR(ERR_INVALID_INIT_FOR_2).arg("parameters","Connector Factory"));
        return;
    }
    ConnectorFormatSelector filter(format, provider);
    if (!_creatorsPerFormat.contains(filter)){
        _creatorsPerFormat.insert(filter, func);
    }
}

std::nullptr_t ConnectorFactory::registerCatalogExplorer(createCatalogExplorer func)
{
    _explorers.push_back(func);

    return nullptr;
}

std::vector<CatalogExplorer*> ConnectorFactory::explorersForResource(const Resource& resource, const QString &provider) const{
   // explorers are split into two classes; implicit and explicit. Implicit catalogs are catalogs that are compound objects, w
   // while explicit catalogs exist withing the container organization of their management system ( OS, database). Implicit catalogs
    // do not have special (catalog) meaning in the manamgement system
    // as the content of implicit catalogs may depend on the content of the explicit catalog they are located in
    //( e.g maps in an ilwis maplist), explicit containers have to be scammed first
   std::vector<CatalogExplorer*> explorers;
   for( createCatalogExplorer createFunc : _explorers){
       IOOptions empty; // not options needed here
       CatalogExplorer *explorer = createFunc(resource, empty);
       if ( explorer){
           bool resourceOk = explorer->canUse(resource);
           bool providerOk = explorer->provider() == provider || provider == sUNDEF;
           if (  resourceOk && providerOk) {
              explorers.push_back(explorer);
           }else
               delete explorer;
       }
   }
   return explorers;
}






