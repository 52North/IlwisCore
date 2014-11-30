#ifndef OPERATIONSBYKEYMODEL_H
#define OPERATIONSBYKEYMODEL_H

#include <QObject>
#include "operationcatalogmodel.h"

class OperationsByKeyModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMLOperationList operationsList READ operationsList CONSTANT)
    Q_PROPERTY(int operationCount READ operationCount CONSTANT)
    Q_PROPERTY(QString keyword READ keyword CONSTANT)

public:
    OperationsByKeyModel();
    explicit OperationsByKeyModel(const QString& kword,const std::vector<OperationModel *>& opers, QObject *parent = 0);

    QString keyword() const;
    QMLOperationList operationsList();
    int operationCount() const;

signals:

public slots:

private:
    QString _keyword;
    QList<OperationModel *> _operations;

};

#endif // OPERATIONSBYKEYMODEL_H
