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
#include "ilwisobjectconnector.h"
#include "internaltableconnector.h"

using namespace Ilwis;
using namespace Internal;

ConnectorInterface *Ilwis::Internal::InternalTableConnector::create(const Ilwis::Resource &item,bool load)
{
    return new InternalTableConnector(item, load);
}

InternalTableConnector::InternalTableConnector(const Resource &item, bool load) : IlwisObjectConnector(item, load)
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
    return true;
}

QString InternalTableConnector::provider() const
{
    return "internal";
}
