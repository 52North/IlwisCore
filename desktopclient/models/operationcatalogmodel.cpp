#include <future>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QQmlContext>
#include "kernel.h"
#include "connectorinterface.h"
#include "resource.h"
#include "ilwisobject.h"
#include "raster.h"
#include "mastercatalog.h"
#include "catalogview.h"
#include "resourcemodel.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "operationmodel.h"
#include "operationcatalogmodel.h"

using namespace Ilwis;

OperationCatalogModel::OperationCatalogModel(QObject *) : CatalogModel()
{
    QUrl location("ilwis://operations");
    QString descr ="main catalog for ilwis operations";
    Resource res(location, itCATALOGVIEW ) ;
    res.name("ilwis-operations",false);
    QStringList lst;
    lst << location.toString();
    res.addProperty("locations", lst);
    res.addProperty("type", "operation" );
    res.addProperty("filter",QString("type=%1").arg(itOPERATIONMETADATA));
    res.setDescription(descr);
    newview(CatalogView(res));

    location = QUrl("ilwis://operations");
    descr ="main catalog for ilwis services";
    res = Resource(location, itCATALOGVIEW ) ;
    res.name("ilwis-services",false);
    lst.clear();
    lst << location.toString();
    res.addProperty("locations", lst);
    res.addProperty("type", "operation" );
    res.addProperty("filter",QString("type=%1 and keyword='service'").arg(itOPERATIONMETADATA));
    res.setDescription(descr);
    CatalogView view(res);
    view.prepare();

    _services = view.items();

}

quint64 OperationCatalogModel::operationId(quint32 index) const{
    if ( index < _currentOperations.size()){
        return _currentOperations[index]->id().toULongLong();
    }
    return i64UNDEF;
}

quint64 OperationCatalogModel::serviceId(quint32 index) const
{
    if ( index < _services.size()){
        return _services[index].id();
    }
    return i64UNDEF;
}

QStringList OperationCatalogModel::serviceNames() const{
    QStringList names;
    for(const Resource& service : _services){
        if ( service.hasProperty("longname"))
            names.push_back(service["longname"].toString());
        else
            names.push_back(service.name());
    }
    return names;

}

QQmlListProperty<OperationModel> OperationCatalogModel::operations()
{
    try{
        if ( _currentOperations.isEmpty()) {
            if ( !_view.isValid())
                return QMLOperationList();

//            _view.prepare();

            gatherItems();

            for(OperationModel *model : _currentOperations)
                delete model;

            _currentOperations.clear();

            for(auto item : _currentItems){
                _currentOperations.push_back(new OperationModel(item->resource()));
            }
        }

        return  QMLOperationList(this, _currentOperations);
    }
    catch(const ErrorObject& err){

    }
    return  QMLOperationList();
}

bool runApplication( OperationExpression opExpr, QString *result){
    Operation op(opExpr);
    SymbolTable tbl;
    ExecutionContext ctx;

    if(op->execute(&ctx, tbl)){
        if ( ctx._results.size() > 0){
            for(auto resultName : ctx._results){
                Symbol symbol = tbl.getSymbol(resultName);
                if ( hasType(symbol._type, itNUMBER)){
                    *result += symbol._var.toDouble();
                }else if ( hasType(symbol._type, itSTRING)){
                    *result += symbol._var.toString();
                }else if ( hasType(symbol._type, (itCOVERAGE | itTABLE))){
                    if ( symbol._type == itRASTER){
                        IRasterCoverage raster = symbol._var.value<IRasterCoverage>();
                        QUrl url = raster->source().container();
                    }
                }
            }
        }
        return true;
    }
    return false;
}

void startApplication( OperationExpression opExpr, OperationCatalogModel *operationCatalogModel){

    QString result;
    std::future<bool> resultApplication = std::async(runApplication, opExpr,&result) ;

    if(!resultApplication.get()) {
        throw ErrorObject(TR("running %1 failed").arg(opExpr.name()));
    }
    emit operationCatalogModel->updateCatalog(QUrl("ilwis://internalcatalog")); // TODO
}

QString OperationCatalogModel::executeoperation(quint64 operationid, const QString& parameters) {
    if ( operationid == 0 || parameters == "")
        return sUNDEF;

    Resource operationresource = mastercatalog()->id2Resource(operationid);
    if ( !operationresource.isValid())
        return sUNDEF;

    QString expression;
    QStringList parms = parameters.split("|");

    for(int i = 0; i < parms.size() - 1; ++ i){ // -1 because the last is the output parameter
        if ( expression.size() != 0)
            expression += ",";
        expression += parms[i];
    }
    QString output = parms[parms.size() - 1];
    expression = QString("%1=%2(%3)").arg(output).arg(operationresource.name()).arg(expression);

    OperationExpression opExpr(expression);



    try {

    std::async(std::launch::async, startApplication, opExpr, this);

    return "TODO";
    } catch (const ErrorObject& err){
        emit error(err.message());
    }
    return sUNDEF;

}




