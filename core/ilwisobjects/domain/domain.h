#ifndef DOMAINDATA_H
#define DOMAINDATA_H

#include "Kernel_global.h"

namespace Ilwis {

class Domain;

typedef Ilwis::IlwisData<Ilwis::Domain> IDomain;

/*!
The domain class defines the semantics of the values used in Ilwis maps or tables. Domains range from numeric, to thematic
and time datatypes. Basically every datatype used in Ilwis is tied to a domain. Domains can be hierarchically organized meaning that
A domain maybe a generalization of other domains. The purpose of this is to be able to use subsets of domains in general cases. For example
the landcover classification system Corine is fairly large and subsetting seems (in relevant cases) reasonable. If the subset would
have no relation to the whole thematic domain (Corine) it would be impossible to reuse it in cases were a corine domain is exprected.
The strict flag limits this behavior. For the behaviour of the individual domain types see the relevant class.
 */
class KERNELSHARED_EXPORT Domain : public IlwisObject
{
public:
    Domain();
    Domain(const Resource& res);
    /*!
     return the strict status of the domain. When a domain is set to strict it will only compare domain properties
     to domain types that are identical to itself. Parent domain properties are ignored. For example if a operation only accepts
     a strict numeric domain min1to1, it will refuse a generic numeric domain as that may contain other (possible illegal) values.
     * \return the strict state.
     */
    bool isStrict() const;
    /*!
     sets the strict status of the domain. When a domain is set to strict it will only compare domain properties
     to domain types that are identical to itself. Parent domain properties are ignored. For example if a operation only accepts
     a strict numeric domain min1to1, it will refuse a generic numeric domain as that may contain other (possible illegal) values.
     * \param the new state
     */
    void setStrict(bool yesno);
    /*!
    /s IlwisObject
    */
    virtual IlwisTypes valueType() const = 0;
    virtual QString value(double v) const = 0;
    /*!
     returns the parent domain of the domain. This maybe an invalid domain when no parent is set. The child domain specializes (ad/or adds)
     some properties of the parent domain to create a more limited behavior. When not strict the parent domain can be used to
    compare properties. For example the default percentage domain has a value range of 0-100 but it is not strict. Other numeric domains maybe freely
    compared to this one as the parent domain ( the generic numeric domain) will accept all values.
     * \return the parent domain; it may be invalid
     */
    IDomain parent() const;
    /*!
     sets the parent domain of the domain. This maybe an invalid domain when no parent is set. The child domain specializes (ad/or adds)
     some properties of the parent domain to create a more limited behavior. When not strict the parent domain can be used to
    compare properties. For example the default percentage domain has a value range of 0-100 but it is not strict. Other numeric domains maybe freely
    compared to this one as the parent domain ( the generic numeric domain) will accept all values.
     * \sets the parent domain; it may be set to invalid
     */
    void setParent(const IDomain& dm);

    /*!
     translates the type of a variant to a compatible ilwis type
     * \param the variant to be typed
     * \return the corresponding ilwis type. This maybe unknown if the variant contains a type that is not part of the base system of Ilwis
     */
    static IlwisTypes ilwType(const QVariant &v);

private:
    bool _strict;
    IDomain _parentDomain;
};
}

Q_DECLARE_METATYPE(Ilwis::IDomain)

#endif // DOMAINDATA_H
