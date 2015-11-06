#include <future>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QQmlContext>
#include <QThread>
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
#include "workspacemodel.h"
#include "uicontextmodel.h"
#include "ilwiscontext.h"
#include "operationworker.h"
#include "dataformat.h"
#include "operationcatalogmodel.h"
#include "workflow/workflowmodel.h"

using namespace Ilwis;

OperationCatalogModel::OperationCatalogModel(QObject *p) : CatalogModel(p)
{


}

QQmlListProperty<OperationsByKeyModel> OperationCatalogModel::operationKeywords()
{
    return  QQmlListProperty<OperationsByKeyModel>(this, _operationsByKey);
}

void OperationCatalogModel::nameFilter(const QString &filter)
{
    CatalogModel::nameFilter(filter);
    _currentOperations.clear();
    _operationsByKey.clear();
    _refresh = true;
    emit operationsChanged();
}

void OperationCatalogModel::refresh()
{
    CatalogModel::refresh();
    _currentOperations.clear();
    _operationsByKey.clear();
    _refresh = true;
    emit operationsChanged();
}

void OperationCatalogModel::filter(const QString &filterString)
{
    CatalogModel::filter(filterString);
    _currentOperations.clear();
    _operationsByKey.clear();
    _refresh = true;
    emit operationsChanged();

}

quint64 OperationCatalogModel::operationId(quint32 index, bool byKey) const{
    if ( byKey){

    }
    else if ( index < _currentOperations.size()){
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

            gatherItems();

            _currentOperations.clear();

            std::map<QString, std::vector<OperationModel *>> operationsByKey;

            for(auto item : _currentItems){
                QString keywords = item->resource()["keyword"].toString();
                if ( !( item->resource().ilwisType() & itOPERATIONMETADATA) )
                    continue;
                if ( keywords.indexOf("internal") != -1)
                    continue;
                _currentOperations.push_back(new OperationModel(item->resource(), this));
                if ( keywords == sUNDEF)
                    keywords = TR("Uncatagorized");
                QStringList parts = keywords.split(",");
                for(auto keyword : parts){
                    operationsByKey[keyword].push_back(new OperationModel(item->resource(), this));
                }
            }
            for(auto operation : operationsByKey){
                _operationsByKey.push_back(new OperationsByKeyModel(operation.first, operation.second, this));
            }

        }

        return  QMLOperationList(this, _currentOperations);
    }
    catch(const ErrorObject& err){

    }
    return  QMLOperationList();
}
void OperationCatalogModel::prepare(const IOOptions& opt){
    if ( opt.contains("globaloperationscatalog")){
        _isGlobalOperationsCatalog = opt["globaloperationscatalog"].toBool();
    }
    _refresh  = true;
    gatherItems();
}

quint64 OperationCatalogModel::operationId(const QString &name)
{
    // query for operations and workflows
    QString query = QString("type=%1 and name='%2'").arg(itOPERATIONMETADATA).arg(name);
    std::vector<Resource> items = mastercatalog()->select(query);
    if ( items.size() == 0)
        return i64UNDEF;
    return items[0].id();
}

void OperationCatalogModel::gatherItems() {
    if (!_refresh)
        return;

    WorkSpaceModel *currentModel = uicontext()->currentWorkSpace();
    bool isDefault = false;
    if (currentModel){
        auto n = currentModel->name();
        isDefault = n == "default";
    }
    if ( currentModel == 0 || isDefault){
        if ( !_view.isValid()){
            QUrl location("ilwis://operations");
            QString descr ="main catalog for ilwis operations";
            Resource res(location, itCATALOGVIEW ) ;
            res.name("ilwis-operations",false);
            QStringList lst;
            lst << location.toString();
            res.addProperty("locations", lst);
            res.addProperty("type", "operation" );
            res.addProperty("filter",QString("(type=%1 or type=%2)").arg(itSINGLEOPERATION).arg(itWORKFLOW));
            res.setDescription(descr);
            setView(CatalogView(res));

            location = QUrl("ilwis://operations");
            descr ="main catalog for ilwis services";
            res = Resource(location, itCATALOGVIEW ) ;
            res.name("ilwis-services",false);
            lst.clear();
            lst << location.toString();
            res.addProperty("locations", lst);
            res.addProperty("type", "operation" );
            res.addProperty("filter",QString("(type=%1 and keyword='service')").arg(itSINGLEOPERATION));
            res.setDescription(descr);
            CatalogView view(res);
            view.prepare();

            _services = view.items();
        }
    }else {
        setView(currentModel->view());
    }
    CatalogModel::gatherItems();
    std::set<QString> keywordset;
    std::map<QString, std::vector<OperationModel *>> operationsByKey;
    for(auto item : _currentItems){
        QString keywords = item->resource()["keyword"].toString();
        if ( !(item->resource().ilwisType() & itOPERATIONMETADATA))
            continue;
        if ( keywords.indexOf("internal") != -1)
            continue;
        _currentOperations.push_back(new OperationModel(item->resource(), this));
        if ( keywords == sUNDEF)
            keywords = TR("Uncatagorized");
        QStringList parts = keywords.split(",");
        for(auto keyword : parts){
            keywordset.insert(keyword);
        }
        for(auto keyword : parts){
            operationsByKey[keyword].push_back(new OperationModel(item->resource(), this));
        }
    }
    _keywords.clear();
    for(auto keyword : keywordset)
        _keywords.push_back(keyword);


    qSort(_keywords.begin(), _keywords.end());

    _keywords.push_front(""); // all

    for(auto operation : operationsByKey){
        _operationsByKey.push_back(new OperationsByKeyModel(operation.first, operation.second, this));
    }
}

