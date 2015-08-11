#include <QColor>
#include "kernel.h"
#include "geometries.h"
#include "shapelookup.h"

using namespace Ilwis;


Shape::Shape()
{

}
//----------------------------
ShapeLookUp::ShapeLookUp()
{

}

ShapeLookUp::~ShapeLookUp()
{

}

Shape ShapeLookUp::shape(Raw raw)
{
    auto iter = _shapes.find(raw);
    if ( iter != _shapes.end())
        return (*iter).second;
    return Shape();
}

ShapeLookUp *ShapeLookUp::clone() const
{
    return 0;
}





