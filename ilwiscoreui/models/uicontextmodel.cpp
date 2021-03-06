#include <QThread>
#include <QGenericReturnArgument>
#include "kernel.h"
#include "ilwisdata.h"
#include "ilwistypes.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "geometries.h"
#include "georeference.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "raster.h"
#include "uicontextmodel.h"
#include "factory.h"
#include "abstractfactory.h"
#include "mastercatalogmodel.h"
#include "tableoperations/tableoperation.h"
#include "visualattributemodel.h"
#include "tableoperations/sortcolumn.h"
#include "tableoperations/convertcolumndomain.h"
#include "tableoperations/columnvisibility.h"
#include "tableoperations/tableoperationfactory.h"
#include "dataformat.h"
#include "oshelper.h"
#include "operationmetadata.h"
#include "script.h"
#include "operationcatalogmodel.h"
#include "scriptmodel.h"
#include "ilwiscontext.h"

using namespace Ilwis;

quint64 UIContextModel::_objectCounter = 0;
std::unique_ptr<UIContextModel> UIContextModel::_uicontext;

UIContextModel::UIContextModel(QObject *parent) :
    QObject(parent)
{
    _abort.store(false);

}

LayerManager *UIContextModel::createLayerManager(QObject *parent)
{
    LayerManager *manager =  new LayerManager(parent, this);

    return manager;
}

TableModel *UIContextModel::createTableModel(QObject *parent,const QString& filter, const QString& type)
{

    IlwisTypes tp = IlwisObject::name2Type(type);
    Resource resource;
    if ( filter.indexOf("itemid=") != -1 || filter.indexOf("resource=") != -1){
        std::vector<Resource> res = mastercatalog()->select(filter);
        if ( res.size() != 1)
            return 0;
        resource = res[0];
    } else
        resource = mastercatalog()->name2Resource(filter,tp);
    if ( resource.isValid()){
        if ( resource.extendedType() == itRASTER){
            bool ok;
            quint64 rasterid = resource["rasterid"].toULongLong(&ok) ;
            if ( ok){
                IRasterCoverage raster;
                raster.prepare(rasterid);
                if ( raster.isValid() && raster->attributeTable().isValid())
                    return new TableModel(raster->attributeTable(), parent);
            }
        }else {
            return new TableModel(resource, parent);
        }
    }
    return 0;
}

ChartModel *UIContextModel::chartModel(const QString &objectname, TableModel *tbl)
{
    QObject *object =_qmlcontext->findChild<QObject *>(objectname);
    QObject *newparent = object == 0 ? this : object;
    ChartModel *chart =  new ChartModel(tbl,newparent);

    return chart;
}

QString UIContextModel::uniqueName()
{
    return "ilwis_ui_object_" + QString::number(_objectCounter++);
}

void UIContextModel::exitUI()
{
    _abort.store(true);
    if ( _threadCount > 0) {  // wait until the threads have aborted
        std::this_thread::sleep_for (std::chrono::seconds(3));
    }
    if ( masterCatalogModel() && masterCatalogModel()->currentCatalog()){
        masterCatalogModel()->currentCatalog()->viewRef().storeFilters();
    }

}
ScriptModel *UIContextModel::scriptModel(const QString& fileorid, QObject *parent)
{
    if ( fileorid == "")
        return new ScriptModel(_consoleScript, parent);
    bool ok;
    IScript scr;
    quint64 id = fileorid.toULongLong(&ok);
    if ( ok){
        Resource res = mastercatalog()->id2Resource(id);
        if ( res.isValid()){
            if ( res.ilwisType() == itSCRIPT && scr.prepare(res)){
                return new ScriptModel(scr, parent);
            }
            else if ( res.ilwisType() == itSINGLEOPERATION) {
                IOperationMetaData metadata;
                if ( metadata.prepare(res)){
                    return new ScriptModel(metadata, parent);
                }
            }
        }
    }else {
        Resource res(fileorid, itSCRIPT);
        scr.prepare(res);
        if ( scr.isValid()){
            return new ScriptModel(scr, parent);
        }
    }

    return 0;
}