QStringList OperationCatalogModel::keywords() const
{
    return _keywords;
}

void OperationCatalogModel::workSpaceChanged()
{
    if ( !_isGlobalOperationsCatalog){
        _currentItems.clear();
        _currentOperations.clear();
        _operationsByKey.clear();
        _services.clear();
        _refresh = true;

        emit operationsChanged();
    }
}

QString OperationCatalogModel::modifyTableOutputUrl(const QString& output, const QStringList& parms)
{
    QString columnName = output;
    QString firstTable = parms[0];
    if ( firstTable.indexOf("://") != -1){
        int index = firstTable.lastIndexOf("/");
        firstTable = firstTable.mid(index + 1);
        index =  firstTable.indexOf(".");
        if ( index != -1)
            firstTable = firstTable.left(index) + ".ilwis";
    }
    QString internalPath = context()->persistentInternalCatalog().toString();
    QString outpath = internalPath + "/" + firstTable + "[" + columnName + "]";

    return outpath;

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
    IlwisTypes outputtype = operationresource["pout_1_type"].toULongLong();
    if ( output.indexOf("@@") != -1 ){
        QString format;
        QStringList parts = output.split("@@");
        output = parts[0];

        QString formatName = parts[1];
        if ( hasType(outputtype, itTABLE)){
            if ( formatName == "Memory"){
                output = modifyTableOutputUrl(output, parms);
            }else
                output = parms[0] + "[" + output + "]";
        }
        if ( formatName == "Keep original"){
            IIlwisObject obj;
            obj.prepare(parms[0], operationresource["pin_1_type"].toULongLong());
            if ( obj.isValid())
                format = "{format(" + obj->provider() + ",\"" + obj->formatCode() + "\")}";
        }
        if ( formatName != "Memory"){ // special case
            if ( format == "") {
                QString query = "name='" + formatName + "'";
                std::multimap<QString, Ilwis::DataFormat>  formats = Ilwis::DataFormat::getSelectedBy(Ilwis::DataFormat::fpNAME, query);
                if ( formats.size() == 1){
                    format = "{format(" + (*formats.begin()).second.property(DataFormat::fpCONNECTOR).toString() + ",\"" +
                            (*formats.begin()).second.property(DataFormat::fpCODE).toString() + "\")}";
                }
            }
            if ( output.indexOf("://") == -1)
                output = context()->workingCatalog()->source().url().toString() + "/" + output + format;
            else
                output = output + format;
        }else{

            if ( hasType(outputtype,itRASTER))
                format = "{format(stream,\"rastercoverage\")}";
            else if (hasType(outputtype, itFEATURE))
                format = "{format(stream,\"featurecoverage\")}";
            else if (hasType(outputtype, itTABLE)){
                format = "{format(stream,\"table\")}";
            }
            output = output + format;
        }
    }

    if ( output == "")
        expression = QString("script %1(%2)").arg(operationresource.name()).arg(expression);
    else
        expression = QString("script %1=%2(%3)").arg(output).arg(operationresource.name()).arg(expression);

    OperationExpression opExpr(expression);



    try {
        QThread* thread = new QThread;
        OperationWorker* worker = new OperationWorker(opExpr);
        worker->moveToThread(thread);
        thread->connect(thread, &QThread::started, worker, &OperationWorker::process);
        thread->connect(worker, &OperationWorker::finished, thread, &QThread::quit);
        thread->connect(worker, &OperationWorker::finished, worker, &OperationWorker::deleteLater);
        thread->connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        thread->start();


    return "TODO";
    } catch (const ErrorObject& err){
        emit error(err.message());
    }
    return sUNDEF;

}

OperationModel *OperationCatalogModel::operation(const QString &id)
{
    for(auto *operation : _currentOperations)    {
        if ( operation->id() == id)
            return operation;
    }
    return 0;
}

WorkflowModel *OperationCatalogModel::createWorkFlow(const QString &filter)
{
    return 0;
}





