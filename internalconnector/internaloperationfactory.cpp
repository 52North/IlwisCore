#include <QString>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QUrlQuery>
#include <functional>
#include "kernel.h"
#include "factory.h"
#include "abstractfactory.h"
#include "connectorinterface.h"
#include "connectorfactory.h"
#include "ilwisobjectfactory.h"
#include "internaloperationfactory.h"

using namespace Ilwis;
using namespace Internal;

InternalOperationFactory::InternalOperationFactory() : IlwisObjectFactory("IlwisObjectFactory", "internal", "operations")
{
}

Ilwis::IlwisObject *InternalOperationFactory::create(const Resource& item) const
{
}

bool InternalOperationFactory::canUse(const Ilwis::Resource &item) const
{
    return findOperationId(item) != i64UNDEF;
}

quint64 InternalOperationFactory::findOperationId(const Ilwis::Resource &item) const {
    OperationExpression expr(item["expression"]);
    bool ok;
    QSqlQuery db(kernel()->database());
    QSqlQuery db2(kernel()->database());
    QString query = QString("select * from mastercatalog where resource = '%1' and (type & %2) != 0").arg(item.url().toString()).arg(tp);
    if (db.exec(query)) {
        while ( db.next()){
            quint64 itemid = db.value("itemid").toLongLong();
            query = QString("select * from catalogitemproperties catalogitemproperties.itemid=%1").arg(itemid);
            if (db2.exec(query)) {
                std::map<QString, QString> values;
                while ( db2.next()){
                    QSqlRecord rec = db.record();
                    values[rec.value("propertyname").toString()] = rec.value("propertyvalue").toString();
                }
                int n = values["inparameters"].toInt();
                if ( n != expr.parameterCount())
                    continue;
                bool found = true;
                for(int i=0; i < expr.parameterCount(); ++i) {
                    QString key = QString("in::parameter::%1::type").arg(i);
                    IlwisTypes tpExpr = expr.parm(i).valuetype();
                    auto iter = values.find(key);
                    if ( iter == values.end()){
                        found = false;
                        break;
                    }
                    IlwisTypes tpMeta = (*iter).second.toLongLong();
                    if ( tpMeta != tpExpr && tpExpr != i64UNDEF) {
                        found = false;
                        break;
                    }

                }
                if ( found)
                    return itemid;
            }
        }
    }
    ERROR2(ERR_NO_INITIALIZED_2,"metadata",item.name());
    return i64UNDEF;
}
