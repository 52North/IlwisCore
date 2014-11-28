#ifndef LINEDRAWER_H
#define LINEDRAWER_H

#include "simpledrawer.h"

namespace Ilwis {
namespace Geodrawer{

class LineDrawer : public SimpleDrawer
{
public:
    LineDrawer(const QString &name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer);

    bool prepare(DrawerInterface::PreparationType prepType, const IOOptions&, QOpenGLContext *openglContext=0);
    bool draw(const IOOptions& options) const;

private:
    std::vector<Coordinate> _coords;

};
}
}

#endif // LINEDRAWER_H
