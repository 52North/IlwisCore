#ifndef OBJECTVISUALIZATIONMODEL_H
#define OBJECTVISUALIZATIONMODEL_H

#include <QQmlListProperty>
#include "resourcemodel.h"

class VisualizationManager;

class ObjectVisualizationModel : public ResourceModel
{
    Q_OBJECT

    Q_PROPERTY(QList<QString> properties READ properties CONSTANT)
public:
    ObjectVisualizationModel();
    ObjectVisualizationModel(const Ilwis::Resource& resource, QObject *obj=0);

    void setEditors(const VisualizationManager& manager);
    std::map<QString, QString> _propertyEditor;

    QList<QString> properties() const;
};

typedef QQmlListProperty<ObjectVisualizationModel> QMLObjectVisualizationList;
Q_DECLARE_METATYPE(QMLObjectVisualizationList)

#endif // OBJECTVISUALIZATIONMODEL_H
