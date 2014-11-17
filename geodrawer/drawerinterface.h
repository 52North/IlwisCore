#ifndef DRAWERINTERFACE_H
#define DRAWERINTERFACE_H

#include <memory>
#include <QColor>
#include "geos/geom/Coordinate.h"
#include "coordinate.h"
#include "location.h"
#include "identityinterface.h"

namespace Ilwis {
namespace Geodrawer{

struct DrawPosition {
    DrawPosition(float x=0, float y=0, float z=0) : _x(x), _y(y), _z(z){}
    DrawPosition(const Coordinate& crd) : _x(crd.x), _y(crd.y), _z(crd.z) {}

    float _x=0,_y=0,_z=0;
};

struct DrawColor {
    DrawColor(float red=0, float green=0, float blue=0, float alpha=1) : _red(red), _green(green), _blue(blue), _alpha(alpha) {}
    DrawColor(const QColor& clr) : _red(clr.redF()), _green(clr.greenF()), _blue(clr.blueF()),_alpha(clr.alphaF()) {}

    float _red=0, _green=0, _blue=0, _alpha=1;
};

class RootDrawer;
class DrawerInterface;
class IOOptions;

class DrawerInterface : public IdentityInterface{
public:
    enum PreparationType{ptNONE=0,ptRENDER=1,ptGEOMETRY=2,ptINITOPENGL=4,ptUI=8,pt3D=16,ptANIMATION=32,
                         ptRESTORE=64,ptOFFSCREENSTART=128,ptOFFSCREENEND=256,
                         ptREDRAW=512,ptNEWCSY=1024,ptALL=4294967295};

    DrawerInterface();
    virtual ~DrawerInterface();

    virtual bool draw(const IOOptions& options) const = 0;
    virtual bool prepare(PreparationType prepType, const IOOptions& options) = 0;

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

    virtual std::vector<DrawPosition>& drawPositions() = 0;
    virtual std::vector<DrawColor>& drawColors() = 0;


};

typedef std::unique_ptr<DrawerInterface> UPDrawer;

#define NEW_DRAWER(name) \
    private: \
static name *dummy_drawer;

#define REGISTER_DRAWER(name, classname) \
    name *name::dummy_drawer = DrawerFactory::registerDrawer(name, classname::create);
}

}
#endif // DRAWERINTERFACE_H
