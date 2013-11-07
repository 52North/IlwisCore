#ifndef VALUEDOMAINDATA_H
#define VALUEDOMAINDATA_H

#include "Kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT NumericDomain : public Domain
{
public:
    /*!
     * Creates an empty NumericDomain
     */
    NumericDomain();

    /*!
     * Creates a NumericDomain with supplied Resource
     *
     * \param resource Resource to be used
     */
    NumericDomain(const Resource& resource);

    /*!
     * Changes the Range of this NumericDomain
     *
     * \param vr The new range
     */
    void range(Range *vr);

    /*!
     * for the time being a empty function
     * \return QVariant.toString
     */
    QString value(const QVariant&) const;

    /*!
     * Checks if the value is contained in either this domain or the parentdomain.
     *
     * \sa Domain
     * \param val The value to be checked
     * \return cPARENT when the value is contained in the parent and this domain is not strict, cSELF if the value is contained in this domain and cNONE if the value is contained in neither
     */
    Domain::Containement contains(const QVariant &value) const;

    /*!
     * Checks if this numbericdomain is compatible  with another IDomain,
     * will not be compatible if the other domain is:
     * - invalid
     * - not a NumbericDomain
     *
     * \param dom the domain to be tested
     * \return true when compatible
     */
    bool isCompatibleWith(const IDomain &dom) const;

    /*!
     * Query for the type of the objects in this domain
     * \sa IlwisObject
     * \return The type of the object in this domains range
     */
    IlwisTypes valueType() const;

    /*!
     * Query for the ilwisType of this object (this domain)
     * \sa IlwisObject
     * \return itNUMBERICDOMAIN
     */
    IlwisTypes ilwisType() const;

    /*!
     * Sets a new parent on this domain, the parent has to be valid and so does this domain
     * \param dm the new parentdomain
     */
    void setParent(const IDomain &dm);

    /*!
     * Determines the Standard name for this NumbericDomain with the given parameters
     *
     * \param vmin minimum value
     * \param vmax maximum value
     * \param step step size
     * \return String with the standard name of this domain: "image","image16","count","integer","min1to1" OR "nilto1"
     */
    static QString standardNumericDomainName(double vmin, double vmax, double step=rUNDEF) ;

protected:
    SPRange getRange() const;
private:
    long significantDigits(double m1);

    SPNumericRange _range;

};

typedef IlwisData<NumericDomain> INumericDomain;
}

#endif // VALUEDOMAINDATA_H
