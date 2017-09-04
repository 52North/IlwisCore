#ifndef PYTHONAPI_COLUMNDEFINITION_H
#define PYTHONAPI_COLUMNDEFINITION_H

#include <memory>
#include "kernel.h"

namespace Ilwis {
class ColumnDefinition;
}

namespace pythonapi{

class DataDefinition;
class Domain;

class ColumnDefinition{
    friend class Feature;
    friend class Table;
    friend class Coverage;
    friend class FeatureCoverage;
public:
    ColumnDefinition();
    ColumnDefinition(const ColumnDefinition& coldef, quint32 index);
    ColumnDefinition(const std::string& name, const DataDefinition& datdef, quint64 colindex);
    ColumnDefinition(const ColumnDefinition& coldef);
    ColumnDefinition(const std::string& nm, const Domain &dom, quint64 colindex);
    ~ColumnDefinition();

    DataDefinition &datadef();
    void columnindex(quint64 idx);
    quint64 columnindex() const;
    bool isChanged() const;
    void changed(bool yesno);

    std::string __str__();
    bool __bool__() const;

protected:
    ColumnDefinition(Ilwis::ColumnDefinition*);
    std::shared_ptr<Ilwis::ColumnDefinition> ptr() const;
    std::shared_ptr<Ilwis::ColumnDefinition> _ilwisColDef;
};

}

#endif // PYTHONAPI_COLUMNDEFINITION_H
