#include <future>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QQmlContext>
#include <QThread>
#include <QDir>
#include <QStandardPaths>
#include "dirent.h"
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

OperationCatalogModel::OperationCatalogModel(QObject *p) : CatalogModel()
{
}

OperationCatalogModel::OperationCatalogModel(const Resource &res,QObject *p) : CatalogModel(res,CatalogModel::ctOPERATION|CatalogModel::ctFIXED|CatalogModel::ctINTERNAL, p)
{

}

QQmlListProperty<OperationsByKeyModel> OperationCatalogModel::operationKeywords()
{
    return  QQmlListProperty<OperationsByKeyModel>(this, _operationsByKey);
}

void OperationCatalogModel::nameFilter(const QString &filter)
{
    _nameFilter = filter;
    emit operationsChanged();
    emit operationsByKeyChanged();
}

void OperationCatalogModel::refresh()
{
    CatalogModel::refresh();
    emit operationsChanged();
    emit operationsByKeyChanged();
}

void OperationCatalogModel::filter(const QString &filterString)
{
    //CatalogModel::filter(filterString);
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

void OperationCatalogModel::fillByName(QList<ResourceModel*>& currentOperations) {
    if ( _nameFilter == "")
        return;

    auto &currentList = currentOperations.size() > 0 ? currentOperations : _allItems;
    QList<ResourceModel *> tempList;
    for(ResourceModel * resource : currentList){
        if ( resource->name().indexOf(_nameFilter) != -1){
            tempList.push_back(resource);
        }
    }
    currentOperations = QList<ResourceModel *>(tempList);
}
void OperationCatalogModel::fillByKeyword(QList<ResourceModel*>& currentOperations) {
    _operationsByKey.clear();
    auto &currentList = currentOperations.size() > 0 ? currentOperations : _allItems;
    QList<ResourceModel *> tempList;
    std::map<QString, std::vector<OperationModel *>> operationsByKey;
    std::set<QString> keywordset;
    QStringList temp = _keyFilter.split(",");
    QStringList filterKeys;
    for(auto k : temp)
        filterKeys.append(k.trimmed());
    for(ResourceModel * item : currentList){
        if ( !( item->resource().ilwisType() & itOPERATIONMETADATA) )
            continue;

        if ( item->resource().hasProperty("keyword")){
            QStringList validKeys;
            QString keysstring = item->resource()["keyword"].toString();
            if ( keysstring.indexOf("internal") != -1)
                continue;
            QStringList keys = keysstring.split(",");
            bool found = true;
            for(auto filterKey : filterKeys){
                for(auto key : keys){
                    if(key.indexOf(filterKey) >= 0){
                        found &= true;
                        validKeys.append(key);
                    }

                }
            }
            if(found && validKeys.size() > 0){
                tempList.push_back(item);
            }


        }
    }
    for(ResourceModel *operation : tempList){
        QString keysstring = operation->resource()["keyword"].toString();
        QStringList keys = keysstring.split(",");
        for(auto key : keys){
            operationsByKey[key].push_back(static_cast<OperationModel *>(operation));
            keywordset.insert(key);
        }
    }
    _operationsByKey.clear();
    for( const auto& item : operationsByKey){
         _operationsByKey.push_back(new OperationsByKeyModel(item.first, item.second, this));
    }
    _keywords.clear();
    for(auto keyword : keywordset)
        _keywords.push_back(keyword);


    qSort(_keywords.begin(), _keywords.end());

    _keywords.push_front(""); // all
    currentOperations = QList<ResourceModel *>(tempList);
}

QQmlListProperty<OperationModel> OperationCatalogModel::operations()
{
    try{
        gatherItems();
        QList<ResourceModel*> currentOperations;
        if ( _keyFilter == "" && _nameFilter == "")
            currentOperations = QList<ResourceModel *>(_allItems);
        if (_nameFilter != "")
            fillByName(currentOperations);
        if (_keyFilter != ""){
            fillByKeyword(currentOperations);
        }
        _currentOperations.clear();
        for(auto resource : currentOperations){
            _currentOperations.append(static_cast<OperationModel *>(resource));
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
        _view.filter("basefilter", "(type=" + QString::number(itSINGLEOPERATION) + " or type=" + QString::number(itWORKFLOW) + ")");
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
        setDescription("main catalog for ilwis operations");
        filter(QString("(type=%1 or type=%2)").arg(itSINGLEOPERATION).arg(itWORKFLOW));

        QString serviceFilter = QString("(type=%1 and catalogitemproperties.propertyname='keyword' and catalogitemproperties.propertyvalue like '%service%')").arg(itSINGLEOPERATION);

        _services = mastercatalog()->select(serviceFilter);
    }else {
        setView(currentModel->viewRef());
    }

    _allItems.clear();
    _refresh = false;

    std::vector<Resource> items = _view.items();
    std::map<QString, std::vector<OperationModel *>> operationsByKey;
    std::set<QString> keywordset;

    for(const Resource& resource : items){
        OperationModel *operation = new OperationModel(resource, this);
        _allItems.push_back(operation);
        QString keysstring = operation->resource()["keyword"].toString();
        QStringList keys = keysstring.split(",");
        for(auto key : keys){
            operationsByKey[key].push_back(operation);
            keywordset.insert(key);
        }
    }
    _operationsByKey.clear();
    for( const auto& item : operationsByKey){
         _operationsByKey.push_back(new OperationsByKeyModel(item.first, item.second, this));

    }
    _keywords.clear();
    for(auto keyword : keywordset)
        _keywords.push_back(keyword);


    qSort(_keywords.begin(), _keywords.end());

    _keywords.push_front(""); // all
}

QStringList OperationCatalogModel::keywords() const
{
    return _keywords;
}

void OperationCatalogModel::workSpaceChanged()
{
    if ( !_isGlobalOperationsCatalog){
        _allItems.clear();
        _currentOperations.clear();
        _operationsByKey.clear();
        _services.clear();
        _refresh = true;

        emit operationsChanged();
        emit operationsByKeyChanged();
    }
}


/**
 * Executes an operation (or workflow) and generates output
 * @param parameters the input and output parameters that the user filled in
 */
QString OperationCatalogModel::executeoperation(quint64 operationid, const QString& parameters, QVariant runparams) {

    auto opExpr = OperationExpression::createExpression(operationid, parameters);
    try {
        QThread* thread = new QThread;
        thread->setProperty("runparameters",runparams);
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

void OperationCatalogModel::keyFilter(const QString &keyf)
{
    _keyFilter = keyf;
    emit operationsChanged();
    emit operationsByKeyChanged();
}
QString OperationCatalogModel::nameFilter() const
{
    return _nameFilter;
}

QString OperationCatalogModel::keyFilter() const
{
    return _keyFilter;
}
