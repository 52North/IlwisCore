#include "kernel.h"
#include "internaldatabaseconnection.h"
#include "internaldatabasemodel.h"

using namespace Ilwis;

InternalDatabaseModel::InternalDatabaseModel(QObject *parent) : QObject(parent)
{

}

QVariantList InternalDatabaseModel::query(const QString &query)
{
    InternalDatabaseConnection conn(query);

    QVariantList result;
    int count = 0;
    if (conn.exec()){
        while(conn.next()){
            QSqlRecord rec = conn.record();
            QVariantMap fields;
            ++count;
            for(int i = 0; i< rec.count(); ++i){
                fields[rec.fieldName(i)] = rec.value(i);
            }
            result.append(fields);
        }
    }
    return result;

}