QList<VisualAttributeEditor *> UIContextModel::propertyEditors(VisualAttributeModel *vattrib, const IIlwisObject &obj, const ColumnDefinition &coldef)
{
    QList<VisualAttributeEditor *> editors;
    for ( const auto& editorItem : _propertyEditors){
        auto *editor = editorItem.second();
        if ( editor){
            if ( editor->canUse(obj, coldef)){
                editor->prepare(vattrib, obj, coldef);
                editors.append(editor);
            }
            else
                delete editor;
        }
    }
    return editors;
}

QList<VisualAttributeEditor *> UIContextModel::propertyEditors(VisualAttributeModel *vattrib, const IIlwisObject &obj, const QString& name){
    QList<VisualAttributeEditor *> editors;
    for ( const auto& editorItem : _propertyEditors){
        auto *editor = editorItem.second();
        if ( editor){
            if ( editor->canUse(obj, name)){
                editor->prepare(vattrib,  obj);
                editors.append(editor);
            }
            else
                delete editor;
        }
    }
    return editors;
}

void UIContextModel::qmlContext(QQmlContext *ctx)
{
    _qmlcontext = ctx;
}

void UIContextModel::rootObject(QObject *root)
{
    _rootObject = root;
}

QObject *UIContextModel::rootObject() const
{
    return _rootObject;
}

QQmlContext* UIContextModel::rootContext() const
{
    return _qmlcontext;
}

int UIContextModel::activeSplit() const
{
    return _activeSplit;
}

void UIContextModel::setActiveSplit(int index)
{
    if ( index == 1 || index == 2){
        _activeSplit = index;
    }
}


void UIContextModel::currentKey(int ev)
{
    _currentKey += ev;
    int z = 0;
    int p = Qt::Key_L;

}

int UIContextModel::currentKey() const
{
    return _currentKey;
}

QStringList UIContextModel::colorNames() const
{
    return _colorNames;
}

void UIContextModel::prepare()
{
    QString ilwisloc = context()->ilwisFolder().absoluteFilePath();
    QString colornames = ilwisloc + "/resources/svgcolornames.txt";
    std::ifstream fstream;
    fstream.open(colornames.toStdString());
    if ( fstream.is_open()){
        char txt[100];
        while (fstream.getline(txt,100)){
            _colorNames.push_back(txt);
        }
    }

    QString colorcodes = ilwisloc + "/resources/color_chart.csv";
    std::ifstream fstream2;
    fstream2.open(colorcodes.toStdString());
    std::vector<QColor> colors;
    bool skip = true;
    if ( fstream2.is_open()){
        char txt[100];
        while (fstream2.getline(txt,100)){
            QStringList parts = QString(txt).split(",");
            if ( !skip){
                QColor clr(parts[1].toInt(), parts[2].toInt(), parts[3].toInt());
                colors.push_back(clr);
            }
            skip = false;
        }
    }
    int index = 0;
    QString code;
    for(int disk=6; disk >= 0 ; --disk){
        code = QString("%1").arg(6-disk,2,10,QChar('0')) + "0000";
        QColor clr = colors[index++];
        _colorCodes[code] = clr;
        for(int circle=1; circle <= disk; ++circle){
            for(int cell = 0; cell < 6*circle; ++cell){
                code = QString("%1%2%3").arg(6-disk,2,10,QChar('0')).arg(circle,2,10,QChar('0')).arg(cell,2,10,QChar('0'));
                QColor clr = colors[index++];
                _colorCodes[code] = clr;
            }

        }
    }
    Ilwis::Desktop::TableOperationFactory *factory = new Ilwis::Desktop::TableOperationFactory();
    factory->registerTableOperation("sortcolumn",Ilwis::Desktop::SortColumn::create);
    factory->registerTableOperation("convertcolumndomain",Ilwis::Desktop::ConvertColumnDomain::create);
    factory->registerTableOperation("columnvisibility",Ilwis::Desktop::ColumnVisibility::create);
    Ilwis::kernel()->addFactory(factory);
    QString rawUrlWorldMap = OSHelper::createFileUrlFromParts(ilwisloc, "/resources/country_boundaries.ilwis");

    QString url = QString("ilwis://system/coverages/country_boundaries.ilwis");
    Resource mapResource(url, rawUrlWorldMap, itFEATURE);
    mapResource.code("coverage:country_boundaries");
    mapResource.name("Country Boundaries", false);
    mapResource.setDescription(TR("Boundary map of all the nations in the world in LatLon Wgs84 projection"));


    url = QString("ilwis://system/scripts/consolescript.py");
    Resource scriptResource(url, itSCRIPT);
    scriptResource.code("script:console");
    scriptResource.name("Console", false);
    scriptResource.setDescription(TR("Generic scrupt resource that functions as backend script for the console when no specific script is called for"));

    mastercatalog()->addItems({mapResource, scriptResource}) ;
    _consoleScript.prepare(scriptResource);
    _worldMap.prepare(mapResource);

    kernel()->loadModulesFromLocation(context()->ilwisFolder().absoluteFilePath() + "/extensions/ui");
}

