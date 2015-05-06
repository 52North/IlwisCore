#ifndef WORKSPACEMODEL_H
#define WORKSPACEMODEL_H

#include "catalogmodel.h"

class ILWISCOREUISHARED_EXPORT WorkSpaceModel : public CatalogModel
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<ResourceModel> data READ data NOTIFY dataChanged)
    Q_PROPERTY(QQmlListProperty<ResourceModel> operations READ operations NOTIFY operationsChanged)

public:
    WorkSpaceModel(const QString& name="", QObject *parent = 0);
    ~WorkSpaceModel();

    Q_INVOKABLE void addItems(const QString &ids);
    Q_INVOKABLE void removeItems(const QString &ids);

signals:
    void dataChanged();
    void operationsChanged();
protected:
    void gatherItems();
private:
    QQmlListProperty<ResourceModel> operations();
    QQmlListProperty<ResourceModel> data();

    QList<ResourceModel *> _operations;
    QList<ResourceModel *> _data;



};

#endif // WORKSPACEMODEL_H
