#include <QString>
#include <QSharedPointer>
#include <QVector>
#include <QUrl>
#include <QFileInfo>
#include "kernel.h"
#include "identity.h"
#include "factory.h"
#include "abstractfactory.h"
#include "catalog.h"
#include "connectorfactory.h"
#include "connectorinterface.h"
#include  "resource.h"
#include "catalogconnector.h"

using namespace Ilwis;

Resource &CatalogConnector::source()
{
    return _location;
}

ConnectorInterface *CatalogConnector::clone() const
{
//    CatalogConnector *connector = ConnectorInterface::create<CatalogConnector>(_location, "ilwis3");
//    if(!connector) {
//        kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Connector",_location.toString()));
//        return 0;
//    }

    return 0;
}
