#ifndef DOMAINDATA_H
#define DOMAINDATA_H

#include "kernel_global.h"

namespace Ilwis {

class Domain;
class Range;

typedef Ilwis::IlwisData<Ilwis::Domain> IDomain;

/*!
 * The domain class defines the semantics of the values used in Ilwis maps or tables. Domains range from numeric, to thematic
 * and time datatypes. Basically every datatype used in Ilwis is tied to a domain. Domains can be hierarchically organized meaning that
 * A domain maybe a generalization of other domains. The purpose of this is to be able to use subsets of domains in general cases. For example
 * the landcover classification system Corine is fairly large and subsetting seems (in relevant cases) reasonable. If the subset would
 * have no relation to the whole thematic domain (Corine) it would be impossible to reuse it in cases were a corine domain is expected.
 * The strict flag limits this behavior. For the behaviour of the individual domain types see the relevant class.
 */
class KERNELSHARED_EXPORT Domain : public IlwisObject
{
public:
    /*!
     * Enumerates the possible containment cases: cSELF,cPARENT,cDECLARED and cNONE<br>
     * - cSELF means containment within the domain asked
     * - cPARENT means containment within the parent of this domain
     * - cDECLARED means //todo
     * - cNONE means no containment at all in relation to this domain
     */
    enum Containement{cSELF=1, cPARENT=2, cDECLARED=3, cNONE=0};

    /*!
     * Creates an empty new strict Domain
     */
    Domain();

    /*!
     * Creates a new strict Domain based on a Resource
     *
     * \param resource The resource to be used for this domain
     */
    Domain(const Resource& resource);

    /*!
     * return the strict status of the domain. When a domain is set to strict it will only compare domain properties<br>
     * to domain types that are identical to itself. Parent domain properties are ignored. For example if a operation only accepts<br>
     * a strict numeric domain min1to1, it will refuse a generic numeric domain as that may contain other (possible illegal) values.
     *
     * \return the strict state.
     */
    bool isStrict() const;

    /*!
     * sets the strict status of the domain. When a domain is set to strict it will only compare domain properties<br>
     * to domain types that are identical to itself. Parent domain properties are ignored. For example if a operation only accepts<br>
     * a strict numeric domain min1to1, it will refuse a generic numeric domain as that may contain other (possible illegal) values.
     *
     * \param the new state
     */
    void setStrict(bool yesno);

    //override
    virtual IlwisTypes valueType() const = 0;

    /*!
     * translates a qvariant into a qstring based on the domain
     */
    virtual QVariant impliedValue(const QVariant& v) const { return v ;}

    /*!
     * returns the parent domain of the domain. This maybe an invalid domain when no parent is set. The child domain specializes (ad/or adds)<br>
     * some properties of the parent domain to create a more limited behavior. When not strict the parent domain can be used to<br>
     * compare properties. For example the default percentage domain has a value range of 0-100 but it is not strict. Other numeric domains maybe freely<br>
     * compared to this one as the parent domain ( the generic numeric domain) will accept all values.
     *
     * \return the parent domain; it may be invalid
     */
    IDomain parent() const;

    /*!
     * sets the parent domain of the domain. This maybe an invalid domain when no parent is set. The child domain specializes (ad/or adds)<br>
     * some properties of the parent domain to create a more limited behavior. When not strict the parent domain can be used to<br>
     * compare properties. For example the default percentage domain has a value range of 0-100 but it is not strict. Other numeric domains maybe freely<br>
     * compared to this one as the parent domain ( the generic numeric domain) will accept all values.
     *
     * \param dm sets the parent domain; it may be set to invalid
     */
    virtual void setParent(const IDomain& dm);

    /*!
     * Checks if this Domain contains a certain value
     *
     * \param value the value to check
     * \return depends on the implementation of the subclass
     */
    virtual Containement contains(const QVariant& value) const = 0;

    /*!
     * Checks if the Domain is compatible with another domain<br>
     * See the relevant subclass for more info
     * \param dom another domain
     * \return true if the Domains are compatible
     */
    virtual bool isCompatibleWith(const IlwisObject* dom, bool strict=false) const = 0;
    virtual bool isOrdered() const = 0;

    template<typename T=Range> QSharedPointer<T> range() const{
        return this->getRange().dynamicCast<T>();
    }

    /**
     * sets a new range on this domain, for requirements on the parameter and ensures on the function see the relevant subclass
     *
     * @param rng the new range
     */
    virtual void range(Range *rng) = 0;

    /*!
     * translates the type of a variant to a compatible ilwis type<br>
     *
     * recognises:
     *  - itUINT8
     *  - itUINT16
     *  - itUINT32
     *  - itINT8
     *  - itINT16
     *  - itINT32
     *  - itINT64
     *  - itBOOL
     *  - itDOUBLE
     *
     * \param the variant to be translated
     * \sa IlwisObject
     * \return the corresponding ilwis type. This maybe unknown if the variant contains a type that is not part of the base system of Ilwis
     */
    static IlwisTypes ilwType(const QVariant &v);

    /*!
     * Translates a String to a compatible ilwis type<br>
     *
     * recognises:
     *  - itUINT8
     *  - itDOUBLE
     *  - itINT32
     *  - itINT64
     *  - itSTRING
     *  - itTIME
     *  - itDATE
     *  - itDATETIME
     *
     * \param value the string to be translated
     * \sa IlwisObject
     * \return the corresponding ilwis type. This maybe unknown if the variant contains a type that is not part of the base system of Ilwis
     */
    static IlwisTypes ilwType(const QString &impliedValue);

protected:
    void addChildDomain(quint64 idchild);
    bool removeChildDomain(quint64 idchild);

    virtual QSharedPointer<Range> getRange() const = 0;
    std::map<quint64, quint32> _childDomains;
private:
    bool _strict;
    IDomain _parentDomain;
};

}

Q_DECLARE_METATYPE(Ilwis::IDomain)

#endif // DOMAINDATA_H
