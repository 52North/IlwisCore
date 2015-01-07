#ifndef OBJECTVISUALIZATIONMODEL_H
#define OBJECTVISUALIZATIONMODEL_H

#include <QQmlListProperty>
#include "resourcemodel.h"
#include "propertyeditormetadata.h"
#include "ilwiscoreui_global.h"

class VisualizationManager;

class ILWISCOREUISHARED_EXPORT ObjectVisualizationModel : public ResourceModel
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<PropertyEditorMetaData> propertyEditors READ propertyEditors NOTIFY propertyEditorChanged)


public:
    ObjectVisualizationModel();
    ObjectVisualizationModel(const Ilwis::Resource& resource, const QList<PropertyEditorMetaData *>& editors, QObject *obj=0);


private:
    QQmlListProperty<PropertyEditorMetaData> propertyEditors();
    QList<PropertyEditorMetaData *> _propertyEditors;

signals:
    void propertyEditorChanged();

};

typedef QQmlListProperty<ObjectVisualizationModel> QMLObjectVisualizationList;
Q_DECLARE_METATYPE(QMLObjectVisualizationList)

#endif // OBJECTVISUALIZATIONMODEL_H
