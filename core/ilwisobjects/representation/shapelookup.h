#ifndef SHAPELOOKUP_H
#define SHAPELOOKUP_H

#include "boost/container/flat_map.hpp"

//TODO this is meant to be a more or less a port of the IVG stuff of Ilwis 3,
namespace Ilwis {
class KERNELSHARED_EXPORT Shape{

public:
    enum ShapeType{sRECTANGLE, sCIRCLE,sELLIPSE,sLINE,sPOLYLINE,sPOLYGON,sPATH, sCOMPOUND, sUNKNOWN};

    Shape();
private:
};

class ShapeLookUp
{
public:
    ShapeLookUp();
    ~ShapeLookUp();

    Shape shape(Raw raw);

    ShapeLookUp* clone() const;

private:
    boost::container::flat_map<Raw, Shape> _shapes;
};
}

#endif // SHAPELOOKUP_H
