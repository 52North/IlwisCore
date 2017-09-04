#ifndef PYTHONAPI_DataDefinition_H
#define PYTHONAPI_DataDefinition_H

#include <memory>
#include "kernel.h"

namespace Ilwis{
class DataDefinition;
class Domain;
class Range;
}

namespace pythonapi {

class Domain;
class Range;

class DataDefinition{

    friend class RasterCoverage;
    friend class ColumnDefinition;

public:
    DataDefinition();
    ~DataDefinition();
    DataDefinition(const Domain& dm, Range *rng = 0);
    DataDefinition(const DataDefinition &datdef);
    std::string __str__();
    bool __bool__() const;
    void range(const Range &rng);
    void domain(const Domain &dm);
    bool isCompatibleWith(const DataDefinition& datdef);
    static DataDefinition* merge(const DataDefinition& datdef1, const DataDefinition& datdef2);
    DataDefinition* __set__(const DataDefinition& datdef);

    Range* range() const;
    Domain* domain() const;

protected:
    DataDefinition(Ilwis::DataDefinition* datdef);
    Ilwis::DataDefinition& ptr() const;
    std::shared_ptr<Ilwis::DataDefinition> _ilwisDatadef;
};

}

#endif // PYTHONAPI_DataDefinition_H