bool UIContextModel::abort() const
{
    return _abort;
}

void UIContextModel::updateThreadCount(int change)
{
    Ilwis::Locker<std::mutex> lock(_lock);
    _threadCount += change;
}

int UIContextModel::threadCount() const
{
    return _threadCount;
}

int UIContextModel::addPropertyEditor(const QString &propertyName, CreatePropertyEditor func)
{
    QStringList parts = propertyName.split("|");
    for(auto part : parts)
        _propertyEditors[part] = func;

    return 0;
}

WorkSpaceModel *UIContextModel::currentWorkSpace() const
{
    return _currentWorkSpace;
}

MasterCatalogModel *UIContextModel::masterCatalogModel() const
{
    if ( _qmlcontext){
        QVariant mastercatalog = _qmlcontext->contextProperty("mastercatalog");
        if ( mastercatalog.isValid()){
            MasterCatalogModel *mcmodel = mastercatalog.value<MasterCatalogModel *>();
            if (mcmodel){
                return mcmodel;
            }
        }
    }
    return 0;
}

OperationCatalogModel *UIContextModel::globalOperationCatalog() const
{
    if ( _qmlcontext){
        QVariant operations = _qmlcontext->contextProperty("operations");
        if ( operations.isValid()){
            OperationCatalogModel *operationsmodel = operations.value<OperationCatalogModel *>();
            if (operationsmodel){
                return operationsmodel;
            }
        }
    }
    return 0;
}

void UIContextModel::setCurrentWorkSpace(WorkSpaceModel *cws)
{
    if ( !_qmlcontext)
        return;
    if ( _currentWorkSpace && _currentWorkSpace->name() == cws->name())
        return;
    _currentWorkSpace = cws;
    QVariant mastercatalog = _qmlcontext->contextProperty("mastercatalog");
    if ( mastercatalog.isValid()){
        MasterCatalogModel *mcmodel = mastercatalog.value<MasterCatalogModel *>();
        if (mcmodel){
            quint32 index = mcmodel->workspaceIndex(cws->name());
            if ( index != iUNDEF){
                QObject *wscombo = rootObject()->findChild<QObject*>("workspace_combobox_mainui");
                if ( wscombo){
                    wscombo->setProperty("currentIndex",index);
                }
                QQuickItem *navbutton = rootObject()->findChild<QQuickItem*>("workbench_navbutton_mainui");
                if ( navbutton){
                    QString wcUrl = context()->workingCatalog()->resource().url().toString();
                    bool isWorkspace = wcUrl.indexOf("ilwis://internalcatalog/workspaces") == 0;
                    navbutton->setProperty("state" , isWorkspace ? "zerosize" : "fullsize");
                }
                emit currentWorkSpaceChanged();
            }
        }
    }
}

