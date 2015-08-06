#include "kernel.h"
#include "ilwisdata.h"
#include "simpledrawer.h"
#include "rootdrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

SimpleDrawer::SimpleDrawer(const QString& nme, DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options) : BaseDrawer(nme, parentDrawer, rootdrawer, options)
{
}

QVariant SimpleDrawer::attributeOfDrawer(const QString &drawercode, const QString &attrName) const
{
    if ( name() == drawercode || code() == drawercode){
        return attribute(attrName);
    }
    return QVariant();
}

bool SimpleDrawer::drawerAttribute(const QString &drawercode, const QString &attrName, const QVariant &value)
{
    if ( name() == drawercode || code() == drawercode){
        setAttribute(attrName, value);
        return true;
    }
    return false;
}

bool SimpleDrawer::isSimple() const
{
    return true;
}




