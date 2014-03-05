#include <QSqlQuery>
#include <QSqlError>

#include "kernel.h"
#include "ilwisdata.h"
#include "connectorinterface.h"
#include "domain.h"
#include "numericrange.h"
#include "numericdomain.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "basetable.h"
#include "flattable.h"
#include "containerstatistics.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "internaltableconnector.h"

using namespace Ilwis;
using namespace Internal;

ConnectorInterface *Ilwis::Internal::InternalTableConnector::create(const Ilwis::Resource &resource,bool load)
{
    return new InternalTableConnector(resource, load);
}

InternalTableConnector::InternalTableConnector(const Resource &resource, bool load) : IlwisObjectConnector(resource, load)
{
}

bool InternalTableConnector::loadMetaData(IlwisObject *data)
{
    return true;
}


IlwisObject *InternalTableConnector::create() const
{
    return new FlatTable(_resource);
}

bool InternalTableConnector::loadBinaryData(IlwisObject *obj)
{
    Table *table = static_cast<Table *>(obj);

    if(!table->createTable())
        return false;
    _binaryIsLoaded = true;
    return true;
}

QString InternalTableConnector::provider() const
{
    return "internal";
}
