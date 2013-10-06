#ifndef TABLEMERGER_H
#define TABLEMERGER_H

namespace Ilwis {
class KERNELSHARED_EXPORT TableMerger
{
public:
    TableMerger();
    ITable mergeMetadataTables(const ITable &tbl1, const ITable &tbl2);
    void mergeTableData(const ITable &sourceTable1, const ITable &sourceTable2, ITable &targetTable) const;
private:
    std::map<QString, RenumberMap> _renumberers;
    std::map<QString, QString> _columnRenames;
    ColumnDefinition mergeColumnDefinitions(const Ilwis::ColumnDefinition &def1, const Ilwis::ColumnDefinition &def2, RenumberMap* renum=0);
};
}

#endif // TABLEMERGER_H
