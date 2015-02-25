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
     * The constructor for a RasterCoverage with a a Resource that is registered in the mastercatalog.<br>
     * This resource must have a name and an Id.<br>
     * Code and description are optional.
     *
     * \param resource The Resource
     */
    TextDomain(const Resource& resource);

    //@override
    virtual IlwisTypes valueType() const;

    /*!
     * Transforms the given QVariant into a Qstring.<br>
     * basically the same as QVariant.toString()
     *
     * \param v The variant of which you want the value
     * \return the value of v
     */
    virtual QVariant impliedValue(const QVariant& v) const;

    /*!
     * Checks if this domain contains a certain value.<br>
     * Every value can be translated to a String, so the containment is always cSELF
     *
     * \return Domain::cSELF
     */
    virtual Containement contains(const QVariant& ) const;

    //@override
    IlwisTypes ilwisType() const;

    /*!
     * Sets the given domain as parent of this TextDomain<br>
     * Not really relevant for a TextDomain, so it is not implemented
     */
    void setParent(const IDomain& ){}

    /*!
     * Checks if the given domain is compatible with this ItemDomain<br>
     * Will not be compatible if the given domain is not viable or not an ItemDomain
     *
     * \param dom The domain to be checked
     * \return true when compatible
     */
    bool isCompatibleWith(const Ilwis::IlwisObject *obj, bool strict=false) const;
    bool isOrdered() const;

    //@override
    void range(Range *) {}

private:
    QSharedPointer<Range> getRange() const;
};
typedef IlwisData<TextDomain> ITextDomain;
}

#endif // TEXTDOMAIN_H
