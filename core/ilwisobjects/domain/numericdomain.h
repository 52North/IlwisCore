#ifndef VALUEDOMAINDATA_H
#define VALUEDOMAINDATA_H

#include "kernel_global.h"
#include "domain.h"

namespace Ilwis {

class NumericRange;
/**
 * A Numericdomain has a range of numeric items,<br>
 * Numeric items are ordered  items that have numeric sub-ranges. For example 0-100,100-200,300-400.
 *
 *\sa Domain
 *\sa ItemDomain
 */
class KERNELSHARED_EXPORT NumericDomain : public Domain
{
public:
    /*!
     * Creates an empty NumericDomain
     */
    NumericDomain();

    /*!
     * The constructor for a NumericDomain with a a Resource that is registered in the mastercatalog.<br>
     * This resource must have a name and an Id.<br>
     * Code and description are optional.
     *
     * \param resource Resource to be used
     */
    NumericDomain(const Resource& resource);
    NumericDomain(NumericRange *r);

    /*!
     * Changes the Range of this NumericDomain.<br>
     * This function only works if the readonly flag has not been set and the new range has to contain numericitems.(be a NumericItemRange)
     *
     * \sa NumericItemRange
     * \param vr The new range
     */
    void range(Range *vr);

    /*!
     * still has to been finished, not yet done
     * \return QVariant.toString
     */
    QVariant impliedValue(const QVariant&) const;

    /*!
     * Checks if the value is contained in either this domain or the parentdomain.
     * Has no preconditions.
     * \sa Domain
     * \param val The value to be checked
     * \return cPARENT when the value is contained in the parent and this domain is not strict, cSELF if the value is contained in this domain and cNONE if the value is contained in neither
     */
    Domain::Containement contains(const QVariant &value) const;

    /*!
     * Checks if this NumericDomain is compatible  with another IDomain,<br>
     * will not be compatible if the other domain is:<br>
     * - invalid
     * - not a NumericDomain
     *
     * \param dom the domain to be tested
     * \return true when compatible
     */
    bool isCompatibleWith(const Ilwis::IlwisObject *obj, bool strict=false) const;
    bool isOrdered() const;

    //@override
    IlwisTypes valueType() const;


    //@override
    IlwisTypes ilwisType() const;

    /*!
     * Sets a new parent on this domain, the parent has to be valid and so does this domain
     * \param dm the new parentdomain
     */
    void setParent(const IDomain &dm);
    IlwisObject *clone() ;
    void copyTo(IlwisObject *obj);

    /*!
     * Determines the Standard name for this NumericDomain with the given parameters
     * The standard name is used to differentiate between different NumericDomains
     *
     * The name can be:
     * - "value" : generic real value domain
     * - "image" : 0- 8 bits integer
     * - "image16" : 0- 16 bits integer
     * - "count" : integer positive domain
     * - "integer" : generic integer domain
     * - "min1to1" : -1 to 1 real value domain
     * - "nilto1" : 0 to 1 real value domain
     *
     * \param vmin minimum value
     * \param vmax maximum value
     * \param step step size
     * \return String with the standard name of this domain
     */
    static QString standardNumericDomainName(double vmin, double vmax, double step=rUNDEF) ;

protected:
    SPRange getRange() const;
private:
    long significantDigits(double m1);

    SPNumericRange _range;

};

typedef IlwisData<NumericDomain> INumericDomain;
typedef IlwisData<NumericDomain> ITimeDomain;
}

Q_DECLARE_METATYPE(Ilwis::INumericDomain)

#endif // VALUEDOMAINDATA_H
