#include "kernel.h"
#include "ilwisdata.h"
#include "factory.h"
#include "abstractfactory.h"
#include "tableoperation.h"
#include "tableoperationfactory.h"

using namespace Ilwis;
using namespace Desktop;

//std::map<QString, TableOperationCreate> TableOperationFactory::_creators;

TableOperationFactory::TableOperationFactory() : AbstractFactory("TableOperationFactory","ilwis","Generates operation objects for UI accessible table and column operations" )
{

}

void Ilwis::Desktop::TableOperationFactory::registerTableOperation(const QString &name, Ilwis::Desktop::TableOperationCreate createFunc)
{
    _creators[name] = createFunc;
}

QList<TableOperation *> TableOperationFactory::selectedOperations(TableModel *tblModel, const QVariantMap& parameters)
{
    QList<TableOperation *> operations;
    for ( auto creator : _creators){
        auto *operation = creator.second();
        if ( operation){
            if ( operation->canUse(tblModel,parameters)){
                operation->prepare(tblModel);
                operations.append(operation);
            }
            else
                delete operation;
        }
    }
    return operations;
}


