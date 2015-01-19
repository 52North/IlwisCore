#ifndef UICONTEXTMODEL_H
#define UICONTEXTMODEL_H

#include <memory>
#include <QObject>
#include <QVariant>
#include <QQmlContext>
#include "iooptions.h"
#include "ilwiscoreui_global.h"
#include "visualizationmanager.h"

using namespace Ilwis;

class PropertyEditorObjectVisualizationModel;
class PropertyEditor;
class VisualizationManager;

typedef std::function<PropertyEditor *()> CreatePropertyEditor;

class ILWISCOREUISHARED_EXPORT UIContextModel : public QObject
{
    Q_OBJECT

    friend std::unique_ptr<UIContextModel>& uicontext();

    Q_PROPERTY(int activeSplit READ activeSplit WRITE setActiveSplit NOTIFY activeSplitChanged)
public:
    explicit UIContextModel(QObject *parent = 0);

    Q_INVOKABLE VisualizationManager* createVisualizationManager(const QString& objectname);
    Q_INVOKABLE QString uniqueName();

    int addPropertyEditor(quint64 objecttype, const QString& propertyName, CreatePropertyEditor func);
    QList<PropertyEditor *> propertyEditors(quint64 objecttype) ;

    void qmlContext(QQmlContext *ctx);
    int activeSplit() const;
    void setActiveSplit(int index);
signals:
    void activeSplitChanged();

public slots:

private:
    std::map<quint64,std::map<QString, CreatePropertyEditor>> _propertyEditors;
    static quint64 _objectCounter;
    QQmlContext *_qmlcontext;
    int _activeSplit = 1;

    static std::unique_ptr<UIContextModel>_uicontext;

};

inline std::unique_ptr<UIContextModel>& uicontext(){
    if ( UIContextModel::_uicontext.get() == 0) {
        UIContextModel::_uicontext.reset( new UIContextModel());
    }
    return UIContextModel::_uicontext;
}


#endif // UICONTEXTMODEL_H
