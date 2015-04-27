#ifndef COLUMNMODEL_H
#define COLUMNMODEL_H

#include <QObject>
#include <QQmlListProperty>
#include "kernel.h"
#include "ilwisdata.h"
#include "attributemodel.h"
#include "tableoperations/tableoperation.h"
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
    QList<Ilwis::Desktop::TableOperation *> _operations;

};

#endif // COLUMNMODEL_H
