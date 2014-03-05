#include <QString>
#include <QStringList>
#include "kernel.h"
#include "factory.h"
#include "connectorinterface.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalogconnector.h"
#include "catalogconnectorfactory.h"

using namespace Ilwis;

CatalogConnectorFactory::CatalogConnectorFactory() : AbstractFactory("catalogconnectorfactory","ilwis","")
{
}

QList<CatalogConnector *> CatalogConnectorFactory::create(const Resource &location) const{

    QList<CatalogConnector *> finalList;
    QListIterator<ConnectorCreate> iter(_creatorsPerObject);
    while(iter.hasNext()) {
        ConnectorCreate createFunc = iter.next();
        CatalogConnector *conn = static_cast<CatalogConnector *>(createFunc(location,true));
        if ( conn && conn->canUse(location)) {
            finalList.push_back(conn);
        } else
            delete conn;
    }

    return finalList;
}

void CatalogConnectorFactory::add(ConnectorCreate cc)
{
    if (!_creatorsPerObject.contains(cc))
        _creatorsPerObject.push_back(cc);
}

