#ifndef CTPGEOREFERENCE_H
#define CTPGEOREFERENCE_H

namespace Ilwis {



class CTPGeoReference : public GeoReference
{
public:
    CTPGeoReference();
    CTPGeoReference(const Resource& res);
    bool isValid() const;
    ControlPoint& controlPoint(quint32 index);
    quint32 nrControlPoints() const;
    qint32 index(const Pixel& pix) const;
    qint32 index(const Coordinate& crd) const;
    qint32 setControlPoint(const ControlPoint &pnt);
    bool compute();

private:
    std::vector<ControlPoint> _controlPoints;
    ControlPoint _invalidCP;
};
}

#endif // CTPGEOREFERENCE_H
