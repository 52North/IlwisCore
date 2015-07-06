#ifndef ROOTDRAWER_H
#define ROOTDRAWER_H

#include <memory.h>
#include <QMatrix4x4>
#include <QObject>
#include "kernel.h"
#include "ilwisdata.h"
#include "complexdrawer.h"

class QQuickFramebufferObject;

namespace Ilwis {

class CoordinateSystem;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;
class GeoReference;
typedef IlwisData<GeoReference> IGeoReference;

namespace Geodrawer{

class RootDrawer : public ComplexDrawer
{
    Q_OBJECT
public:
    explicit RootDrawer(const QQuickFramebufferObject *fbo, const IOOptions& options);
    ~RootDrawer();

    void addSpatialDrawer(DrawerInterface *newdrawer, bool overrule);
    void addEnvelope(const ICoordinateSystem& csSource, const Envelope& env, bool overrule);

    Envelope viewEnvelope() const;
    Envelope zoomEnvelope() const;
    Envelope coverageEnvelope() const;
    void applyEnvelopeView(const Envelope& viewRect, bool overrule);
    void applyEnvelopeZoom(const Envelope& zoomRect);
    void pixelAreaSize(const Size<> &size);
    Size<> pixelAreaSize() const;
    Size<> coverageAreaSize() const;
    const QMatrix4x4& mvpMatrix() const;
    const ICoordinateSystem& coordinateSystem() const;
    void coordinateSystem(const ICoordinateSystem& csy);
    const IGeoReference& screenGrf() const;

    void viewPoint(const Coordinate &viewCenter, bool setEyePoint=false);
    void cleanUp();
    bool prepare(PreparationType prepType, const IOOptions& options);

    double aspectRatioView() const;
    double zoomScale() const;
    bool is3D() const;
    void set3D(bool yesno);
    const QQuickFramebufferObject *fbo() const;

    DrawerInterface::DrawerType drawerType() const;

    QVariant attribute(const QString &attrNme) const;

    void redraw();

    Ilwis::Coordinate normalizedCoord(const Coordinate &crd) const;
    Envelope normalizedEnveope(const Envelope &env) const;
    Ilwis::Coordinate pixel2Coord(const Ilwis::Pixel &pix);
    Ilwis::Pixel coord2Pixel(const Ilwis::Coordinate &crd);
public slots:

private:
    QMatrix4x4 _view,_projection, _model, _mvp;
    ICoordinateSystem _coordinateSystem;
    Envelope _zoomRect; // extent of the portion of the map now visible in the visualization
    Envelope _coverageRect; // extent of coverage without any additional area
    Envelope _viewEnvelope; // original zoomrect; calculated only once and gives the size of the entiremap view
    Size<> _pixelAreaSize; // size of the area in the viewRect in pixels
    double _aspectRatioCoverage = 0; //ration between width/height of the coverage. determines how sides of a map will size in reaction to size changes
    double _aspectRatioView = 0;
    Coordinate _viewPoint;
    Coordinate _eyePoint;
    IGeoReference _screenGrf;
    double _zoomScale=1;
    bool _is3D = false;
    const QQuickFramebufferObject *_frameBufferObject;

    bool _useGeoref = false;

    Envelope envelope2RootEnvelope(const ICoordinateSystem& csSource, const Envelope& env);
    void setMVP();


};

typedef std::unique_ptr<RootDrawer> UPRootDrawer;
}
}

#endif // ROOTDRAWER_H
