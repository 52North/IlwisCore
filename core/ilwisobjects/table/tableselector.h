#ifndef TABLESELECTOR_H
#define TABLESELECTOR_H

namespace Ilwis {
class TableSelector
{
    friend class FlatTable;

    TableSelector();
    static std::vector<quint32> select(const Table *tbl, const QString &conditions) ;
};
}

#endif // TABLESELECTOR_H
