#ifndef COLUMNMODEL_H
#define COLUMNMODEL_H

#include <QObject>
#include <QQmlListProperty>
#include "kernel.h"
#include "ilwisdata.h"
#include "attributemodel.h"
#include "tableoperations/tableoperation.h"
#include "util/containerstatistics.h"
#include "ilwiscoreui_global.h"
namespace  Ilwis{
class Table;
typedef IlwisData<Table> ITable;
}

class ILWISCOREUISHARED_EXPORT ColumnModel : public AttributeModel
{
    Q_OBJECT

    Q_PROPERTY(bool isSelected READ isSelected WRITE selected NOTIFY isSelectedChanged)
    Q_PROPERTY(bool isSortColumn READ isSortColumn WRITE sortColumn NOTIFY isSortColumnChanged)
    Q_PROPERTY(QString role READ role CONSTANT)
    Q_PROPERTY(quint32 defaultWidth READ defaultWidth CONSTANT)
    Q_PROPERTY(quint32 columnIndex READ columnIndex CONSTANT)
    Q_PROPERTY(QString min READ min CONSTANT)
    Q_PROPERTY(QString max READ max CONSTANT)
    Q_PROPERTY(QString sum READ sum CONSTANT)
    Q_PROPERTY(QString median READ median CONSTANT)
    Q_PROPERTY(QString average READ average CONSTANT)
    Q_PROPERTY(QString stdev READ stdev CONSTANT)
    Q_PROPERTY(QString distance READ distance CONSTANT)
    Q_PROPERTY(QString variance READ variance CONSTANT)
    Q_PROPERTY(QString skew READ skew CONSTANT)
    Q_PROPERTY(QString kurtosis READ kurtosis CONSTANT)
    Q_PROPERTY(QQmlListProperty<Ilwis::Desktop::TableOperation> operations READ operations NOTIFY operationsChanged)
public:
    explicit ColumnModel();
    ColumnModel(TableModel *tblModel, quint32 index);
    ColumnModel(TableModel *tblModel,const QString& name, const QString &rle);

    QString attributename() const;
    QString role() const;
    bool isSelected() const;
    void selected(bool yesno);
    bool isSortColumn() const;
    void sortColumn(bool yesno);
    quint32 defaultWidth() const;
    quint32 columnIndex() const;
    QString min() const;
    QString max() const;
    QString average() const;
    QString median() const;
    QString distance() const;
    QString sum() const;
    QString stdev() const;
    QString variance() const;
    QString skew() const;
    QString kurtosis() const;
    QQmlListProperty<Ilwis::Desktop::TableOperation> operations();
    Q_INVOKABLE Ilwis::Desktop::TableOperation* operation(quint32 index);

signals:
    void isSelectedChanged();
    void isSortColumnChanged();
    void operationsChanged();

public slots:

private:
    QString _name = sUNDEF;
    QString _role = sUNDEF;
    quint32 _defaultWidth = 40;
    bool _selected = false;
    bool _sortColumn = false;
    quint32 _columnIndex;
    Ilwis::NumericStatistics _stats;
    QList<Ilwis::Desktop::TableOperation *> _operations;

};

#endif // COLUMNMODEL_H
