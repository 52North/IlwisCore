#include "coverage.h"
#include "location.h"
#include "geos/geom/Coordinate.h"
#include "coordinate.h"
#include "box.h"
#include "coordinatesystem.h"
#include "georeference.h"
//#include "cornersgeoreference.h"
#include "rootdrawer.h"
#include "spatialdatadrawer.h"
#include "griddrawer.h"
#include "layersrenderer.h"

using namespace Ilwis;
using namespace Geodrawer;

RootDrawer::RootDrawer(const QQuickFramebufferObject *fbo, const IOOptions& options) : ComplexDrawer("RootDrawer",0,0, options), _frameBufferObject(fbo)
{
    valid(true);
    _screenGrf = new GeoReference();
    _screenGrf->create("corners");
    _backgroundColor = "white";
    GridDrawer *griddrawer = new GridDrawer(this,this,options);
    addDrawer(griddrawer,DrawerInterface::dtPOST,iUNDEF,"GridDrawer");
}

RootDrawer::~RootDrawer()
{
    cleanUp();
}

void RootDrawer::addSpatialDrawer(DrawerInterface *newdrawer, bool overrule)
{
    overrule = drawerCount(ComplexDrawer::dtMAIN) == 0 || overrule;
    SpatialDataDrawer *datadrawer = dynamic_cast<SpatialDataDrawer *>(newdrawer);
    if ( overrule && datadrawer && datadrawer->coverage().isValid()) {
        ICoordinateSystem cs = datadrawer->coverage()->coordinateSystem();
        Envelope envelope =  envelope2RootEnvelope(cs, datadrawer->envelope());
        Envelope viewEnv = _zoomRect;
        bool setViewEnv =  envelope != viewEnv;
        viewEnv += envelope;
        _coverageRect = datadrawer->envelope();
        if ( setViewEnv)
            applyEnvelopeView(viewEnv, true);

    }
    return ComplexDrawer::addDrawer(newdrawer);
}

void RootDrawer::addEnvelope(const ICoordinateSystem &csSource, const Envelope &env, bool overrule)
{
    if ( overrule || !_coverageRect.isValid() || _coverageRect.isNull()) {
        Envelope newEnvelope = envelope2RootEnvelope(csSource, env);
        _coverageRect += newEnvelope;
    }
    applyEnvelopeView(_coverageRect, overrule);
}

Envelope RootDrawer::viewEnvelope() const
{
    return _viewEnvelope;
}

Envelope RootDrawer::zoomEnvelope() const
{
    return _zoomRect;
}

Envelope RootDrawer::coverageEnvelope() const
{
    return _coverageRect;
}

void RootDrawer::applyEnvelopeView(const Envelope &viewRect, bool overrule)
{
    if ( !_coverageRect.isValid() || _coverageRect.isNull()){
        return;
    }
    if ( !_pixelAreaSize.isValid()) {
        ERROR2(ERR_NO_INITIALIZED_2,TR("Pixel area"), TR("Visualization"));
        return;
    }
    if ( overrule)
        _viewEnvelope = Envelope();

    double w = viewRect.xlength() - 1;
    double h = viewRect.ylength() - 1;
    _aspectRatioCoverage = w / h;
    double deltax = 0, deltay = 0;
    Envelope env;
    if ( _aspectRatioCoverage <= 1.0) { // coverage is higher than it is wide)
        double pixwidth = (double)_pixelAreaSize.ysize() * _aspectRatioCoverage;
        if ( pixwidth > _pixelAreaSize.xsize()) {
            deltay = (_coverageRect.ylength() - 1) * ( pixwidth / _pixelAreaSize.xsize() - 1.0);
            pixwidth = _pixelAreaSize.xsize();
        }
        double fractioOffWidth = 1.0 - (_pixelAreaSize.xsize() - pixwidth) / (double)_pixelAreaSize.xsize();
        double crdWidth = w / fractioOffWidth;
        deltax = (crdWidth - w) / 2.0;

        _zoomRect = Envelope(Coordinate(viewRect.min_corner().x - deltax,viewRect.min_corner().y  - deltay/2.0,0),
                             Coordinate(viewRect.max_corner().x + deltax,viewRect.max_corner().y  + deltay/2.0,0));

    } else {
        double pixheight = _pixelAreaSize.xsize() / _aspectRatioCoverage;
        if ( pixheight > _pixelAreaSize.ysize()){
            deltax = (_coverageRect.xlength() - 1) * ( pixheight / _pixelAreaSize.ysize() - 1.0);; //_aspectRatioCoverage;
            pixheight = _pixelAreaSize.ysize();
        }
        double fractionOfHeight = 1.0 - std::abs(_pixelAreaSize.ysize() - pixheight)/(double)_pixelAreaSize.ysize();
        double crdHeight = h / fractionOfHeight;
        deltay = (crdHeight - h)/ 2.0;
        _zoomRect = Envelope(Coordinate(viewRect.min_corner().x - deltax /2.0,viewRect.min_corner().y  - deltay,0),
                             Coordinate(viewRect.max_corner().x + deltax /2.0,viewRect.max_corner().y  + deltay,0));
    }

    viewPoint(_zoomRect.center(), true);

    setMVP();

}

