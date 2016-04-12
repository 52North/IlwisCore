#ifndef WORKSPACEMODEL_H
#define WORKSPACEMODEL_H

#include "catalogmodel.h"
#include "resourcemodel.h"
#include "operationmodel.h"

class ILWISCOREUISHARED_EXPORT WorkSpaceModel : public CatalogModel
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<ResourceModel> data READ data NOTIFY dataChanged)
    Q_PROPERTY(QQmlListProperty<OperationModel> operations READ operations NOTIFY operationsChanged)
    Q_PROPERTY(bool isDefault READ isDefault CONSTANT)

public:
    WorkSpaceModel(const QString& name="", QObject *parent = 0);
    ~WorkSpaceModel();

    Q_INVOKABLE void addItems(const QString &ids);
    Q_INVOKABLE void removeItems(const QString &ids);

    bool isDefault() const;

    QString modelType() const;
signals:
    void dataChanged();
    void operationsChanged();
protected:
    void gatherItems();
private:
    QQmlListProperty<OperationModel> operations();
    QQmlListProperty<ResourceModel> data();

    QList<OperationModel *> _operations;
    QList<ResourceModel *> _data;



};

#endif // WORKSPACEMODEL_H
