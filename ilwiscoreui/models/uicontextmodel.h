#ifndef UICONTEXTMODEL_H
#define UICONTEXTMODEL_H

#include <memory>
#include <QObject>
#include <QVariant>
#include <QQmlContext>
#include <Qt>
#include "iooptions.h"
#include "ilwiscoreui_global.h"
#include "layermanager.h"
#include "tablemodel.h"
#include "drawers/layersviewcommandinterface.h"

using namespace Ilwis;

class PropertyEditorObjectVisualizationModel;
class VisualAttributeEditor;
class LayerManager;
class CoverageLayerModel;

typedef std::function<VisualAttributeEditor *()> CreatePropertyEditor;

class ILWISCOREUISHARED_EXPORT UIContextModel : public QObject
{
    Q_OBJECT

    friend std::unique_ptr<UIContextModel>& uicontext();

    Q_PROPERTY(int activeSplit READ activeSplit WRITE setActiveSplit NOTIFY activeSplitChanged)
    Q_PROPERTY(int currentKey READ currentKey CONSTANT)
    Q_PROPERTY(QStringList colorNames READ colorNames CONSTANT)

public:
    explicit UIContextModel(QObject *parent = 0);

    Q_INVOKABLE LayerManager* createLayerManager(const QString& objectname);
    Q_INVOKABLE TableModel *createTableModel(QObject *parent,const QString& url, const QString& type);
    Q_INVOKABLE QString uniqueName();

    int addPropertyEditor(const QString& propertyName, CreatePropertyEditor func);
    QList<VisualAttributeEditor *> propertyEditors(CoverageLayerModel *parentLayer, const IIlwisObject &obj, const Ilwis::ColumnDefinition& datadef) ;
    QList<VisualAttributeEditor *> propertyEditors(CoverageLayerModel *parentLayer, const IIlwisObject &obj, const QString& name) ;

    void qmlContext(QQmlContext *ctx);
    void rootObject(QObject *root);
    QObject* rootObject() const;
    int activeSplit() const;
    void setActiveSplit(int index);
    void addViewer(LayersViewCommandInterface *viewer, quint64 vid);
    LayersViewCommandInterface *viewer(quint64 viewerid);
    void removeViewer(quint64 viewerid);
    void currentKey(int ev);
    int currentKey() const;
    QStringList colorNames() const;
    void prepare();

signals:
    void activeSplitChanged();

public slots:

private:



    std::map<QString, CreatePropertyEditor> _propertyEditors;
    std::map<quint64, LayersViewCommandInterface *> _viewers;
    static quint64 _objectCounter;
    QQmlContext *_qmlcontext;
    QObject *_rootObject = 0;
    int _activeSplit = 1;
    int _currentKey = 0;
    QStringList _colorNames;

    static std::unique_ptr<UIContextModel>_uicontext;

};

inline std::unique_ptr<UIContextModel>& uicontext(){
    if ( UIContextModel::_uicontext.get() == 0) {
        UIContextModel::_uicontext.reset( new UIContextModel());
    }
    return UIContextModel::_uicontext;
}


#endif // UICONTEXTMODEL_H
