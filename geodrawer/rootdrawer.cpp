#include "kernel.h"
#include "rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

RootDrawer::RootDrawer(QObject *parent) : ComplexDrawer("RootDrawer",0,0, parent)
{
    valid(true);

    // for testing
    QMatrix4x4 model(1,0,0,0,
                     0,1,0,0,
                     0,0,1,0,
                     0,0,0,1);
    _model = model;
    _projection.ortho(QRect(-200,-1,202,2));
    _mvp = _model * _view * _projection;
    initializeOpenGLFunctions();
}

Envelope RootDrawer::viewEnvelope() const
{
    return Envelope();
}

void RootDrawer::coordBoundsView(const Envelope &viewRect, bool overrule)
{
    if ( overrule || !_viewRect.isValid()) {
        _aspectRatioCoverage = _coverageRect.xlength() / _coverageRect.ylength();
        double w = viewRect.xlength();
        double h = viewRect.ylength();
        if ( _aspectRatioCoverage <= 1.0) { // coverage is higher than it is wide
            double pixwidth = (double)_rasterSize.ysize() * _aspectRatioCoverage; // height is dominant, so width in pixels is derived from height
            double deltay = 0;
            if ( pixwidth > _rasterSize.xsize()) { // if the width is bigger than the available width
                deltay = viewRect.ylength() * ( pixwidth / viewRect.xlength() - 1.0); // we need to reduce the ysize else the coverage goes outside the window
                pixwidth = _rasterSize.xsize(); // pixelwidth becomes the max available number of pix in x
            }
            double fracofWidth = 1.0 - (_rasterSize.xsize() - pixwidth) / _rasterSize.xsize(); // fraction of the width of pixarea actually used
            double crdWidth = w / fracofWidth; // the actual width used in coordinates (x)
            double delta = (crdWidth - w) / 2.0; // the difference between the requested view width and the realized view width
            // new extent of the viewrectangle (coords)
            _viewRect =  Envelope(Coordinate(viewRect.min_corner().x - delta, viewRect.min_corner().y - deltay /2.0,0),
                                  Coordinate(viewRect.max_corner().x + delta,viewRect.max_corner().y + deltay/ 2.0,0));
        } else {
            double pixheight = (double)_rasterSize.xsize() / _aspectRatioCoverage; // width is dominant, so height in pixels is derived from width
            double deltax = 0;
            if ( pixheight > _rasterSize.ysize()) {
                deltax = viewRect.xlength() * ( pixheight / _rasterSize.ysize() - 1.0);
                pixheight = _rasterSize.ysize();
            }
            double fracofHeight = 1.0 - abs(_rasterSize.ysize() - pixheight) / (double)_rasterSize.ysize();
            double crdHeight = h / fracofHeight;
            double delta = (crdHeight - h) / 2.0;
            _viewRect =  Envelope(Coordinate(viewRect.min_corner().x - deltax /2.0,viewRect.min_corner().y  - delta,0),
                                  Coordinate(viewRect.max_corner().x + deltax /2.0,viewRect.max_corner().y  + delta,0));

        }
        _zoomRect = _viewRect;
        viewPoint(_viewRect.center(), true);
    }
}

void RootDrawer::rasterSize(const Size<> size)
{
    _rasterSize = size;


}

Size<> RootDrawer::rasterSize() const
{
    return _rasterSize;
}

const QMatrix4x4 *RootDrawer::mvpMatrix() const
{

    return &_mvp;
}

void RootDrawer::viewPoint(const Coordinate& viewCenter, bool setEyePoint){
    _viewPoint = viewCenter;
    if ( setEyePoint){
        _eyePoint.x = _viewPoint.x;
        _eyePoint.y = _viewPoint.y - _zoomRect.ylength() * 1.5;
        _eyePoint.z = _zoomRect.xlength() ;
    }
}

void RootDrawer::cleanUp()
{

}
