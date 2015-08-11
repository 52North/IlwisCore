#ifndef TABLEOPERATION_H
#define TABLEOPERATION_H

#include "kernel.h"
#include "ilwisdata.h"
#include "ilwiscoreui_global.h"

class TableModel;

namespace Ilwis {
class Table;
typedef IlwisData<Table> ITable;

namespace Desktop {


class ILWISCOREUISHARED_EXPORT TableOperation : public QObject, public Ilwis::Identity
{
    Q_OBJECT

    Q_PROPERTY(QString  name READ name CONSTANT)
    Q_PROPERTY(QString qmlUrl READ qmlUrl CONSTANT)
public:
    TableOperation(){}
    explicit TableOperation(const QString& name, const QUrl& ur, const QString& description, QObject *parent = 0);
    virtual bool canUse(TableModel *tblModel, const QVariantMap& parameters) const{ return false;}
    virtual void prepare(TableModel *tblModel);
    virtual void execute(const QVariantMap& parameters ) {}

    QString qmlUrl() const;

signals:

public slots:

protected:
    ITable table() const;
    TableModel *tableModel() const;
private:
    QUrl _qmlUrl;
    TableModel *_tblModel;

};
}
}

#endif // TABLEOPERATION_H
