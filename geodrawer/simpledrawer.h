#ifndef SIMPLEDRAWER_H
#define SIMPLEDRAWER_H

#include "basedrawer.h"



namespace Ilwis{

namespace Geodrawer{

class SimpleDrawer : public BaseDrawer
{
public:
    SimpleDrawer(const QString &name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer);

    bool isSimple() const ;

private:
    std::vector<VertexPosition>& drawPositions(); // never use
};
}
}

#endif // SIMPLEDRAWER_H
