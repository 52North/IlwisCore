#ifndef GRIDDRAWER_H
#define GRIDDRAWER_H

#include <QObject>
#include <QVariant>
#include "box.h"
#include "simpledrawer.h"
#include "complexdrawer.h"

namespace Ilwis{


namespace Geodrawer{

class GridDrawer : public ComplexDrawer {
public:
    GridDrawer(DrawerInterface* parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);

    bool prepare(PreparationType prepType, const IOOptions& options);


    DrawerInterface::DrawerType drawerType() const;
    quint32 defaultOrder() const;
    Envelope  envelope() const;



};

class SubGridDrawer : public SimpleDrawer
{
public:
    SubGridDrawer(const QString &gridname, DrawerInterface* parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);

    bool draw(const IOOptions& options=IOOptions()) ;
    bool prepare(PreparationType prepType, const IOOptions& options);

    void setAttribute(const QString& attrName, const QVariant& attrib);
    QVariant attribute(const QString &attrName) const;
    DrawerInterface::DrawerType drawerType() const;


protected:
    void calcEnvelope(Coordinate &cmin, Coordinate &cmax);

    QVector<QVector3D> _vertices;
    QVector<QVector3D> _normals;
    std::vector<VertexIndex> _indices;
    std::vector<VertexColor> _colors;
    QMatrix4x4 _view,_projection, _model, _mvp ;
    double _cellDistance = rUNDEF;
    QColor _lineColor;
    double _opacity = rUNDEF;
    float _linewidth = 1.0;
    ICoordinateSystem _latlon;

};

class PrimaryGridDrawer : public SubGridDrawer{
public:
    PrimaryGridDrawer(Ilwis::Geodrawer::DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);
    bool prepare(PreparationType prepType, const IOOptions& options);

};

class SecondaryGridDrawer : public SubGridDrawer{
public:
    SecondaryGridDrawer(Ilwis::Geodrawer::DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);
    bool prepare(PreparationType prepType, const IOOptions& options);
    void setAttribute(const QString& attrName, const QVariant& attrib);
    QVariant attribute(const QString &attrName) const;
    bool draw(const IOOptions &options);

private:
    int _cellCount = 4;

};
}
}

#endif // GRIDDRAWER_H
