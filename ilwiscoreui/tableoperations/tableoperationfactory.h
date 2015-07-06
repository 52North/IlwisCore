#ifndef TABLEOPERATIONFACTORY_H
#define TABLEOPERATIONFACTORY_H

#include "ilwiscoreui_global.h"

namespace Ilwis {
namespace Desktop {

class TableOperation;

typedef std::function<TableOperation *()> TableOperationCreate;


class ILWISCOREUISHARED_EXPORT TableOperationFactory : public AbstractFactory
{
public:
    TableOperationFactory();
    void registerTableOperation(const QString& name, TableOperationCreate);
    TableOperationCreate * create(const QString& name);
    QList<TableOperation *> selectedOperations(TableModel *tblModel, const QVariantMap &parameters);
    template<class T=TableOperation> T *create(const QString& name)
    {
       auto iter = _creators.find(name);
       if ( iter == _creators.end()){
           return 0;
       }
       return static_cast<T *>((*iter).second());
    }

private:
     std::map<QString, TableOperationCreate> _creators;

};
}
}

#endif // TABLEOPERATIONFACTORY_H
