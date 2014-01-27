#ifndef GEOREFADAPTER_H
#define GEOREFADAPTER_H

namespace Ilwis {
class GeoRefAdapter
{
public:
    GeoRefAdapter();
    virtual Pixeld adaptBefore(const Pixeld& inPix) const = 0;
    virtual Pixeld adaptAfter(const Pixeld& inPix) const = 0;
    virtual ~GeoRefAdapter() {}
};

class FactorAdapter {
public:
    FactorAdapter(std::vector<double> &offsets, double fac);
    Pixeld adaptBefore(const Pixeld& inPix) const;
    Pixeld adaptAfter(const Pixeld& inPix) const;
private:
    std::vector<double> _offsets;
    double _factor;
};

class ScaleRotateAdapter {
public:
    ScaleRotateAdapter(const std::vector<double>& pivot, const std::vector<double>& targetOffet, double angle, double scale);
    Pixeld adaptBefore(const Pixeld& inPix) const;
    Pixeld adaptAfter(const Pixeld& inPix) const;
private:
    std::vector<double> _pivot;
    std::vector<double> _offset;
    double _angle;
    double _scale;
};
}

#endif // GEOREFADAPTER_H
