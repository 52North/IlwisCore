#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "table.h"
#include "geometries.h"
#include "georeference.h"
#include "uicontextmodel.h"
#include "ilwiscontext.h"

quint64 UIContextModel::_objectCounter = 0;
std::unique_ptr<UIContextModel> UIContextModel::_uicontext;

UIContextModel::UIContextModel(QObject *parent) :
    QObject(parent)
{


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

QString UIContextModel::uniqueName()
{
    return "ilwis_ui_object_" + QString::number(_objectCounter++);
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
}

int UIContextModel::addPropertyEditor(const QString &propertyName, CreatePropertyEditor func)
{
    _propertyEditors[propertyName] = func;

    return 0;
}





