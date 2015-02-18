#ifndef DRAWERINTERFACE_H
#define DRAWERINTERFACE_H

#include <memory>
#include <QColor>
#include "ilwiscoreui_global.h"
#include <math.h>
#include "kernel.h"
#include "geos/geom/Coordinate.h"
#include "coordinate.h"
#include "location.h"
#include "ilwisdata.h"
#include "representation.h"
#include "identityinterface.h"
#include "iooptions.h"

class QOpenGLContext;

namespace Ilwis {

class IOOptions;

namespace Geodrawer{

struct ILWISCOREUISHARED_EXPORT VertexIndex {
    VertexIndex(quint32 start=0, quint32 count=0, IlwisTypes geomType=0, Raw vid=iUNDEF) : _start(start), _count(count), _geomtype(geomType), _objectid(vid){}
    quint32 _start;
    quint32 _count;
    IlwisTypes _geomtype;
    Raw _objectid = iUNDEF;
};

struct ILWISCOREUISHARED_EXPORT VertexColor {
    VertexColor(float r=0, float g=0, float b = 0, float a = 1.0) : _c1(r), _c2(g), _c3(b), _c4(a){}
    VertexColor(const QColor& clr) : _c1(clr.redF()), _c2(clr.greenF()), _c3(clr.blueF()), _c4(clr.alphaF()) {}
    float _c1=0, _c2=0, _c3=0, _c4=1.0;
};

class RootDrawer;
class DrawerInterface;

class ILWISCOREUISHARED_EXPORT DrawerInterface : public QObject, public IdentityInterface{
public:
    enum PreparationType{ptNONE=0,ptRENDER=1,ptGEOMETRY=2,ptINITOPENGL=4,ptUI=8,pt3D=16,ptANIMATION=32,
                         ptRESTORE=64,ptOFFSCREENSTART=128,ptOFFSCREENEND=256,
                         ptREDRAW=512,ptNEWCSY=1024,ptMVP=2048,ptSHADERS=4096, ptALL=4294967295};
    enum ColorValueMeaning{cvmFRACTION, cvmTRUEVALUE};
    enum DrawerType{dtDONTCARE=0, dtPOST=1, dtPRE=2, dtMAIN=4, dtALL=0xFFFFFFFF};

    DrawerInterface(QObject * parent=0);
    virtual ~DrawerInterface();

    virtual bool draw(const IOOptions& options=IOOptions()) = 0;
    virtual bool prepare(PreparationType prepType, const IOOptions& options) = 0;
    virtual void unprepare(PreparationType prepType) = 0;
    virtual bool isPrepared(quint32 type=ptALL) const = 0;

    virtual RootDrawer* rootDrawer() = 0;
    virtual const RootDrawer* rootDrawer() const = 0;
    virtual DrawerInterface *parent() = 0;
    virtual const DrawerInterface *parent() const = 0;

    virtual bool isActive() const = 0;
    virtual void active(bool yesno) = 0;
    virtual bool isSimple() const = 0;
    virtual bool isValid() const = 0;
    virtual bool isSelected() const = 0;
    virtual void selected(bool yesno) = 0;
    virtual void cleanUp() = 0;
    virtual QColor color(const IRepresentation& rpr, double value, ColorValueMeaning cvm = cvmTRUEVALUE) = 0;

    virtual std::vector<QVariant> attributes(const QString& attrNames) const = 0;
    virtual QVariant attribute(const QString& attrName) const = 0;
    virtual void setAttribute(const QString& attrName, const QVariant& attrib)  = 0;
    virtual bool drawerAttribute(const QString drawercode, const QString& attrName, const QVariant& attrib)  = 0;

    virtual quint32 defaultOrder() const = 0;
    virtual DrawerType drawerType()  const = 0;
};

typedef std::unique_ptr<DrawerInterface> UPDrawer;

#define NEW_DRAWER \
    private: \
static DrawerInterface *dummy_drawer;

#define REGISTER_DRAWER(classname) \
    DrawerInterface *classname::dummy_drawer = DrawerFactory::registerDrawer(#classname, classname::create);
}

}
#endif // DRAWERINTERFACE_H
