#ifndef OPERATIONSMODEL_H
#define OPERATIONSMODEL_H

#include "catalogmodel.h"
#include "operationmodel.h"
#include "operationsbykeymodel.h"
#include "operationExpression.h"
#include "ilwiscoreui_global.h"
#include "models/workflow/errormodel.h"

class OperationModel;
class OperationsByKeyModel;
class WorkflowModel;
namespace Ilwis {
class IOOptions;
}

class ILWISCOREUISHARED_EXPORT OperationCatalogModel : public CatalogModel
{
    Q_OBJECT
    Q_PROPERTY(QMLOperationList operations READ operations NOTIFY operationsChanged)
    Q_PROPERTY(QStringList keywords READ keywords NOTIFY operationsChanged)
    Q_PROPERTY(QString nameFilter READ nameFilter WRITE nameFilter NOTIFY operationsChanged)
    Q_PROPERTY(QString keyFilter READ keyFilter WRITE keyFilter NOTIFY operationsChanged)
    Q_PROPERTY(QQmlListProperty<OperationsByKeyModel> operationKeywords READ operationKeywords NOTIFY operationsByKeyChanged)
public:


    OperationCatalogModel(QObject * parent=0);
    OperationCatalogModel(const Ilwis::Resource& res,QObject *p);

   QMLOperationList operations();
   QQmlListProperty<OperationsByKeyModel> operationKeywords();
   void nameFilter(const QString&);
   Q_INVOKABLE void filter(const QString& filterString);
   void prepare(const Ilwis::IOOptions& options=Ilwis::IOOptions());

   Q_INVOKABLE quint64 operationId(const QString& name);
   Q_INVOKABLE quint64 operationId(quint32 index, bool byKey) const;
   Q_INVOKABLE quint64 serviceId(quint32 index) const;
   Q_INVOKABLE QStringList serviceNames() const;
   Q_INVOKABLE QString executeoperation(quint64 operationid, const QString &parameters, QVariant stepMode=QVariant());
   Q_INVOKABLE OperationModel *operation(const QString& id);
   Q_INVOKABLE WorkflowModel *createWorkFlow(const QString& filter);
   Q_INVOKABLE void refresh();

   void gatherItems();

   void keyFilter(const QString &keyf);
private:
    QList<OperationModel *> _currentOperations;
    QList<OperationsByKeyModel *> _operationsByKey;
    std::vector<Ilwis::Resource> _services;
    QStringList _keywords;
    bool _isGlobalOperationsCatalog = false;

    QStringList keywords() const;
    QString _keyFilter;
    QString _nameFilter;

    ErrorModel* em = ErrorModel::getInstance();

    QString keyFilter() const;
    QString nameFilter() const;
    void fillByName(QList<ResourceModel *> &currentOperations);
    void fillByKeyword(QList<ResourceModel*>& currentOperations);

public slots:
    void workSpaceChanged();

signals:
    void updateCatalog(const QUrl& url);
    void operationsChanged();
    void error(const QString& err);
    void operationsByKeyChanged();

};




#endif // OPERATIONSMODEL_H
