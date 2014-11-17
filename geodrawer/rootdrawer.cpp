#include "kernel.h"
#include "rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

RootDrawer::RootDrawer() : ComplexDrawer("RootDrawer",0,0)
{
}

Envelope RootDrawer::viewEnvelope() const
{
    return Envelope();
}
