#ifndef GEOREFADAPTER_H
#define GEOREFADAPTER_H

namespace Ilwis {
class GeoRefAdapter
{
public:
    GeoRefAdapter();
    virtual Pixel_d adaptBefore(const Pixel_d& inPix) const = 0;
    virtual Pixel_d adaptAfter(const Pixel_d& inPix) const = 0;
    virtual ~GeoRefAdapter() {}
};

class FactorAdapter {
public:
    FactorAdapter(std::vector<double> &offsets, double fac);
    Pixel_d adaptBefore(const Pixel_d& inPix) const;
    Pixel_d adaptAfter(const Pixel_d& inPix) const;
private:
    std::vector<double> _offsets;
    double _factor;
};

class ScaleRotateAdapter {
public:
    ScaleRotateAdapter(const std::vector<double>& pivot, const std::vector<double>& targetOffet, double angle, double scale);
    Pixel_d adaptBefore(const Pixel_d& inPix) const;
    Pixel_d adaptAfter(const Pixel_d& inPix) const;
private:
    std::vector<double> _pivot;
    std::vector<double> _offset;
    double _angle;
    double _scale;
};
}

#endif // GEOREFADAPTER_H