void UIContextModel::initializeDataPane()
{
     QObject *datapane = rootObject()->findChild<QObject*>("datapane_container_mainui");
     if ( datapane ){
         QUrl urlWorkingCatalog = context()->workingCatalog()->resource().url();
         QString filter="container='" + urlWorkingCatalog.toString() + "'";
         bool ok = QMetaObject::invokeMethod(datapane,"newCatalog",Q_ARG(QVariant, filter),Q_ARG(QVariant,"catalog"),Q_ARG(QVariant,urlWorkingCatalog.toString()),Q_ARG(QVariant,"left"));
         if ( !ok)
             qDebug() << "failed";
     }

}

qint64 UIContextModel::addMapPanel(const QString& filter, const QString& side, const QString url){
    QObject *datapane = rootObject()->findChild<QObject*>("datapane_mainui");
    if ( datapane ){
        Locker<std::mutex> lock(_mutexAddPanel);
        // the invoke goes to a different thread
        bool ok = QMetaObject::invokeMethod(datapane,"newPanel",Q_ARG(QVariant, filter),Q_ARG(QVariant,"coverage"),Q_ARG(QVariant,url),Q_ARG(QVariant,side));
        if (!ok)
            return i64UNDEF;
        // we want to retrieve the viewid for potential use else where; it is set in a different thread
        // the action might already been done in which case _lastaddid has a value; in case it is not done
        // we have to wait until the other thread frees the qwaitcondition
        if ( _lastAddedId.load() == Ilwis::i64UNDEF){
            _mutex4viewLock.lock();
            _wait4ViewCreate.wait(&_mutex4viewLock);
        }
        // it is now guaranteed that _lastaddedid has a value
        quint64 id =  _lastAddedId;
        // we reset it to undef as the value is safely stored in a local var
        _lastAddedId.store(Ilwis::i64UNDEF);
        return id + 1;
        // the outer lock is now freed and other threads may use the mechanism
    }
    return i64UNDEF;
}

void UIContextModel::addViewer(LayersViewCommandInterface *viewer, quint64 vid)
{
    _viewers[vid] = viewer;
    _lastAddedId.store(vid);
    _wait4ViewCreate.wakeAll();
}

LayersViewCommandInterface *UIContextModel::viewer(quint64 viewerid)
{
    auto iter = _viewers.find(viewerid);
    if ( iter != _viewers.end()){
        return (*iter).second;
    }
    return 0;
}

void UIContextModel::removeViewer(quint64 viewerid)
{
    auto iter= _viewers.find(viewerid)    ;
    if ( iter != _viewers.end()){
        _viewers.erase(iter);
    }
}


QString UIContextModel::ilwisFolder() const
{
    QFileInfo inf = context()->ilwisFolder();
    QUrl url = QUrl::fromLocalFile(inf.absoluteFilePath());
    return url.toString();
}

QString UIContextModel::showLastGeneratedResult() const
{
    return _lastGeneratedShowCommand;
}

void UIContextModel::showLastGeneratedResult(const QString &lgr)
{
    _lastGeneratedShowCommand = lgr;
    emit showLastGeneratedResultChanged();
}

QString UIContextModel::paleColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/palecolor",QString("#B9CEBD"));

    return sc;
}

void UIContextModel::paleColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/palecolor",clr);
        emit paleColorChanged();
    }
}

QString UIContextModel::middleColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/middlecolor",QString("#6CBFA1"));

    return sc;
}

void UIContextModel::middleColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/middlecolor",clr);
        emit middleColorChanged();
    }
}

QString UIContextModel::darkColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/darkcolor",QString("#2E8D63"));

    return sc;
}

void UIContextModel::darkColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/darkcolor",clr);
        emit darkColorChanged();
    }
}

QString UIContextModel::darkestColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/darkestcolor",QString("#256E4D"));

    return sc;
}

