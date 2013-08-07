#ifndef ATTRIBUTETABLE_H
#define ATTRIBUTETABLE_H

#define hash32(x) ((x)*2654435761)
#define H_BITS 24 // Hashtable size
#define H_SHIFT (32-H_BITS)


namespace Ilwis {
class KERNELSHARED_EXPORT AttributeRecord
{
public:
    AttributeRecord();
    AttributeRecord(const ITable& attTable, const QString& keyColumn);

    quint32 columns(bool coverages=true) const;
    bool addColumn(const QString &name, const IDomain &domain, bool coverages=true);
    bool addColumn(const ColumnDefinition& def, bool coverages=true);
    ColumnDefinition columndefinition(const QString& nme, bool coverages=true) const;
    quint32 columnIndex(const QString& nme, bool coverages=true) const;
    QVariant cellByKey(quint64 key, const QString &col, int index=-1);
    QVariant cellByIndex(quint64 index, quint32 colIndex, int zindex=-1);
    void setTable(const ITable& tbl, const QString& keyColumn, int indexCount=-1);
    bool isValid() const;
private:
    void indexCOVERAGEKEYCOLUMN();
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
