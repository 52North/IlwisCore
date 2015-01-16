#include "coverage.h"
#include "location.h"
#include "geos/geom/Coordinate.h"
#include "coordinate.h"
#include "box.h"
#include "coordinatesystem.h"
#include "rootdrawer.h"
#include "spatialdatadrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

RootDrawer::RootDrawer(QObject *parent) : ComplexDrawer("RootDrawer",0,0, parent)
{
    valid(true);

}

void RootDrawer::addDrawer(DrawerInterface *newdrawer, bool overrule)
{
    overrule = drawerCount(ComplexDrawer::dtMAIN) == 0 || overrule;
    SpatialDataDrawer *datadrawer = dynamic_cast<SpatialDataDrawer *>(newdrawer);
    if ( overrule && datadrawer && datadrawer->coverage().isValid()) {
        ICoordinateSystem cs = datadrawer->coverage()->coordinateSystem();
        Envelope envelope =  envelope2RootEnvelope(cs, datadrawer->envelope());
        Envelope viewEnv = _viewRect;
        bool setViewEnv =  envelope != viewEnv;
        viewEnv += envelope;
        _coverageRect = datadrawer->envelope();
        if ( setViewEnv)
            envelopeView(viewEnv, true);

    }
    return ComplexDrawer::addDrawer(newdrawer);
}

void RootDrawer::addEnvelope(const ICoordinateSystem &csSource, const Envelope &env, bool overrule)
{
    if ( overrule || !_coverageRect.isValid() || _coverageRect.isNull()) {
        Envelope newEnvelope = envelope2RootEnvelope(csSource, env);
        _coverageRect += newEnvelope;
    }
    envelopeView(_coverageRect, overrule);
}

Envelope RootDrawer::viewEnvelope() const
{
    return _viewRect;
}

void RootDrawer::envelopeView(const Envelope &viewRect, bool overrule)
{
    if ( !_coverageRect.isValid() || _coverageRect.isNull()){
        ERROR2(ERR_NO_INITIALIZED_2,TR("Coverage area"), TR("Visualization"));
        return;
    }
    if ( !_pixelAreaSize.isValid()) {
        ERROR2(ERR_NO_INITIALIZED_2,TR("Pixel area"), TR("Visualization"));
        return;
    }
    if ( overrule || !_viewRect.isValid()) {
        _aspectRatioCoverage = _coverageRect.xlength() / _coverageRect.ylength();
        double w = viewRect.xlength();
        double h = viewRect.ylength();
        if ( _aspectRatioCoverage <= 1.0) { // coverage is higher than it is wide
            double pixwidth = (double)_pixelAreaSize.ysize() * _aspectRatioCoverage; // height is dominant, so width in pixels is derived from height
            double deltay = 0;
            if ( pixwidth > _pixelAreaSize.xsize()) { // if the width is bigger than the available width
                deltay = viewRect.ylength() * ( pixwidth / _pixelAreaSize.xsize() - 1.0); // we need to reduce the ysize else the coverage goes outside the window
                pixwidth = _pixelAreaSize.xsize(); // pixelwidth becomes the max available number of pix in x
            }
            double fracofWidth = 1.0 - (_pixelAreaSize.xsize() - pixwidth) / _pixelAreaSize.xsize(); // fraction of the width of _pixelArea actually used
            double crdWidth = w / fracofWidth; // the actual width used in coordinates (x)
            double delta = (crdWidth - w) / 2.0; // the difference between the requested view width and the realized view width
            // new extent of the viewrectangle (coords)
            _viewRect =  Envelope(Coordinate(viewRect.min_corner().x - delta, viewRect.min_corner().y - deltay /2.0,0),
                                  Coordinate(viewRect.max_corner().x + delta,viewRect.max_corner().y + deltay/ 2.0,0));
        } else {
            double pixheight = (double)_pixelAreaSize.xsize() / _aspectRatioCoverage; // width is dominant, so height in pixels is derived from width
            double deltax = 0;
            if ( pixheight > _pixelAreaSize.ysize()) {
                deltax = viewRect.xlength() * ( pixheight / _pixelAreaSize.ysize() - 1.0);
                pixheight = _pixelAreaSize.ysize();
            }
            double fracofHeight = 1.0 - abs(_pixelAreaSize.ysize() - pixheight) / (double)_pixelAreaSize.ysize();
            double crdHeight = h / fracofHeight;
            double delta = (crdHeight - h) / 2.0;
            _viewRect =  Envelope(Coordinate(viewRect.min_corner().x - deltax /2.0,viewRect.min_corner().y  - delta,0),
                                  Coordinate(viewRect.max_corner().x + deltax /2.0,viewRect.max_corner().y  + delta,0));

        }
        _zoomRect = _viewRect;
        viewPoint(_viewRect.center(), true);

        setMVP();
    }
}