void UIContextModel::darkestColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/darkestcolor",clr);
        emit darkestColorChanged();
    }
}

QString UIContextModel::lightestColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/lightestcolor",QString("#EBF0EC"));

    return sc;
}

void UIContextModel::lightestColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/lightestcolor",clr);
        emit lightestColorChanged();
    }
}

QString UIContextModel::lightColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/lightcolor",QString("#DDE6DE"));

    return sc;
}

void UIContextModel::lightColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/lightcolor",clr);
        emit lightColorChanged();
    }
}

QString UIContextModel::lighterColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/lightercolor",QString("#E4EBE5"));

    return sc;
}

void UIContextModel::lighterColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/lightercolor",clr);
        emit lighterColorChanged();
    }
}

QString UIContextModel::workbenchBGColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/workbenchbgcolor",QString("white"));

    return sc;
}

void UIContextModel::workbenchBGColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/workbenchbgcolor",clr);
        emit workbenchBGColorChanged();
    }
}

QString UIContextModel::catalogBGColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/catalogbgcolor",QString("white"));

    return sc;
}

void UIContextModel::catalogBGColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/catalogbgcolor",clr);
        emit catalogBGColorChanged();
    }
}

QString UIContextModel::genericBGColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/genericbgcolor",QString("white"));

    return sc;
}

void UIContextModel::genericBGColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/genericbgcolor",clr);
        emit genericBGColorChanged();
    }
}

QString UIContextModel::actionHeaderColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/actionheadercolor",QString("#B9CEBD"));

    return sc;
}

void UIContextModel::actionHeaderColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/actionheadercolor",clr);
        emit actionHeaderColorChanged();
    }
}

QStringList UIContextModel::typeNames() const
{
    QStringList types;
    types.append(TypeHelper::type2name(itRASTER));
    types.append(TypeHelper::type2name(itFEATURE));
    types.append(TypeHelper::type2name(itCOVERAGE));
    types.append(TypeHelper::type2name(itTABLE));
    types.append(TypeHelper::type2name(itCOORDSYSTEM));
    types.append(TypeHelper::type2name(itPROJECTION));
    types.append(TypeHelper::type2name(itGEOREF));
    types.append(TypeHelper::type2name(itDOMAIN));
    types.append(TypeHelper::type2name(itITEMDOMAIN));
    types.append(TypeHelper::type2name(itNUMERICDOMAIN));
    types.append(TypeHelper::type2name(itNUMBER));
    types.append(TypeHelper::type2name(itSTRING));
    types.append(TypeHelper::type2name(itENVELOPE));
    types.append(TypeHelper::type2name(itBOUNDINGBOX));

    return types;
}

QString UIContextModel::actionColor() const
{
    QString sc = ilwisconfig("users/" + Ilwis::context()->currentUser() + "/actioncolor",QString("#eeeeee"));

    return sc;
}

void UIContextModel::actionColor(const QString &clr)
{
    QColor c(clr);
    if ( c.isValid()){
        context()->configurationRef().putValue("users/" + Ilwis::context()->currentUser() + "/actioncolor",clr);
        emit actionColorChanged();
    }
}



bool UIContextModel::debugMode() const
{
#ifdef QT_DEBUG
    return true;
#else
    return false;
#endif

}

