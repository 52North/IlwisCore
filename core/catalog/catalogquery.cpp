#include "ilwisobject.h"
#include "catalogquery.h"

using namespace Ilwis;

CatalogQuery::CatalogQuery()
{
    int bits = log(itILWISOBJECT) / log(2);
    for(int i=1 ;i < bits; ++i) {
        _names[IlwisObject::type2Name(pow(2,i))] = pow(2,i);
    }
    _names["Catalog"] = itCATALOG;
}

QString CatalogQuery::transformQuery(const QString& baseQuery) const{
    QString query = baseQuery.toLower();
    for(auto name = _names.constBegin(); name != _names.constEnd(); ++name) {
        QString findTxt = "'" + name.key().toLower() + "'";
        query.replace(findTxt, QString::number(name.value()));
    }
    return query;
}
