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
#include "uicontextmodel.h"
#include "factory.h"
#include "abstractfactory.h"
#include "mastercatalogmodel.h"
#include "tableoperations/tableoperation.h"
#include "visualattributemodel.h"
#include "tableoperations/sortcolumn.h"
#include "tableoperations/convertcolumndomain.h"
#include "tableoperations/tableoperationfactory.h"
#include "consolescriptmodel.h"
#include "dataformat.h"
#include "oshelper.h"
#include "ilwiscontext.h"

using namespace Ilwis;

quint64 UIContextModel::_objectCounter = 0;
std::unique_ptr<UIContextModel> UIContextModel::_uicontext;

UIContextModel::UIContextModel(QObject *parent) :
    QObject(parent)
{
    _abort.store(false);

    _consoles.push_back(new ConsoleScriptModel(this)); // console of mainwindow

}

LayerManager *UIContextModel::createLayerManager(const QString& objectName)
{
    QObject *object =_qmlcontext->findChild<QObject *>(objectName);
    QObject *newparent = object == 0 ? this : object;
    LayerManager *manager =  new LayerManager(newparent, this);

    return manager;
}

TableModel *UIContextModel::createTableModel(QObject *parent,const QString& filter, const QString& type)
{

    IlwisTypes tp = IlwisObject::name2Type(type);
    Resource resource;
    if ( filter.indexOf("itemid=") != -1){
        std::vector<Resource> res = mastercatalog()->select(filter);
        if ( res.size() != 1)
            return 0;
        resource = res[0];
    } else
        resource = mastercatalog()->name2Resource(filter,tp);
    if ( resource.isValid()){
        return new TableModel(resource, parent);
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
}

ConsoleScriptModel *UIContextModel::consoleScript(int type)
{
    if ( type < _consoles.size()){
        return _consoles[type];
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

void UIContextModel::addViewer(LayersViewCommandInterface *viewer, quint64 vid)
{
    _viewers[vid] = viewer;
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

void UIContextModel::currentKey(int ev)
{
    _currentKey = ev;
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
    Ilwis::kernel()->addFactory(factory);
    QString worldmp = OSHelper::createFileUrlFromParts(ilwisloc, "/resources/country_boundaries.ilwis");
    _worldMap.prepare(worldmp);
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
    QVariant mastercatalog = _qmlcontext->contextProperty("mastercatalog");
    if ( mastercatalog.isValid()){
        MasterCatalogModel *mcmodel = mastercatalog.value<MasterCatalogModel *>();
        if (mcmodel){
            return mcmodel;
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
        QUrl urlWorkingCatalog = context()->workingCatalog()->resource().url();
        QVariant ret;
        bool ok = QMetaObject::invokeMethod(datapane,"newPanel",Q_RETURN_ARG(QVariant,ret ),Q_ARG(QVariant, filter),Q_ARG(QVariant,"coverage"),Q_ARG(QVariant,url),Q_ARG(QVariant,side));
        if ( ok)
            ret.toLongLong();
    }
    return -1;
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
       formatList.append("Memory");
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

QString UIContextModel::worldmapCommand(const QString& id) const
{
    QString cmd = QString("adddrawer(%1,%2, \"itemid=%3\",featurecoverage)").arg(id).arg(_worldMap->resource().url().toString()).arg(_worldMap->id());

    return cmd;
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



