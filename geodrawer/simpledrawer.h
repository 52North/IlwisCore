#ifndef SIMPLEDRAWER_H
#define SIMPLEDRAWER_H

#include "basedrawer.h"

namespace Ilwis {
namespace Geodrawer{

class SimpleDrawer : public BaseDrawer
{
public:
    SimpleDrawer(const QString &name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer);

    bool isSimple() const ;

private:
    std::vector<DrawPosition>& drawPositions(); // never use
    std::vector<DrawColor> &drawColors(); // never use
};
}
}

#endif // SIMPLEDRAWER_H