#ifndef TEXTDOMAIN_H
#define TEXTDOMAIN_H

namespace Ilwis {
class KERNELSHARED_EXPORT TextDomain : public Domain
{
public:
    /*!
     * Constructs an empty TextDomain
     */
    TextDomain();

    /*!
     * Constructs a TextDomain based on a Resource
     * \param resource The Resource
     */
    TextDomain(const Resource& resource);

    //@override
    virtual IlwisTypes valueType() const;

    /*!
     * Returns the toString of v
     * \param v The variant of which you want the value
     * \return the value of v
     */
    virtual QString value(const QVariant& v) const;

    /*!
     * Checks if this domain contains a certain value
     * \return Domain::cSELF
     */
    virtual Containement contains(const QVariant& ) const;

    //@override
    IlwisTypes ilwisType() const;

    /*!
     * Sets the given domain as parent of this TextDomain
     */
    void setParent(const IDomain& ){}

    /*!
     * Checks if the given domain is compatible with this ItemDomain
     * Will not be compatible if the given domain is not viable or not an ItemDomain
     * \param dom The domain to be checked
     * \return true when compatible
     */
    bool isCompatibleWith(const IDomain &dom) const;

    //@override
    void range(Range *) {}

private:
    QSharedPointer<Range> getRange() const;
};
typedef IlwisData<TextDomain> ITextDomain;
}

#endif // TEXTDOMAIN_H
