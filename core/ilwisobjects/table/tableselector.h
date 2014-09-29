#ifndef TABLESELECTOR_H
#define TABLESELECTOR_H

namespace Ilwis {
class TableSelector
{
    friend class SelectableTable;

    TableSelector();
    static std::vector<quint32> select(const Table *tbl, const QString &conditions) ;
    static void numericCase(const LogicalExpressionPart &part, double val1, std::vector<bool>::iterator &iter);
    static void stringCase(const LogicalExpressionPart &part, const Ilwis::ColumnDefinition &coldef,
                           const QVariant &val1, std::vector<bool>::iterator &iter);
};
}

#endif // TABLESELECTOR_H