QStringList UIContextModel::formatList(const QString& query, const QString& selectParm) const
{
    if ( selectParm == "format"){
        std::multimap<QString, Ilwis::DataFormat>  formats = Ilwis::DataFormat::getSelectedBy(Ilwis::DataFormat::fpCONNECTOR, query);
        QStringList formatList;
        for(auto &format : formats)    {
            QString formatstring = format.second.property(Ilwis::DataFormat::fpCONNECTOR).toString() + ":" + format.second.property(Ilwis::DataFormat::fpCODE).toString();
            formatList.append(formatstring);
        }
        return formatList;
    }

    bool ok;
    IlwisTypes ilwtype = selectParm.toULongLong(&ok);
    if (!ok){
        ilwtype = IlwisObject::name2Type(selectParm);
        if (ilwtype == itUNKNOWN)
            return QStringList();
    }
    if ( hasType(ilwtype,itFEATURE )){
        ilwtype = itFEATURE;
    }
    QString expr = query.arg(ilwtype);
    std::multimap<QString, Ilwis::DataFormat>  formats = Ilwis::DataFormat::getSelectedBy(Ilwis::DataFormat::fpNAME, expr);
    QStringList formatList;
    for(auto &format : formats)    {
        formatList.append(format.second.property(Ilwis::DataFormat::fpNAME).toString());
    }
    if ( formatList.size() != 0)
       formatList.append("Temporary");
    if ( hasType(ilwtype, itCOLUMN)){
        formatList.append("Keep original");
    }
    return formatList;
}

QObject *UIContextModel::getItem(const QString& name,QObject *parent)
{
    QQuickItem *item = 0;
    if ( parent == 0)
        item = rootObject()->findChild<QQuickItem*>(name);
    else
        item = parent->findChild<QQuickItem *>(name);

    return item;
}

QString UIContextModel::consoleScriptId() const
{
    if ( _consoleScript.isValid()){
        return QString::number(_consoleScript->id());
    }
    return QString::number(iUNDEF);
}

QVariantList UIContextModel::debugProperty(const QString &property)
{
    QVariantList results;
#ifdef QT_DEBUG
    std::multimap<QString, QVariantMap> items;
    if ( property == "references"){
        const QHash<quint64, ESPIlwisObject>& lookup = mastercatalog()->dumpLookup();

        for(auto object : lookup){
         //   ESPIlwisObject object = objectentry.second;
            QVariantMap mp;
            mp["name"] = object->name();
            mp["id"] = object->id();
            QVariant v;
            v.setValue(object.use_count());
            mp["referenceCount"] = v;
            mp["type"] = TypeHelper::type2name(object->ilwisType());
           items.insert({TypeHelper::type2name(object->ilwisType()), mp});
        }
    }
    for(auto pair : items){
        results.push_back(pair.second);
    }

#endif
    return results;
}

QString UIContextModel::type2icon(const QString &typeName)
{
    IlwisTypes tp = TypeHelper::name2type(typeName);
    return ResourceModel::iconPath(tp);
}

void UIContextModel::addMessage(const QString &message, const QString &type)
{
    IssueObject::IssueType tp = IssueObject::itNone;
    if ( type == "error"){
        tp = IssueObject::itError;
    }
    if ( type == "warning"){
        tp = IssueObject::itWarning;
    }
    if ( type == "message"){
        tp = IssueObject::itMessage;
    }
    if ( type == "debug"){
        tp = IssueObject::itDebug;
    }
    if ( type == "result"){
        tp = IssueObject::itRESULT;
    }
    kernel()->issues()->log(message,tp);
}

QString UIContextModel::worldmapCommand(const QString& id) const
{
    try{
        QString cmd = QString("adddrawer(%1,%2, \"itemid=%3\",featurecoverage)").arg(id).arg(_worldMap->resource().url().toString()).arg(_worldMap->id());

        return cmd;
    } catch (const ErrorObject&){}
    return "";
}

QColor UIContextModel::code2color(const QString &code) const
{
    auto iter = _colorCodes.find(code);
    if ( iter != _colorCodes.end()){
        return (*iter).second;
    }
    return QColor();
}

QString UIContextModel::type2typeName(const QString &tp) const
{
    return Ilwis::TypeHelper::type2HumanReadable(tp.toULongLong());
}

QString UIContextModel::typeName2typeId(const QString &nm) const
{
    return QString::number(Ilwis::IlwisObject::name2Type(nm));
}

QString UIContextModel::moduleData() const{
    QString modules;
    for(auto module : Ilwis::kernel()->modules()){
        modules+=module->getNameAndVersion() + "\n";
    }
    return modules;
}