void RootDrawer::setMVP()
{
    _projection.setToIdentity();
    _projection.ortho(_zoomRect.min_corner().x, _zoomRect.max_corner().x,_zoomRect.min_corner().y,_zoomRect.max_corner().y, -1, 1);
    _mvp = _model * _view * _projection;
    unprepare(DrawerInterface::ptMVP); // we reset the mvp so for all drawers a new value has to be set to the graphics card
}

void RootDrawer::pixelAreaSize(const Size<> size)
{
    _aspectRatioView = (double)size.xsize() / (double)size.ysize();
    if ( _aspectRatioCoverage != 0 && !_viewRect.isNull() && !_zoomRect.isNull()){
        if ( _aspectRatioCoverage <= 1.0){
            if ( size.xsize() != _pixelAreaSize.xsize()){
                modifyEnvelopeZoomView(_viewRect.size().xsize(), _zoomRect.size().xsize(),(double)size.xsize() / _pixelAreaSize.xsize());
            }
            if ( size.ysize() != _pixelAreaSize.ysize()) { // make sure the zoomsize is changed if the cols change
                //modify_zoomRectView(_viewRect.size().xsize()(), _zoomRect.size().xsize()(),(double)_pixelAreaSize.ysize() / size.ysize());
                modifyEnvelopeZoomView(_viewRect.size().xsize(), _zoomRect.size().xsize(), (double)_pixelAreaSize.ysize() / (double)size.ysize() );
            }

        } else { // x < y
            if ( size.ysize() != _pixelAreaSize.ysize()){
                modifyEnvelopeZoomView(_viewRect.size().ysize(), _zoomRect.size().ysize(),(double)size.ysize() / _pixelAreaSize.ysize() );

            }
            if ( size.xsize() != _pixelAreaSize.xsize()) {
                modifyEnvelopeZoomView(_viewRect.size().ysize(), _zoomRect.size().ysize(),(double)_pixelAreaSize.xsize() / size.xsize() );
            }
        }

    }
    setMVP();
    _pixelAreaSize = size;
}

Size<> RootDrawer::pixelAreaSize() const
{
    return _pixelAreaSize;
}

void RootDrawer::modifyEnvelopeZoomView(double dview, double dzoom, double ratio) {
    double deltaview = dview * ratio;
    double deltazoom = dzoom * ratio;
    Coordinate cMiddle = _zoomRect.center();
    if ( _aspectRatioCoverage <= 1.0) {
        _viewRect.min_corner().x = cMiddle.x - deltaview / 2.0;
        _viewRect.max_corner().x = cMiddle.x + deltaview / 2.0;
        _zoomRect.min_corner().x = cMiddle.x - deltazoom / 2.0;
        _zoomRect.max_corner().x = cMiddle.x + deltazoom / 2.0;
    } else {
        _viewRect.min_corner().y = cMiddle.y - deltaview / 2.0;
        _viewRect.max_corner().y = cMiddle.y + deltaview / 2.0;
        _zoomRect.min_corner().y = cMiddle.y - deltazoom / 2.0;
        _zoomRect.max_corner().y = cMiddle.y + deltazoom / 2.0;
    }
}

const QMatrix4x4 &RootDrawer::mvpMatrix() const
{

    return _mvp;
}

const ICoordinateSystem &RootDrawer::coordinateSystem() const
{
    return _coordinateSystem;
}

void RootDrawer::coordinateSystem(const ICoordinateSystem &csy)
{
    _coordinateSystem = csy;
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

bool RootDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options, QOpenGLContext *openglContext)
{
    return ComplexDrawer::prepare(prepType, options, openglContext)    ;
}

Envelope RootDrawer::envelope2RootEnvelope(const ICoordinateSystem &csSource, const Envelope &env)
{
    Envelope envelope = (!_coordinateSystem.isValid() ||
                         !csSource.isValid() ||
                         _coordinateSystem->isEqual(csSource.ptr()) ||
                         _coordinateSystem->isUnknown() ||
                         csSource->isUnknown()) ?
                env : _coordinateSystem->convertEnvelope(csSource, env);

    return envelope;
}
