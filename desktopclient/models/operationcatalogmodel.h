#ifndef OPERATIONSMODEL_H
#define OPERATIONSMODEL_H

#include "catalogmodel.h"
#include "operationmodel.h"


class OperationModel;

class OperationCatalogModel : public CatalogModel
{
    Q_OBJECT
    Q_PROPERTY(QMLOperationList operations READ operations CONSTANT)
public:


    OperationCatalogModel(QObject * parent=0);

   QMLOperationList operations();

   Q_INVOKABLE quint64 operationId(quint32 index) const;
   Q_INVOKABLE quint64 serviceId(quint32 index) const;
   Q_INVOKABLE QStringList serviceNames() const;
   Q_INVOKABLE QString executeoperation(quint64 operationid, const QString &parameters);
private:
    QList<OperationModel *> _currentOperations;
    std::vector<Ilwis::Resource> _services;

signals:
    void updateCatalog(const QUrl& url);
    void error(const QString& err);

};

#endif // OPERATIONSMODEL_H