void RootDrawer::applyEnvelopeZoom(const Envelope &zoomRect)
{
    if ( zoomRect.area() == 1) // we dont zoom in on pointsize area
        return;

    Envelope envelope = zoomRect;
    if ( _zoomRect.isValid()) {
    // zooming never changes the shape of the mapwindow so any incomming zoom rectangle must conform to the shape of the existing mapwindow

        double factCur = (_zoomRect.xlength() - 1.0) / (_zoomRect.ylength() - 1.0);
        double factIn = (zoomRect.xlength() - 1.0) / (zoomRect.ylength() - 1.0);
        double delta = std::abs(factCur - factIn);
        if (  delta > 0.01 ) {
            if ( factCur < 1.0) {
                double newHeight = (zoomRect.xlength() - 1) / ( factCur * 2.0);
                Coordinate center = zoomRect.center();
                envelope = Envelope(Coordinate(zoomRect.min_corner().x, center.y - newHeight), Coordinate(zoomRect.max_corner().x, center.y + newHeight));
            } else {
                double newWidth = (zoomRect.ylength() - 1) * factCur / 2.0;
                Coordinate center = zoomRect.center();
                envelope = Envelope(Coordinate(center.x - newWidth, zoomRect.min_corner().y), Coordinate(center.x + newWidth, zoomRect.max_corner().y));
            }
        }
    }
    _zoomRect = envelope;
    viewPoint(_zoomRect.center(), true);
    setMVP();

}

void RootDrawer::setMVP()
{
    _projection.setToIdentity();
    QRectF rct(_zoomRect.min_corner().x, _zoomRect.min_corner().y,_zoomRect.xlength() -1,_zoomRect.ylength() - 1);
    _projection.ortho(rct);
    _mvp = _model * _view * _projection;

    if ( _coverageRect.xlength() > 0 && _coverageRect.ylength() > 0) {
        double xscale =  (_zoomRect.xlength() - 1) / (_coverageRect.xlength() - 1);
        double yscale = (_zoomRect.ylength() - 1) /(_coverageRect.ylength() - 1);

        _zoomScale =  std::min(xscale, yscale);
    }
    if ( _viewEnvelope.isNull() && _zoomRect.isValid() && !_zoomRect.isNull())
        _viewEnvelope = _zoomRect;

    _screenGrf->envelope(_zoomRect);
    _screenGrf->size(_pixelAreaSize);
    _screenGrf->compute();

    unprepare(DrawerInterface::ptMVP); // we reset the mvp so for all drawers a new value has to be set to the graphics card
}

