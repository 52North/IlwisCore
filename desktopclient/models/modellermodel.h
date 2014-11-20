#ifndef MODELERMODEL_H
#define MODELERMODEL_H

#include <QQmlListProperty>
#include <QColor>
#include "kernel.h"

#include <QObject>

class WorkflowModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString modelName READ modelName CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)

public:
    WorkflowModel();
    explicit WorkflowModel(const QString &name, const QString &desccription, QObject *parent);
    QString name() const;
     QString modelName() const;
    QString description() const;

private:
    QString _name;
    QString _description;
};

typedef QQmlListProperty<WorkflowModel> QMLModellerList;

class ModellerModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QMLModellerList workflows READ workflows NOTIFY workflowChanged)
    Q_INVOKABLE WorkflowModel* get(quint32 index) const;
    Q_INVOKABLE quint64 count() const;
    Q_INVOKABLE void removeWorkflow(quint32 index) const;

public:
    explicit ModellerModel(QObject *parent = 0);
    QMLModellerList workflows();

signals:
    void workflowChanged();

public slots:
    qint64 count();
    WorkflowModel* get(quint32 index);
    void addWorkflow(const QString &name, const QString &desccription);
    void removeWorkflow(quint32 index);

private:
   QList<WorkflowModel *> _models;

};

Q_DECLARE_METATYPE(QMLModellerList)

#endif // MODELERMODEL_H
