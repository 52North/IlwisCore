#ifndef PYTHONAPI_VERTEXITERATOR_H
#define PYTHONAPI_VERTEXITERATOR_H

#include <memory>
#include "pythonapi_util.h"

namespace Ilwis{
class VertexIterator;
}


namespace pythonapi {

    class Geometry;

class VertexIterator
{
    friend class Feature;
public:
    VertexIterator();
    VertexIterator(const Geometry& geom);
    VertexIterator(const std::string& wkt);
    ~VertexIterator();

    bool __bool__() const;
    std::string __str__() const;
    VertexIterator* __iter__();
    Coordinate __next__();

    VertexIterator operator+(int n);
    VertexIterator __radd__(int n);
    VertexIterator* operator+=(int n);

    VertexIterator operator-(int n);
    VertexIterator __rsub__(int n);
    VertexIterator* operator-=(int n);

    Coordinate __getitem__(quint32 linearpos);
    Coordinate current() const;

    bool operator==(const VertexIterator& other) const;
    bool operator!=(const VertexIterator& other) const;
    bool operator<(const VertexIterator& other) const;
    bool operator>(const VertexIterator& other) const;
    bool operator<=(const VertexIterator& other) const;
    bool operator>=(const VertexIterator& other) const;

    bool nextSubGeometry() const;
    bool isInteriorVertex() const;

    VertexIterator begin();
    VertexIterator end();

protected:
    VertexIterator(Ilwis::VertexIterator* ilwIt);
    Ilwis::VertexIterator& ptr() const;
    std::shared_ptr<Ilwis::VertexIterator> _ilwisVertexIterator;
};
}
#endif // PYTHONAPI_VERTEXITERATOR_H
