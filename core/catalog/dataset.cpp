#include <QString>
#include <QUrl>
#include <QFileInfo>
#include <QSqlQuery>
#include <QSqlRecord>
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogconnector.h"
#include "abstractfactory.h"
#include "connectorfactory.h"
#include "catalog.h"
#include "catalogquery.h"
#include "catalogexplorer.h"
#include "ilwiscontext.h"
#include "mastercatalog.h"
#include "oshelper.h"
#include "indexdefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "dataset.h"

using namespace Ilwis;
DataSet::DataSet()
{
}

DataSet::DataSet(const Resource &resource) : Catalog(resource)
{
    QString domainName = resource["domain"].toString();
    IDomain dom(domainName);
    if ( !dom.isValid())
        return;
    _index.domain(dom);
    QString indexValues = resource["indexvalues"].toString();
    QStringList values= indexValues.split("|");
    for(int i=0; i < values.size(); ++i){
        _index.addIndex(0,values[i],i);
    }
}

DataSet::~DataSet()
{

}

bool DataSet::isValid() const
{
    return _index.domain().isValid() && _index.indexSize() > 0;
}

IIlwisObject DataSet::createObject(const QVariant &indexValue) const
{
     quint32 index =_index(0,indexValue);
     if ( index != iUNDEF)
         return IIlwisObject( item(index).id());
     return IIlwisObject();
}

bool DataSet::prepare()
{
    return true;
}
