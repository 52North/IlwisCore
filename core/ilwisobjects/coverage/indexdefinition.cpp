
#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "interval.h"
#include "itemrange.h"
#include "intervalrange.h"
#include "datadefinition.h"
#include "indexdefinition.h"

using namespace Ilwis;

size_t hash(const Ilwis::IndexDefinition::TrackIndex& val){
    std::hash<double> h1;
    std::hash<quint64> h2;

    size_t v = h1(val._trackValue)/2 + h2(val._featureid)/2;

    return v;
}

bool operator==(const Ilwis::IndexDefinition::TrackIndex& val1, const Ilwis::IndexDefinition::TrackIndex& val2 ){
    return val1._featureid == val2._featureid && val1._trackValue == val2._trackValue;
}

inline bool operator<(const Ilwis::IndexDefinition::TrackIndex& val1, const Ilwis::IndexDefinition::TrackIndex& val2 ){
    if ( val1._featureid < val2._featureid)
        return true;

    return val1._trackValue < val2._trackValue;
}

bool IndexDefinition::LessTrack::operator()(const IndexDefinition::TrackIndex &val1, const IndexDefinition::TrackIndex &val2) const
{
    if ( val1._featureid < val2._featureid)
        return true;

    return val1._trackValue < val2._trackValue;
}

//--------------------------------------------------------
IndexDefinition::IndexDefinition() : DataDefinition()
{

}

IndexDefinition::IndexDefinition(const IDomain& dm, Ilwis::Range *rng) : DataDefinition(dm, rng) {
}

quint32 IndexDefinition::indexSize() const
{
    return _index.size();
}

void IndexDefinition::addIndex(quint64 fid, const QVariant& tvalue, quint32 rec)
{
    double value = key(tvalue);
    if ( value != rUNDEF)
        _index[TrackIndex(value, fid)] = rec;
}

void IndexDefinition::removeIndex(quint64 fid, const QVariant& tvalue)
{
    double value = key(tvalue);
    if ( value != rUNDEF){
        auto iter = _index.find(TrackIndex(value, fid));
        if(iter != _index.end())
            _index.erase(iter);
    }
}
quint32 IndexDefinition::indexRecord(quint64 fid, const QVariant &tvalue) const
{
    double value = key(tvalue);
    if ( value != rUNDEF){
        auto iter = _index.find(TrackIndex(value, fid));
        if ( iter != _index.end())
            return (*iter).second;
    }
    return iUNDEF;
}

double IndexDefinition::key(const QVariant &value) const
{
    double keyvalue = rUNDEF;
    if ( hasType(domain()->valueType(), itNUMBER )) {
        QVariant var = domain()->impliedValue(value);
        keyvalue = hasType(domain()->valueType(), itDATETIME) ? (double)var.value<Ilwis::Time>() : var.toDouble();
    }
    if ( hasType(domain()->ilwisType(), itITEMDOMAIN)) {
        SPItemRange rng = domain()->range<ItemRange>();
        if ( !rng->isValid())
            return rUNDEF;
        IlwisTypes valueType = domain()->valueType();
        if ( hasType(valueType, itNUMERICITEM )) {
            SPIntervalRange rng = domain()->range<IntervalRange>();
            keyvalue = rng->index(value.toDouble());
        }

        SPDomainItem domItem = rng->item(value.toString());
        if ( !domItem.isNull())
            keyvalue = domItem->raw();
    }

    return keyvalue;
}





