#ifndef ATTRIBUTETABLE_H
#define ATTRIBUTETABLE_H

#define hash32(x) ((x)*2654435761)
#define H_BITS 24 // Hashtable size
#define H_SHIFT (32-H_BITS)
#define COVERAGETABLE -1


namespace Ilwis {
class KERNELSHARED_EXPORT AttributeRecord
{
public:
    AttributeRecord();
    AttributeRecord(const ITable& attTable, const QString& keyColumn);

    quint32 columns(bool coverages=true) const;
    ColumnDefinition columndefinition(const QString& nme, bool coverages=true) const;
    quint32 columnIndex(const QString& nme, bool coverages=true) const;
    QVariant cellByKey(quint64 key, quint32 colIndex, int index=COVERAGETABLE);
    QVariant cellByRecord(quint64 record, quint32 colIndex, int index=COVERAGETABLE) const;
    QVariant cellByRecord(quint64 record, quint32 colIndex, const QVariant &var, int index);
    void cellByKey(quint64 key, quint32 colIndex, const QVariant &var, int index);
    void setTable(const ITable& tbl, const QString& keyColumn, int indexCount=COVERAGETABLE);
    bool isValid() const;
private:
    void indexKeyColumn();
    void indexVerticalIndex(int index);
    ITable _coverageTable;
    ITable _indexTable;
    QString _keyColumn;
    std::unordered_map<quint32, quint32> _coverageIndex;
    std::vector<std::unordered_map<quint32, quint32>> _verticalIndex;

};

typedef QSharedPointer<AttributeRecord> SPAttributeRecord;
}

#endif // ATTRIBUTETABLE_H
