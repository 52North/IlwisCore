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
#include "table.h"
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
#include "consolescriptmodel.h"
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
}

void OperationCatalogModel::refresh()
{
    _refresh = true;
    _allItems.clear();

    emit operationsChanged();
    emit operationsByKeyChanged();
}

void tableCase(const IIlwisObject &obj, const QString& condition, int parmIndex, QVariantList& result)
{
    ITable tbl ;
    if (hasType(obj->ilwisType(), itCOVERAGE)){
        ICoverage coverage = obj.as<Coverage>();
        tbl = coverage->attributeTable();
    }else if (hasType(obj->ilwisType(), itTABLE) ){
        tbl = obj.as<Table>();
    }
    QVariantMap mp;
    mp["parameterIndex"] = parmIndex;
    QStringList names;
    int index;
    IlwisTypes domainType;
    if ( (index = condition.indexOf(" with ")) != -1){
        QString domainPart = condition.mid(index + 6);
        QStringList parts = domainPart.split("=");
        QVariantMap mp;
        if ( parts.size() == 2){
            domainType = IlwisObject::name2Type(parts[1]);
      }
    }
    for(int c=0; c < tbl->columnCount(); ++c){
        if ( domainType != itUNKNOWN){
            DataDefinition def = tbl->columndefinition(c).datadef();
            if ( hasType(def.domain()->ilwisType(), domainType))
               names.append(tbl->columndefinition(c).name());
        }else {
            names.append(tbl->columndefinition(c).name());
        }
    }
    mp["result"] = names;
    mp["uielement"] = "list";
    result.append(mp);
}
void valueListCase(const IIlwisObject& obj, const QString& condition, int parmIndex,  QVariantList& result){
    int index = condition.indexOf("with");
    QString rest = condition.mid(index + 5 );
    rest.trimmed();

    QVariantMap mp;
    mp["parameterIndex"] = parmIndex;
    QStringList codes;

    InternalDatabaseConnection db(rest);


    while(db.next()){
        QString code = db.value(0).toString();
        codes.push_back(code);
    }
    mp["result"] = codes;
    mp["uielement"] = "list";
    result.append(mp);

}

void domainCase(const IIlwisObject& obj, const QString& condition, int parmIndex,  QVariantList& result)
{
    if (hasType(obj->ilwisType(), itRASTER))    {
        IRasterCoverage raster = obj.as<RasterCoverage>();
        QStringList parts = condition.split("=");
        QVariantMap mp;
        if ( parts.size() == 2){
            if (parts[0] == "domain"){
                QString domainType = parts[1];
                if ( domainType == "numericdomain"){
                    mp["parameterIndex"] = parmIndex;
                    mp["result"] = hasType(raster->datadef().domain()->ilwisType(), itNUMERICDOMAIN) ? obj->resource().url().toString() : "";
                    mp["uielement"] = "textfield";
                }else if ( domainType == "itemdomain"){
                    mp["parameterIndex"] = parmIndex;
                    mp["result"] = hasType(raster->datadef().domain()->ilwisType(), itITEMDOMAIN) ? obj->resource().url().toString() : "";
                    mp["uielement"] = "textfield";
                }
            } else if ( parts[0] == "valuetype"){
                QString valueType = parts[1];
                IlwisTypes vt = IlwisObject::name2Type(valueType);
                mp["parameterIndex"] = parmIndex;
                mp["result"] = hasType(raster->datadef().domain()->valueType(), vt) ? obj->resource().url().toString() : "";
                mp["uielement"] = "textfield";
            }
        }
        result.append(mp);
    }
}

QVariantList OperationCatalogModel::resolveValidation(const QString &metaid, const QString& objectid, int sourceParameterIndex)
{
   QVariantList result;
    try {

       Resource resource = mastercatalog()->id2Resource(metaid.toULongLong());
       IIlwisObject obj;
       obj.prepare(objectid.toULongLong());
       if ( obj.isValid() && resource.isValid()){
           QStringList lst = resource["inparameters"].toString().split("|");
           int maxParmCount = lst.last().toInt();
           for(int i = 0; i < maxParmCount; ++i){
               QString parmPrefix = "pin_" + QString::number(i+1) + "_";
               if ( resource.hasProperty(parmPrefix + "validationcondition")){
                   int source = resource[parmPrefix + "validationsource"].toInt();
                   if ( source == sourceParameterIndex){
                       QString condition = resource[parmPrefix + "validationcondition"].toString();
                       if ( condition != sUNDEF){
                           if ( condition.indexOf("columns") == 0){
                               tableCase(obj, condition, i, result);
                           }else if ( condition.indexOf("domain") == 0){
                                domainCase(obj, condition, i, result);
                           }else if ( condition.indexOf("values with") == 0){
                               valueListCase(obj, condition, i, result);
                           }
                       }
                   }
               }
           }
       }
       return result;
   }catch(const ErrorObject& err){
   }
   return result;
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
        if ( _refresh || _currentOperations.size() == 0){
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

    _allItems.clear();
    _refresh = false;

    WorkSpaceModel *currentModel = uicontext()->currentWorkSpace();
    bool isDefault = false;
    if (currentModel){
        auto n = currentModel->name();
        isDefault = n == "default";
    }
    if ( currentModel == 0 || isDefault){
        setDescription("main catalog for ilwis operations");

        QString serviceFilter = QString("(type=%1 and catalogitemproperties.propertyname='keyword' and catalogitemproperties.propertyvalue like '%service%')").arg(itSINGLEOPERATION);

        _services = mastercatalog()->select(serviceFilter);
    }else {
        setView(currentModel->viewRef());
    }

    std::vector<Resource> items = _view.items();
    std::sort(items.begin(), items.end(),[](const Resource& r1, const Resource& r2){
        QString name1 = r1["longname"].toString();
        if ( name1 == sUNDEF)
            name1 = r1.name();
        QString name2 = r2["longname"].toString();
        if ( name2 == sUNDEF)
            name2 = r2.name();
        return name1.toLower() < name2.toLower();


    });
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
        thread->setProperty("workingcatalog", qVariantFromValue(context()->workingCatalog()));
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
}
QString OperationCatalogModel::nameFilter() const
{
    return _nameFilter;
}

QString OperationCatalogModel::keyFilter() const
{
    return _keyFilter;
}
