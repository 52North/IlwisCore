#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "connectorinterface.h"
#include "table.h"
#include "tablemerger.h"

using namespace Ilwis;

TableMerger::TableMerger()
{
}

ITable TableMerger::mergeTables(const ITable& tbl1, const ITable& tbl2, int options) {
    std::vector<ColumnDefinition> newdefs;
    for(int c1 = 0; c1 < tbl1->columns(); ++c1) {
        auto coldef1 = tbl1->columndefinition(c1);
        for(int c2 = 0; c2 < tbl2->columns(); ++c2) {
            auto coldef2 = tbl2->columndefinition(c2);
            ColumnDefinition defnew = mergeColumnDefinitions(coldef1, coldef2);
            if ( defnew.isValid())
                newdefs.push_back(defnew);
        }
    }
    return ITable();
}

ColumnDefinition TableMerger::mergeColumnDefinitions(const ColumnDefinition &def1, const ColumnDefinition &def2) {
    if (def1.datadef().domain() == def2.datadef().domain()) {
        SPRange newRange = mergeRanges(def1.datadef().range(), def2.datadef().range());
    }
    return ColumnDefinition();
}

SPRange TableMerger::mergeRanges(const SPRange& rang1, const SPRange& rang2) {

}


