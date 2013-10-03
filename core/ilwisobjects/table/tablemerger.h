#ifndef TABLEMERGER_H
#define TABLEMERGER_H

namespace Ilwis {
class KERNELSHARED_EXPORT TableMerger
{
public:
    TableMerger();
    static ITable mergeTables(const ITable &tbl1, const ITable &tbl2, Ilwis::RenumberMap *renumberer=0);
private:
    static ColumnDefinition mergeColumnDefinitions(const Ilwis::ColumnDefinition &def1, const Ilwis::ColumnDefinition &def2, RenumberMap* renum=0);
    static SPRange mergeRanges(const SPRange &rang1, const SPRange &rang2);
};
}

#endif // TABLEMERGER_H
