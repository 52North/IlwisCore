#ifndef UICONTEXTMODEL_H
#define UICONTEXTMODEL_H

#include <memory>
#include <QObject>
#include <QVariant>
#include <QQmlContext>
#include <QQuickItem>
#include <Qt>
#include <atomic>
#include "iooptions.h"
#include "ilwiscoreui_global.h"
#include "layermanager.h"
#include "tablemodel.h"
#include "workspacemodel.h"
#include "chartmodel.h"
#include "scriptmodel.h"
#include "drawers/layersviewcommandinterface.h"


namespace Ilwis{
class FeatureCoverage;
class Script;

typedef IlwisData<FeatureCoverage> IFeatureCoverage;
typedef IlwisData<Script> IScript;
}

using namespace Ilwis;

class PropertyEditorObjectVisualizationModel;
class VisualAttributeEditor;
class LayerManager;
class CoverageLayerModel;
class VisualAttributeModel;
class MasterCatalogModel;
class OperationCatalogModel;


typedef std::function<VisualAttributeEditor *()> CreatePropertyEditor;

class ILWISCOREUISHARED_EXPORT UIContextModel : public QObject
{
    Q_OBJECT

    friend std::unique_ptr<UIContextModel>& uicontext();

    Q_PROPERTY(int activeSplit READ activeSplit WRITE setActiveSplit NOTIFY activeSplitChanged)
    Q_PROPERTY(int currentKey READ currentKey CONSTANT)
    Q_PROPERTY(QStringList colorNames READ colorNames CONSTANT)
    Q_PROPERTY(WorkSpaceModel * currentWorkSpace READ currentWorkSpace WRITE setCurrentWorkSpace NOTIFY currentWorkSpaceChanged)
    Q_PROPERTY(QString ilwisFolder READ ilwisFolder CONSTANT)
    Q_PROPERTY(bool debugMode READ debugMode CONSTANT)

    Q_PROPERTY(QString paleColor READ paleColor WRITE paleColor NOTIFY paleColorChanged)
    Q_PROPERTY(QString middleColor READ middleColor WRITE middleColor NOTIFY middleColorChanged)
    Q_PROPERTY(QString darkColor READ darkColor WRITE darkColor NOTIFY darkColorChanged)
    Q_PROPERTY(QString darkestColor READ darkestColor WRITE darkestColor NOTIFY darkestColorChanged)
    Q_PROPERTY(QString lightColor READ lightColor WRITE lightColor NOTIFY lightColorChanged)
    Q_PROPERTY(QString lighterColor READ lighterColor WRITE lightColor NOTIFY lighterColorChanged)
    Q_PROPERTY(QString lightestColor READ lightestColor WRITE lightestColor NOTIFY lightestColorChanged)
    Q_PROPERTY(QString workbenchBGColor READ workbenchBGColor WRITE workbenchBGColor NOTIFY workbenchBGColorChanged)
    Q_PROPERTY(QString catalogBGColor READ catalogBGColor WRITE catalogBGColor NOTIFY catalogBGColorChanged)
    Q_PROPERTY(QString genericBGColor READ genericBGColor WRITE genericBGColor NOTIFY genericBGColorChanged)
    Q_PROPERTY(QString actionColor READ actionColor WRITE actionColor NOTIFY actionColorChanged)
    Q_PROPERTY(QString actionHeaderColor READ actionHeaderColor WRITE actionHeaderColor NOTIFY actionHeaderColorChanged)



public:
    explicit UIContextModel(QObject *parent = 0);

    Q_INVOKABLE LayerManager* createLayerManager(const QString& objectname);
    Q_INVOKABLE TableModel *createTableModel(QObject *parent,const QString& url, const QString& type);
    Q_INVOKABLE ChartModel *chartModel(const QString& objectname, TableModel *tbl);
    Q_INVOKABLE QString uniqueName();
    Q_INVOKABLE void exitUI();
    Q_INVOKABLE ScriptModel* scriptModel(const QString& fileorid, QObject *parent);
    Q_INVOKABLE QString moduleData() const;
    Q_INVOKABLE QString typeName2typeId(const QString& nm) const;
    Q_INVOKABLE QString worldmapCommand(const QString &id) const;
    Q_INVOKABLE QColor code2color(const QString& code) const;
    Q_INVOKABLE QString type2typeName(const QString& tp) const;
    Q_INVOKABLE QStringList formatList(const QString &query, const QString& ilwtype) const;
    Q_INVOKABLE QObject *getItem(const QString &name, QObject *parent);
    Q_INVOKABLE QString consoleScriptId() const;
    Q_INVOKABLE QVariantList debugProperty(const QString& property);
    Q_INVOKABLE QString type2icon(const QString& typeName);
    Q_INVOKABLE void addMessage(const QString& message, const QString& type);

