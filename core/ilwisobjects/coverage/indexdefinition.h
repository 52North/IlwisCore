#ifndef INDEXDEFINITION_H
#define INDEXDEFINITION_H

#include "kernel_global.h"
#include "boost/container/flat_map.hpp"
#include "datadefinition.h"

namespace Ilwis {
class ItemRange;

const quint32 INDEX_NOT_PRESENT=1000000;




class KERNELSHARED_EXPORT IndexDefinition : public DataDefinition
{
public:


    struct TrackIndex{
        TrackIndex(double tvalue, quint64 fid) : _trackValue(tvalue), _featureid(fid) {}
        double _trackValue;
        quint64 _featureid;
    };

    struct LessTrack
    {
      bool operator()(const IndexDefinition::TrackIndex& val1, const IndexDefinition::TrackIndex& val2 ) const;
    };

    IndexDefinition();
    IndexDefinition(const IDomain& dm, Ilwis::Range *rng=0);

    quint32 operator()(quint64 fid, const QVariant& value) const{
        if ( !value.isValid())
            return 0;
        if ( value == COVERAGEATRIB)
            return COVERAGEATRIB;
        return indexRecord(fid, value);
    }
    quint32 indexSize() const;

    void addIndex(quint64 fid, const QVariant &tvalue, quint32 rec);
    QVariant byOrder(quint32 record);
    void removeIndex(quint64 fid, const QVariant &tvalue);
    quint32 indexRecord(quint64 fid, const QVariant &tvalue) const;

private:
    quint32 indexInternal(const QVariant &value) const;
    double key(const QVariant& value) const;
    boost::container::flat_map<Ilwis::IndexDefinition::TrackIndex, quint32, IndexDefinition::LessTrack> _index;
};

}

KERNELSHARED_EXPORT size_t hash(const Ilwis::IndexDefinition::TrackIndex& val);
KERNELSHARED_EXPORT bool operator==(const Ilwis::IndexDefinition::TrackIndex& val1, const Ilwis::IndexDefinition::TrackIndex& val2 );
KERNELSHARED_EXPORT bool operator<(const Ilwis::IndexDefinition::TrackIndex& val1, const Ilwis::IndexDefinition::TrackIndex& val2 );

#endif // INDEXDEFINITION_H
