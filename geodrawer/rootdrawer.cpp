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

RootDrawer::RootDrawer(const IOOptions& options) : ComplexDrawer("RootDrawer",0,0, options)
{
    valid(true);

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

Envelope RootDrawer::zoomEnvelope() const
{
    return _zoomRect;
}

void RootDrawer::applyEnvelopeView(const Envelope &viewRect, bool overrule)
{
    if ( !_coverageRect.isValid() || _coverageRect.isNull()){
        ERROR2(ERR_NO_INITIALIZED_2,TR("Coverage area"), TR("Visualization"));
        return;
    }
    if ( !_pixelAreaSize.isValid()) {
        ERROR2(ERR_NO_INITIALIZED_2,TR("Pixel area"), TR("Visualization"));
        return;
    }
    double w = viewRect.xlength() - 1;
    double h = viewRect.ylength() - 1;
    _aspectRatioCoverage = w / h;
    Coordinate center = _coverageRect.center();
    if ( _aspectRatioCoverage <= 1.0) { // coverage is higher than it is wide)
        double dx = _aspectRatioView * h / 2.0;
        _zoomRect = Envelope(Coordinate(center.x - dx, center.y -  h / 2.0),Coordinate(center.x + dx, center.y +  h / 2.0));
    } else {
        double dy = _aspectRatioView * w / 2.0;
        _zoomRect = Envelope(Coordinate(center.x - w / 2.0, center.y - dy),Coordinate(center.x + w / 2.0, center.y + dy));
    }

    viewPoint(_zoomRect.center(), true);

    setMVP();

}

void RootDrawer::applyEnvelopeZoom(const Envelope &zoomRect)
{
    Envelope envelope = zoomRect;
    if ( _zoomRect.isValid()) {
    // zooming never changes the shape of the mapwindow so any incomming zoom rectangle must conform to the shape of the existing mapwindow

        double factCur = (_zoomRect.xlength() - 1.0) / (_zoomRect.ylength() - 1.0);
        double factIn = (zoomRect.xlength() - 1.0) / (zoomRect.ylength() - 1.0);
        if ( abs(factCur - factIn) > 0.01 ) {
            if ( factCur < 1.0) {
                double newHeight = (zoomRect.xlength() - 1) / factCur;
                envelope = Envelope(zoomRect.min_corner(), Coordinate(zoomRect.max_corner().x, zoomRect.min_corner().y + newHeight, zoomRect.max_corner().z));
            } else {
                double newWidth = (zoomRect.ylength() - 1) * factCur;
                envelope = Envelope(zoomRect.min_corner(), Coordinate(zoomRect.min_corner().x + newWidth, zoomRect.max_corner().y, zoomRect.max_corner().z));
            }
        }
    }
    _zoomRect = envelope;
    qDebug() << _zoomRect.xlength() - 1 << _zoomRect.ylength() - 1;
    viewPoint(_zoomRect.center(), true);
    setMVP();

}

void RootDrawer::setMVP()
{
    _projection.setToIdentity();
    QRectF rct(_zoomRect.min_corner().x, _zoomRect.min_corner().y,_zoomRect.xlength(),_zoomRect.ylength());
    _projection.ortho(rct);
    _mvp = _model * _view * _projection;

    if ( _coverageRect.xlength() > 0 && _coverageRect.ylength() > 0) {
        double xscale =  _zoomRect.xlength() / _coverageRect.xlength();
        double yscale = _zoomRect.ylength() /_coverageRect.ylength();

        _zoomScale =  std::min(xscale, yscale);
    }

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
//    auto env = normalizedEnveope(_zoomRect);
//   // qDebug() << "zoom" << env.toString() << env.xlength() * _pixelAreaSize.xsize() << env.ylength() * _pixelAreaSize.ysize();
//    env = normalizedEnveope(_coverageRect);
//    //qDebug() << "cov" << env.toString() << ((env.xlength() - 1) / 2.0) * _pixelAreaSize.xsize() << env.ylength() * _pixelAreaSize.ysize();
//    qDebug() << "cov" << ((env.xlength() - 1) / 2.0) * _pixelAreaSize.xsize() ;

    _pixelAreaSize = size;

}

Size<> RootDrawer::pixelAreaSize() const
{
    return _pixelAreaSize;
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
    ComplexDrawer::cleanUp();
}

bool RootDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options)
{
    return ComplexDrawer::prepare(prepType, options)    ;
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
    if ( attrName == "coverageenvelope"){
        QVariant var = qVariantFromValue(_coverageRect);
        return var;
    }

    if ( attrName == "zoomenvelope"){
        QVariant var = qVariantFromValue(_zoomRect);
        return var;
    }
    return QVariant();
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