    int addPropertyEditor(const QString& propertyName, CreatePropertyEditor func);
    QList<VisualAttributeEditor *> propertyEditors(VisualAttributeModel *vattrib, const IIlwisObject &obj, const Ilwis::ColumnDefinition& datadef) ;
    QList<VisualAttributeEditor *> propertyEditors(VisualAttributeModel *vattrib, const IIlwisObject &obj, const QString& name) ;

    void qmlContext(QQmlContext *ctx);
    void rootObject(QObject *root);
    QObject* rootObject() const;
    QQmlContext *rootContext() const;
    int activeSplit() const;
    void setActiveSplit(int index);
    void addViewer(LayersViewCommandInterface *viewer, quint64 vid);
    LayersViewCommandInterface *viewer(quint64 viewerid);
    void removeViewer(quint64 viewerid);
    void currentKey(int ev);
    int currentKey() const;
    QStringList colorNames() const;
    void prepare();
    bool abort() const;
    void updateThreadCount(int change);
    int threadCount() const;
    WorkSpaceModel *currentWorkSpace() const;
    MasterCatalogModel *masterCatalogModel() const;
    OperationCatalogModel *globalOperationCatalog() const;
    void setCurrentWorkSpace(WorkSpaceModel* cws);
    void initializeDataPane();
    qint64 addMapPanel(const QString &filter, const QString &side, const QString url);
    QStringList formatList() const;
    QString ilwisFolder() const;

signals:
    void activeSplitChanged();
    void currentWorkSpaceChanged();
    void paleColorChanged();
    void middleColorChanged();
    void darkColorChanged();
    void darkestColorChanged();
    void lightColorChanged();
    void lighterColorChanged();
    void lightestColorChanged();
    void workbenchBGColorChanged();
    void catalogBGColorChanged();
    void genericBGColorChanged();
    void actionColorChanged();
    void actionHeaderColorChanged();


public slots:

private:
    std::atomic<bool> _abort;
    std::mutex _lock;
    int _threadCount = 0;

    std::map<QString, CreatePropertyEditor> _propertyEditors;
    std::map<quint64, LayersViewCommandInterface *> _viewers;
    static quint64 _objectCounter;
    QQmlContext *_qmlcontext = 0;
    QObject *_rootObject = 0;
    int _activeSplit = 1;
    int _currentKey = 0;
    std::atomic<quint64> _lastAddedId;
    QStringList _colorNames;
    std::map<QString, QColor> _colorCodes;
    WorkSpaceModel *_currentWorkSpace = 0;
    IFeatureCoverage _worldMap;
    Ilwis::IScript _consoleScript; // script that used for console use
    std::mutex _mutexAddPanel;
    QMutex _mutex4viewLock;
    QWaitCondition _wait4ViewCreate;

    static std::unique_ptr<UIContextModel>_uicontext;

    QString paleColor() const;
    void paleColor(const QString& clr);
    QString middleColor() const;
    void middleColor(const QString& clr);
    QString darkColor() const;
    void darkColor(const QString& clr);
    QString darkestColor() const;
    void darkestColor(const QString& clr);
    QString lightColor() const;
    void lightColor(const QString& clr);
    QString lighterColor() const;
    void lighterColor(const QString& clr);
    QString lightestColor() const;
    void lightestColor(const QString& clr);
    QString workbenchBGColor() const;
    void workbenchBGColor(const QString& clr);
    QString catalogBGColor() const;
    void catalogBGColor(const QString& clr);
    QString genericBGColor() const;
    void genericBGColor(const QString& clr);
    QString actionColor() const;
    void actionColor(const QString& clr);
    QString actionHeaderColor() const;
    void actionHeaderColor(const QString& clr);


    bool debugMode() const;

};

inline std::unique_ptr<UIContextModel>& uicontext(){
    if ( UIContextModel::_uicontext.get() == 0) {
        UIContextModel::_uicontext.reset( new UIContextModel());
    }
    return UIContextModel::_uicontext;
}


#endif // UICONTEXTMODEL_H
