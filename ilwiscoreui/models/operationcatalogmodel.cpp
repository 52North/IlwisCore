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
            firstTable = firstTable.left(index) ;
    }
    QString internalPath = context()->persistentInternalCatalog().toString();
    QString outpath = internalPath + "/" + output;

    return outpath;

}

/**
 * Executes an operation (or workflow) and generates output
 * @param parameters the input and output parameters that the user filled in
 */
QString OperationCatalogModel::executeoperation(quint64 operationid, const QString& parameters) {
    if ( operationid == 0 || parameters == "")
        return sUNDEF;

    Resource operationresource = mastercatalog()->id2Resource(operationid);
    if ( !operationresource.isValid())
        return sUNDEF;

    em->clearList();

    QString expression;
    QStringList parms = parameters.split("|");
    bool hasMissingParameters = false;

    for(int i = 0; i < parms.size(); ++ i){
        if (operationresource.ilwisType() & itWORKFLOW){
            int parm = i + 1;
            if (operationresource[QString("pout_%1_optional").arg(parm)] == "false" && i < operationresource["outparameters"].toInt()) {
                QString value = parms[i + operationresource["inparameters"].toInt()];
                if (value.split("@@")[0].size() == 0) {
                    em->addError(1, "Output parameter " + QString::number(i) + " is undefined with name " +  operationresource[QString("pout_%1_name").arg(parm)].toString());
                    hasMissingParameters = true;
                }
            }
            if (operationresource[QString("pin_%1_optional").arg(parm)] == "false" && i < operationresource["inparameters"].toInt() && parms[i].size() == 0) {
                em->addError(1, "Input parameter " + QString::number(i) + " is undefined with name " +  operationresource[QString("pin_%1_name").arg(parm)].toString());
                hasMissingParameters = true;
            }
        }
        if(i < operationresource["inparameters"].toInt()){
            if ( expression.size() != 0)
                expression += ",";
            expression += parms[i];
        }
    }

    if (hasMissingParameters) return sUNDEF;

    QString allOutputsString;

    bool duplicateFileNames = false;

    QStringList parts = operationresource["outparameters"].toString().split("|");
    int maxparms = parts.last().toInt();
    int count = 1;
    for(int i=(parms.size() - maxparms); i<parms.size(); ++i){
        QString output = parms[i];


        QString pout = QString("pout_%1_type").arg(count++);

        IlwisTypes outputtype = operationresource[pout].toULongLong();
        if ( output.indexOf("@@") != -1 ){
            QString format;
            QStringList parts = output.split("@@");
            output = parts[0];
            if ( output == "")
                continue;

            //Check if user didnt put the same output name in another output field
            int occurences = 0;
            for(int j=(parms.size() - maxparms); j<parms.size(); ++j){
                QString compareString = parms[j].split("@@")[0];
                if(output == compareString){
                    occurences++;
                }
            }

            //Add the duplicate name to the list of duplicate names
            if(occurences>1){
                duplicateFileNames = true;
                em->addError(111, "Workflow did not execute, multiple occurences of an output name");
            }

            QString formatName = parts[1];

            if ( operationresource.ilwisType() & itWORKFLOW) {
                QStringList existingFileNames;

                DIR *directory;

                //If not memory
                QString fileName;

                if(formatName == "Memory" ){
                    //Get all files in the internal catalog
                    QString dataLocation = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/internalcatalog";
                    directory = opendir(dataLocation.toStdString().c_str());
                }else {
                    //Get all files in the directory
                    QString dataLocation = output;
                    dataLocation.remove("file:///");

                    QStringList splitUrl = dataLocation.split("/");

                    fileName = splitUrl.last();

                    QString query = "name='" + formatName + "'";
                    std::multimap<QString, Ilwis::DataFormat>  formats = Ilwis::DataFormat::getSelectedBy(Ilwis::DataFormat::fpNAME, query);
                    if ( formats.size() == 1){
                        QString connector = (*formats.begin()).second.property(DataFormat::fpCONNECTOR).toString();
                        QString code = (*formats.begin()).second.property(DataFormat::fpCODE).toString();

                        QVariantList extensions = Ilwis::DataFormat::getFormatProperties(DataFormat::fpEXTENSION,outputtype, connector, code);

                        fileName += ".";
                        fileName += extensions[0].toString();
                    }

                    splitUrl.removeLast();

                    dataLocation = splitUrl.join("/");

                    directory = opendir(dataLocation.toStdString().c_str());
                }

                struct dirent *file;

                //Put the existing file names in a list for later use
                while ((file = readdir (directory)) != NULL) {
                    existingFileNames.push_back(file->d_name);
                }

                closedir(directory);

                //Check if a file with the same name already exist
                for(int j=0;j<existingFileNames.size();++j){
                    if(formatName == "Memory"){
                        if(existingFileNames[j] == output) {
                            duplicateFileNames = true;
                            em->addError(1, "Workflow did not execute duplicate name: " + output + ". Please change this name.");
                        }
                    }else{
                        if(existingFileNames[j] == fileName){
                            duplicateFileNames = true;
                            em->addError(1, "Workflow did not execute duplicate name: " + fileName + ". Please change this name.");
                        }
                    }
                }
            }

            if ( hasType(outputtype, itCOLUMN)){
                if ( formatName == "Memory"){
                    output = modifyTableOutputUrl(output, parms);
                }else
                    output = parms[0] + "[" + output + "]";
            }
            if ( formatName == "Keep original"){
                IIlwisObject obj;
                obj.prepare(parms[0], operationresource["pin_1_type"].toULongLong());
                if ( obj.isValid()){
                    IlwisTypes type = operationresource[pout].toULongLong();
                    QVariantList values = DataFormat::getFormatProperties(DataFormat::fpCODE,type,obj->provider());
                    if ( values.size() != 0){
                        format = "{format(" + obj->provider() + ",\"" + values[0].toString() + "\")}";
                    }else{
                        kernel()->issues()->log(QString("No valid conversion found for provider %1 and format %2").arg(obj->provider()).arg(IlwisObject::type2Name(type)));
                        return sUNDEF;
                    }
                }
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
                // if there is no path we extend it with a path unless the output is a new column, output is than the "old" table so no new output object
                if ( output.indexOf("://") == -1 )
                    output = context()->workingCatalog()->source().url().toString() + "/" + output + format;
                else
                    output = output + format;
            }else{
                if ( hasType(outputtype,itRASTER)){
                    format = "{format(stream,\"rastercoverage\")}";
                }else if (hasType(outputtype, itFEATURE)){
                    format = "{format(stream,\"featurecoverage\")}";
                }else if (hasType(outputtype, itTABLE | itCOLUMN)){
                    format = "{format(stream,\"table\")}";
                }else if (hasType(outputtype, itCATALOG)){
                    format = "{format(stream,\"catalog\")}";
                }else if (hasType(outputtype, itDOMAIN)){
                    format = "{format(stream,\"domain\")}";
                }else if (hasType(outputtype, itCOORDSYSTEM)){
                    format = "{format(stream,\"coordinatesystem\")}";
                }else if (hasType(outputtype, itGEOREF)){
                    format = "{format(stream,\"georeference\")}";
                }

                output = output + format;
            }
        }

        if(!allOutputsString.isEmpty()){
            allOutputsString.append(",");
        }
        allOutputsString += output;
    }

    if(!duplicateFileNames){
        if ( allOutputsString == "")
            expression = QString("script %1(%2)").arg(operationresource.name()).arg(expression);
        else
            expression = QString("script %1=%2(%3)").arg(allOutputsString).arg(operationresource.name()).arg(expression);

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
    QStringList parts= keyf.split(" ",QString::SkipEmptyParts);
    QString result;
    for(QString part : parts){
        if ( part.toLower() == "or")
            result += " or ";
        else if ( part.toLower() == "and")
            result += " and ";
        else {
            result += "keyword='" + part + "'";
        }

    }
    _currentOperations.clear();
    _operationsByKey.clear();
    _refresh = true;
    CatalogModel::filter(result);
    emit operationsChanged();
}
