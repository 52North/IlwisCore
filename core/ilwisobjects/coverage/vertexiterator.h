#ifndef VERTEXITERATOR_H
#define VERTEXITERATOR_H

namespace geos{
namespace geom {
class LineString;
class Geometry;
class CoordinateSequence;
class Coordinate;
}
}
namespace Ilwis {

typedef std::unique_ptr<geos::geom::Geometry> UPGeometry;

class KERNELSHARED_EXPORT VertexIterator : public std::iterator<std::random_access_iterator_tag, geos::geom::Coordinate>
{
public:
    VertexIterator();
    VertexIterator(geos::geom::Geometry *geom);
    VertexIterator(const UPGeometry &geom);
    VertexIterator(const QString& wkt);
    VertexIterator(const VertexIterator& iter);
    ~VertexIterator();

    VertexIterator& operator=(const VertexIterator& iter);

    VertexIterator& operator++();
    VertexIterator operator++(int n);
    VertexIterator& operator--();
    VertexIterator operator--(int n);
    VertexIterator& operator+=(int n);
    VertexIterator& operator-=(int n);
    geos::geom::Coordinate& operator[](quint32 n);
    bool operator==(const VertexIterator& iter) const;
    bool operator!=(const VertexIterator& iter) const;
    bool operator<(const VertexIterator& iter) const;
    bool operator>(const VertexIterator& iter) const;
    bool operator<=(const VertexIterator& iter) const;
    bool operator>=(const VertexIterator& iter) const;
    const geos::geom::Coordinate& operator*() const;
    geos::geom::Coordinate& operator*();
    geos::geom::Coordinate* operator->();
    VertexIterator operator-(int n) const;
    VertexIterator operator+(int n) const;
    bool nextSubGeometry() const;

private:
    void move(int n);
    void setFromGeometry(geos::geom::Geometry *geom);
    bool compatible(const VertexIterator& iter) const;
    std::vector<const geos::geom::CoordinateSequence *> _coordinates;
    std::vector<const geos::geom::Coordinate *> _pointCoordinates;
    bool _nextSubGeometry = false;

    int _index = 0;
    int _partIndex = 0;
    qint32 _linearSize = 0;
    qint32 _linearPosition = 0;
    bool _pointMode = false;
    bool _polygonMode = false;
    bool _hasOwnership = false;
    std::unique_ptr<geos::geom::Geometry> _internalGeom;


    void storeLineString(const geos::geom::LineString *cline, int index);
};

}

inline Ilwis::VertexIterator begin(std::unique_ptr<geos::geom::Geometry> &geom) {
    return Ilwis::VertexIterator(geom);
}

inline Ilwis::VertexIterator end(std::unique_ptr<geos::geom::Geometry> &geom) {
    Ilwis::VertexIterator iter(geom);
    iter += 100000000; //  at the end;
    return iter;
}

inline Ilwis::VertexIterator begin(Ilwis::VertexIterator &iter) {
    Ilwis::VertexIterator iterNew(iter);
    return iterNew - 10000000;
}

inline Ilwis::VertexIterator end(Ilwis::VertexIterator& iter) {
    Ilwis::VertexIterator iterNew(iter);
    iterNew += 100000000; //  at the end;
    return iterNew;
}

#endif // VERTEXITERATOR_H
