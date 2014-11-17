#ifndef DRAWINGCOLOR_H
#define DRAWINGCOLOR_H

namespace Ilwis {
namespace Geodrawer{

class SpatialDataDrawer;

class DrawingColor
{
public:
    DrawingColor(SpatialDataDrawer *spatialdrawer);

private:
   SpatialDataDrawer *_spatialdrawer;

   QColor clrVal(double rVal) const;
};
}
}

#endif // DRAWINGCOLOR_H
