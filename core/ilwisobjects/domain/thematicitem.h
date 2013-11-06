#ifndef THEMATICITEM_H
#define THEMATICITEM_H

#include "Kernel_global.h"

namespace Ilwis {

class KERNELSHARED_EXPORT ThematicItem : public NamedIdentifier
{
public:
    /**
     * Constructs an empty ThematicItem
     */
    ThematicItem();

    //ThematicItem(const QString &name, const QString &cde=sUNDEF, const QString &descr=sUNDEF);

    /**
     * Constructs a ThematicItem from a list of strings, in which the parts of this ThematicItem are specified
     * @param parts the list of strings or the parts
     */
    ThematicItem(const QStringList &parts);

    /**
     * Query for the description of this ThematicItem
     * @return the description
     */
    QString description() const;

    /**
     * Query for the code of this ThematicItem
     * @return the code
     */
    QString code() const;

    /**
     * Sets a new description to this ThematicItem
     * @param desc the new description
     */
    void description(const QString &desc);

    /**
     * Sets a new code for this ThematicItem
     * @param code the new code
     */
    void code(const QString &code);

    //@override
    DomainItem *clone() const;

    //@override
    static IlwisTypes valueTypeS();

    //@override
    IlwisTypes valueType() const;

private:
    QString _description;
    QString _code;
};

typedef IlwisData<ItemDomain<ThematicItem> > IThematicDomain;
typedef QSharedPointer<ThematicItem> SPThematicItem;

}

bool operator == (const Ilwis::ThematicItem& item1, const Ilwis::ThematicItem& item2);


#endif // THEMATICITEM_H
