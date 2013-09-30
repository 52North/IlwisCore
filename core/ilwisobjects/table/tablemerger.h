#ifndef TABLEMERGER_H
#define TABLEMERGER_H

namespace Ilwis {
class TableMerger
{
public:
    TableMerger();
    ITable mergeTables(const ITable &tbl1, const ITable &tbl2, int options);
private:
    ColumnDefinition mergeColumnDefinitions(const Ilwis::ColumnDefinition &def1, const Ilwis::ColumnDefinition &def2);
    SPRange mergeRanges(const SPRange &rang1, const SPRange &rang2);
};
}

#endif // TABLEMERGER_H
