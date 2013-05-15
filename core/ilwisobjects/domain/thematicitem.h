#ifndef THEMATICITEM_H
#define THEMATICITEM_H

#include "Kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT ThematicItem : public NamedIdentifier
{
public:

    ThematicItem();
    ThematicItem(const QString &name, const QString &cde=sUNDEF, const QString &descr=sUNDEF);
    QString itemType() const;

    QString description() const;
    void setDescription(const QString &desc);
    void setCode(const QString &code);

    void store(QDataStream &s, const SerializationOptions& opt) const;
    void load(QDataStream &s);

private:
    QString _description;
    QString _code;
};

typedef IlwisData<ItemDomain<ThematicItem> > IThematicDomain;

}

bool operator == (const Ilwis::ThematicItem& item1, const Ilwis::ThematicItem& item2);


#endif // THEMATICITEM_H
