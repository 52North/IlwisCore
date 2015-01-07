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

class PropertyEditorMetaData;
class ObjectVisualizationModel;

class ILWISCOREUISHARED_EXPORT UIContextModel : public QObject
{
    Q_OBJECT
public:
    explicit UIContextModel(QObject *parent = 0);

    Q_INVOKABLE VisualizationManager* createVisualizationManager(const QString& objectname);
    Q_INVOKABLE QString uniqueName();

    void addPropertyEditor(quint64 objecttype, const QString& propertyName, const PropertyEditorMetaData& metadata);
    QList<PropertyEditorMetaData *> propertyEditors(quint64 objecttype) ;

    void qmlContext(QQmlContext *ctx);
signals:

public slots:

private:
    std::map<quint64,std::map<QString, PropertyEditorMetaData *>> _propertyEditors;
    static quint64 _objectCounter;
    QQmlContext *_qmlcontext;

};



#endif // UICONTEXTMODEL_H
