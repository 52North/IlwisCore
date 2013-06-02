#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

namespace Ilwis {
class ControlPoint : public Coordinate{
public:
    ControlPoint(bool invalid=false);
    ControlPoint(const Coordinate& crd, const Pixel& pix);
    ControlPoint(const Coordinate& crd, const Pixel_d& pix );
    ControlPoint(const Coordinate& crd, const LatLon& ll);

    LatLon llLocation() const;
    void llLocation(const LatLon& ll);
    Pixel_d gridLocation() const;
    void gridLocation(const Pixel_d& pix);
    bool isActive() const;
    void active(bool yesno);
    bool isValid() const;
private:
    Pixel_d _gridLocation;
    LatLon  _llLocation;
    bool _active;
    bool _neverValid;
};
}

#endif // CONTROLPOINT_H
