#ifndef WORKFLOWCATALOGMODEL_H
#define WORKFLOWCATALOGMODEL_H

#include <QObject>
#include <QQmlListProperty>

#include "ilwiscoreui_global.h"

class ScenarioDesignerModel;

class ILWISCOREUISHARED_EXPORT ScenarioBuilderModel : public QObject
{
    Q_OBJECT
public:
    explicit ScenarioBuilderModel(QObject *parent = 0);

    Q_INVOKABLE ScenarioDesignerModel * create();

private:


signals:


public slots:

};


#endif // WORKFLOWCATALOGMODEL_H
