#ifndef ROOTDRAWER_H
#define ROOTDRAWER_H

#include <memory.h>
#include <QMatrix4x4>
#include <QObject>
#include "kernel.h"
#include "ilwisdata.h"
#include "complexdrawer.h"

namespace Ilwis {

class CoordinateSystem;
typedef IlwisData<CoordinateSystem> ICoordinateSystem;

namespace Geodrawer{

class RootDrawer : public ComplexDrawer
{
    Q_OBJECT
public:
    explicit RootDrawer(const IOOptions& options);

    void newDrawer(DrawerInterface *newdrawer, bool overrule);
    void addEnvelope(const ICoordinateSystem& csSource, const Envelope& env, bool overrule);

    Envelope viewEnvelope() const;
    Envelope zoomEnvelope() const;
    void applyEnvelopeView(const Envelope& viewRect, bool overrule);
    void applyEnvelopeZoom(const Envelope& zoomRect);
    void pixelAreaSize(const Size<> &size);
    Size<> pixelAreaSize() const;
    const QMatrix4x4& mvpMatrix() const;
    const ICoordinateSystem& coordinateSystem() const;
    void coordinateSystem(const ICoordinateSystem& csy);

    void viewPoint(const Coordinate &viewCenter, bool setEyePoint=false);
    void cleanUp();
    bool prepare(PreparationType prepType, const IOOptions& options);

    double aspectRatioView() const;

    DrawerInterface::DrawerType drawerType() const;

signals:

public slots:

private:
    QMatrix4x4 _view,_projection, _model, _mvp;
    ICoordinateSystem _coordinateSystem;
    Envelope _zoomRect; // extent of the portion of the map now visible in the visualization
    Envelope _viewRect; // extent of the whole area that is covered by the visualization. this might be bigger than the map due to the aspect ratio of the map
    Envelope _coverageRect; // extent of coverage without any additional area
    Size<> _pixelAreaSize; // size of the area in the viewRect in pixels
    double _aspectRatioCoverage = 0; //ration between width/height of the coverage. determines how sides of a map will size in reaction to size changes
    double _aspectRatioView = 0;
    Coordinate _viewPoint;
    Coordinate _eyePoint;

    bool _useGeoref = false;

    Envelope envelope2RootEnvelope(const ICoordinateSystem& csSource, const Envelope& env);
    void modifyEnvelopeZoomView(double dv, double dz, double f);
    void setMVP();


//    QVector<QVector3D> vertices;
//    QVector<QVector3D> normals;
//    QOpenGLShaderProgram program1;
//    int vertexAttr1;
//    int normalAttr1;
//    int matrixUniform1;
//    void createGeometry2();
//    void initialize();
};

typedef std::unique_ptr<RootDrawer> UPRootDrawer;
}
}

#endif // ROOTDRAWER_H
