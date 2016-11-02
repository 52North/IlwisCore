#ifndef SIMPLEDRAWER_H
#define SIMPLEDRAWER_H

#include "basedrawer.h"



namespace Ilwis{

namespace Geodrawer{

class ILWISCOREUISHARED_EXPORT SimpleDrawer : public BaseDrawer
{
public:
    SimpleDrawer(const QString &name, DrawerInterface* parentDrawer, RootDrawer *rootdrawer, const IOOptions &options);
    QVariant attributeOfDrawer(const QString& drawercode, const QString& attrName) const;
    bool drawerAttribute(const QString& , const QString &, const QVariant &);

    bool isSimple() const ;

private:
};
}
}

#endif // SIMPLEDRAWER_H