void RootDrawer::pixelAreaSize(const Size<>& size)
{
    if ( size == _pixelAreaSize || size.ysize() == 0 || size.xsize() == 0)
        return;

    _aspectRatioView = (double)size.xsize() / (double)size.ysize();
    if ( _aspectRatioCoverage != 0 && !_zoomRect.isNull()){
        double fx = (double)size.xsize() / _pixelAreaSize.xsize();
        double fy = (double)size.ysize() / _pixelAreaSize.ysize();
        double w = _zoomRect.xlength() - 1;
        double h = _zoomRect.ylength() - 1;
        Coordinate center = _zoomRect.center();
        if (  size.xsize() != _pixelAreaSize.xsize() && size.ysize() != _pixelAreaSize.ysize()){
            double dx = w * fx;
            double dy = h * fy;
            _zoomRect = Envelope(Coordinate(center.x - dx / 2.0, center.y - dy / 2.0), Coordinate(center.x + dx / 2.0, center.y + dy / 2.0));

        }else {
            if ( size.xsize() != _pixelAreaSize.xsize())    {
                double dx = w * fx;
                _zoomRect = Envelope(Coordinate(center.x - dx / 2.0, center.y - h / 2.0), Coordinate(center.x + dx / 2.0, center.y + h / 2.0));
            }
            if ( size.ysize() != _pixelAreaSize.ysize())    {
                double dy = h * fy;
                _zoomRect = Envelope(Coordinate(center.x - w / 2.0, center.y - dy / 2.0), Coordinate(center.x + w / 2.0, center.y + dy / 2.0));
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

Size<> RootDrawer::coverageAreaSize() const
{
    if ( !_screenGrf.isValid())
        return Size<>();
    auto bb = _screenGrf->coord2Pixel(_coverageRect);
    return bb.size();
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

const IGeoReference &RootDrawer::screenGrf() const
{
    return _screenGrf;
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
    ComplexDrawer::cleanUp();
}

bool RootDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options)
{
    if(!ComplexDrawer::prepare(prepType, options))
        return false;

    if ( hasType(prepType, DrawerInterface::ptGEOMETRY) && !isPrepared(DrawerInterface::ptGEOMETRY)){
        if (!hasDrawer("GridDrawer", DrawerInterface::dtPOST)){
            drawer("GridDrawer", DrawerInterface::dtPOST)->prepare(DrawerInterface::ptALL, options);

        }
    }
    return true;
}

double RootDrawer::aspectRatioView() const
{
    return _aspectRatioView;
}

double RootDrawer::zoomScale() const
{
    return _zoomScale;
}

bool RootDrawer::is3D() const
{
    return _is3D;
}

void RootDrawer::set3D(bool yesno)
{
    _is3D = yesno;
}

const QQuickFramebufferObject *RootDrawer::fbo() const
{
    return _frameBufferObject;
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

DrawerInterface::DrawerType RootDrawer::drawerType() const
{
    return DrawerInterface::dtDONTCARE; // rootdrawer is never child of anything so it never is a pre,post, or main drawer. it is the root
}

QVariant RootDrawer::attribute(const QString &attrNme) const
{
    QString attrName = attrNme.toLower();
    QVariant var = ComplexDrawer::attribute(attrName);
    if ( var.isValid())
        return var;

    if ( attrName == "coordinatesystem"){
        QVariant var = qVariantFromValue(coordinateSystem());
        return var;
    }
    if ( attrName == "latlonenvelope"){
        QVariant var;
        if ( coordinateSystem()->isLatLon())
            var = qVariantFromValue(_coverageRect);
        else {
            ICoordinateSystem csyWgs84("code=epsg:4326");
            Envelope llEnvelope = csyWgs84->convertEnvelope(coordinateSystem(), _coverageRect);
            var = qVariantFromValue(llEnvelope);
        }
        return var;
    }
    if ( attrName == "coverageenvelope"){
        QVariant var = qVariantFromValue(_coverageRect);
        return var;
    }

    if ( attrName == "zoomenvelope"){
        QVariant var = qVariantFromValue(_zoomRect);
        return var;
    }
    if ( attrName == "pixelarea"){
        QVariant var = qVariantFromValue(_pixelAreaSize);
        return var;
    }
    if ( attrName == "backgroundcolor"){
        QVariant var = qVariantFromValue(_backgroundColor);
        return var;
    }
    return QVariant();
}

void RootDrawer::setAttribute(const QString &key, const QVariant &attribValue)
{
    ComplexDrawer::setAttribute(key, attribValue);
    if ( key == "backgroundcolor"){
        _backgroundColor = attribValue.value<QColor>();
    }
}

void RootDrawer::redraw()
{
    emit updateRenderer();
}

Ilwis::Coordinate RootDrawer::normalizedCoord(const Coordinate& crd) const{
    QVector3D v1( crd.x,crd.y, crd.is3D()? crd.z : 0);
    QVector3D a1 = _mvp * v1;
    return Ilwis::Coordinate(a1);
}

Envelope RootDrawer::normalizedEnveope(const Envelope& env) const
{
    Ilwis::Coordinate v1normalized =normalizedCoord(env.min_corner());
    Ilwis::Coordinate v2normalized = normalizedCoord(env.max_corner());
    return Envelope(v1normalized, v2normalized);
}

Ilwis::Coordinate RootDrawer::pixel2Coord(const Ilwis::Pixel& pix){
    if ( _screenGrf.isValid())
        return _screenGrf->pixel2Coord(pix);
    return Ilwis::Coordinate();
}

Ilwis::Pixel RootDrawer::coord2Pixel(const Ilwis::Coordinate& crd){
    if ( _screenGrf.isValid())
        return _screenGrf->coord2Pixel(crd);
    return Ilwis::Pixel();
}

