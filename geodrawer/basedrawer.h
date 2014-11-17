#ifndef BASEDRAWER_H
#define BASEDRAWER_H



#include "iooptions.h"
#include "drawerinterface.h"
#include "box.h"

namespace Ilwis {
namespace Geodrawer{

class BaseDrawer : public QObject, public DrawerInterface, public Identity
{
public:
    enum Containment { cINSIDE, cOUTSIDE, cUNKNOWN};

    bool prepare(DrawerInterface::PreparationType, const IOOptions&);
    bool draw(const IOOptions&) const;

    RootDrawer* rootDrawer() ;
    const RootDrawer *rootDrawer() const;
    DrawerInterface* parent();
    const DrawerInterface *parent() const;

    bool isActive() const ;
    void active(bool yesno);
    bool isValid() const;
    void selected(bool yesno);
    bool isSelected() const;
    Containment containment() const;
protected:
    BaseDrawer(const QString &name, DrawerInterface *parentDrawer, RootDrawer *rootdrawer);

    std::vector<DrawPosition> _positions;
    std::vector<DrawColor> _colors;

private:
    bool _active = true; // unless defined otherwise, the drawer is active
    bool _valid = false;
    bool _selected = false;
    RootDrawer* _rootDrawer = 0;
    DrawerInterface* _parentDrawer = 0;
    Envelope _envelope;




};
}
}

#endif // BASEDRAWER_H
