#include <QThread>
#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "geometries.h"
#include "georeference.h"
#include "uicontextmodel.h"
#include "factory.h"
#include "abstractfactory.h"
#include "mastercatalogmodel.h"
#include "tableoperations/tableoperation.h"
#include "tableoperations/sortcolumn.h"
#include "tableoperations/tableoperationfactory.h"
#include "ilwiscontext.h"

quint64 UIContextModel::_objectCounter = 0;
std::unique_ptr<UIContextModel> UIContextModel::_uicontext;

UIContextModel::UIContextModel(QObject *parent) :
    QObject(parent)
{
    _abort.store(false);

}

LayerManager *UIContextModel::createLayerManager(const QString& objectName)
{
    QObject *object =_qmlcontext->findChild<QObject *>(objectName);
    QObject *newparent = object == 0 ? this : object;
    LayerManager *manager =  new LayerManager(newparent, this);

    return manager;
}

TableModel *UIContextModel::createTableModel(QObject *parent,const QString& url, const QString& type)
{

    IlwisTypes tp = IlwisObject::name2Type(type);
    Resource resource = mastercatalog()->name2Resource(url,tp);
    if ( resource.isValid()){
        return new TableModel(resource, parent);
    }
    return 0;
}

ChartModel *UIContextModel::chartModel(const QString &objectname)
{
    QObject *object =_qmlcontext->findChild<QObject *>(objectname);
    QObject *newparent = object == 0 ? this : object;
    ChartModel *chart =  new ChartModel(newparent);

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

QList<VisualAttributeEditor *> UIContextModel::propertyEditors(CoverageLayerModel *parentLayer, const IIlwisObject &obj, const ColumnDefinition &coldef)
{
    QList<VisualAttributeEditor *> editors;
    for ( const auto& editorItem : _propertyEditors){
        auto *editor = editorItem.second();
        if ( editor){
            if ( editor->canUse(obj, coldef)){
                editor->prepare(parentLayer, obj, coldef);
                editors.append(editor);
            }
            else
                delete editor;
        }
    }
    return editors;
}

QList<VisualAttributeEditor *> UIContextModel::propertyEditors(CoverageLayerModel *parentLayer, const IIlwisObject &obj, const QString& name){
    QList<VisualAttributeEditor *> editors;
    for ( const auto& editorItem : _propertyEditors){
        auto *editor = editorItem.second();
        if ( editor){
            if ( editor->canUse(obj, name)){
                editor->prepare(parentLayer, obj);
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
    Ilwis::Desktop::TableOperationFactory *factory = new Ilwis::Desktop::TableOperationFactory();
    factory->registerTableOperation("sortcolumn",Ilwis::Desktop::SortColumn::create);
    Ilwis::kernel()->addFactory(factory);

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
    _propertyEditors[propertyName] = func;

    return 0;
}

WorkSpaceModel *UIContextModel::currentWorkSpace() const
{
    return _currentWorkSpace;
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
                    QString wcUrl = context()->workingCatalog()->source().url().toString();
                    bool isWorkspace = wcUrl.indexOf("ilwis://internalcatalog/workspaces") == 0;
                    navbutton->setProperty("state" , isWorkspace ? "zerosize" : "fullsize");
                }
                emit currentWorkSpaceChanged();
            }
        }
    }
}




