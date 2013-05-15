#ifndef ATTRIBUTETABLE_H
#define ATTRIBUTETABLE_H

namespace Ilwis {
class KERNELSHARED_EXPORT AttributeRecord
{
public:
    AttributeRecord(const ITable& attTable, const QString& keyColumn);

    quint32 columns() const;
    void indexKey();
    bool addColumn(const QString &name, const IDomain &domain);
    bool addColumn(const ColumnDefinition& def);
    ColumnDefinition columndefinition(const QString& nme) const;
    quint32 columnIndex(const QString& nme) const;
     QVariant cellByKey(quint64 itemId, const QString &col);
     void setTable(const ITable& tbl, const QString& keyColumn);
private:
    ITable _tableImpl;
    QString _keyColumn;
    std::unordered_map<quint32, quint32> _index;
};

typedef QSharedPointer<AttributeRecord> SPAttributeRecord;
}

#endif // ATTRIBUTETABLE_H
