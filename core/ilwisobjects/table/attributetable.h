#ifndef ATTRIBUTETABLE_H
#define ATTRIBUTETABLE_H

namespace Ilwis {
class KERNELSHARED_EXPORT AttributeTable : public Table
{
public:
    AttributeTable();

    quint32 rows() const;
    quint32 columns() const;
    QVariantList record(quint32 n) const ;
    void record(quint32, const QVariantList& vars, quint32 offset=0);
    QVariantList column(const QString& nme) const;
    void column(const QString& nme, const QVariantList& vars, quint32 offset=0);
    QVariant cell(const QString& col, quint32 rec) const;
    void cell(const QString& col, quint32 rec, const QVariant& var);

     bool createTable();
     bool addColumn(const QString &name, const IDomain &domain);
     bool addColumn(const ColumnDefinition& def);
     ColumnDefinition columndefinition(const QString& nme) const;
     quint32 columnIndex(const QString& nme) const;

     QVariant cellByKey(quint64 itemId, const QString &col);
     void setTable(const ITable& tbl, const QString& keyColumn);
private:
    void indexKey();

    ITable _tableImpl;
    QString _keyColumn;
    std::unordered_map<quint32, quint32> _index;
};

typedef IlwisData<AttributeTable> IAttributeTable;
}

#endif // ATTRIBUTETABLE_H
