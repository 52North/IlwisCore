#ifndef ATTRIBUTETABLE_H
#define ATTRIBUTETABLE_H

#include "selectabletable.h"

namespace Ilwis {

class FeatureCoverage;
typedef IlwisData<FeatureCoverage> IFeatureCoverage;

class KERNELSHARED_EXPORT AttributeTable : public SelectableTable
{
public:
    AttributeTable(FeatureCoverage *featureCoverage, int level);
    AttributeTable(const Resource& res);
    IlwisTypes ilwisType() const;

    void record(quint32 rec, const std::vector<QVariant> &vars, quint32 offset=0);
    std::vector<QVariant> column(const QString& columnName) const;
    QVariant cell(const QString& columnName, quint32 rec, bool asRaw=true) const;
    QVariant cell(const quint32 index, quint32 rec, bool asRaw=true) const ;

    quint32 recordCount() const;
    quint32 columnCount() const;
    void recordCount(quint32 r);

    bool createTable();
    bool addColumn(const QString &name, const IDomain &domain, const bool readonly=false);
    bool addColumn(const ColumnDefinition& def);
    bool addColumn(const QString &name, const QString& domainname,const bool readonly=false);
    void columndefinition(const ColumnDefinition& coldef);
    ColumnDefinition columndefinition(const QString& columnName) const;
    ColumnDefinition columndefinition(quint32 index) const;
    ColumnDefinition& columndefinitionRef(quint32 index);
    ColumnDefinition &columndefinitionRef(const QString &columnName);
    quint32 columnIndex(const QString& columnName) const;
    std::vector<QVariant> column(const QString& nme, quint32 start=0, quint32 stop=2e9) const;
    std::vector<QVariant> column(quint32 index, quint32 start=0, quint32 stop=2e9) const;
    void column(const QString& nme, const std::vector<QVariant>& vars, quint32 offset=0);
    void column(const quint32 columnIndex, const std::vector<QVariant>& vars, quint32 offset=0);

    void setCell(const QString& columnName, quint32 rec, const QVariant& var);
    void setCell(quint32 col, quint32 rec, const QVariant& var);


    Record& recordRef(quint32 n);
    const Record& record(quint32 n) const;

    ITable copyTable(const QString& name=sUNDEF) ;
    void featureCoverage(FeatureCoverage* f);


    void dataLoaded(bool yesno);
    virtual bool isDataLoaded() const;
    virtual void initValuesColumn(const QString& ) {}

    bool canUse(const IlwisObject *obj, bool strict=false) const ;
private:
    Record _dummy;
    Record& newRecord() { return _dummy ;}
    void removeRecord(quint32 rec){}


    IFeatureCoverage _features;
    quint32 _level = 0;
};

typedef IlwisData<AttributeTable> IAttributeTable;
}

#endif // ATTRIBUTETABLE_H
