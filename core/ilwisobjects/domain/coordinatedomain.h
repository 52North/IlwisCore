#ifndef COORDINATEDOMAIN_H
#define COORDINATEDOMAIN_H

namespace Ilwis {
class CoordinateDomain : public Domain
{
public:
    /*!
     * Constructor for an empty CoordinateDomain
     */
    CoordinateDomain();

    /*!
     * \brief Constructor for a CoordinateDomain based on a Resource
     * \param resource The Resource
     */
    CoordinateDomain(const Resource& resource);

    /*!
     * Changes the range of this domain
     *
     * \param vr the new range
     */
    void setRange(Range *vr);

    /*!
     * Checks if this domain contains a certain coordinate can both be 2D and 3D
     *
     * \param value The coordinate to be checked
     * \return cSELF if the coordinate is contained in this domain or cNONE if it is not
     */
    Domain::Containement contains(const QVariant& value) const;

    /*!
     * Checks if this domain is compatible with another domain.
     * it is not compatible if the other domain is not valid,or when it is not a CoordinateDomain
     * \param dom The other domain
     * \return True when compatible
     */
    bool isCompatibleWith(const IDomain &dom) const;

    /*!
     * Query to the value of this variant within this domain as a String
     * \param v The coordinate in Qvariant form
     * \return The coordinate in String form
     */
    QString value(const QVariant& v) const;

    /*!
     * Query to the type of values in this domain
     * \return itCOORD3D
     */
    IlwisTypes valueType() const;

    /*!
     * Query to the type of this domain
     * \return itCOORDDOMAIN
     */
    IlwisTypes ilwisType() const;

    /*!
     * Query to the ICoordinateSystem of this domain
     * \return the ICoordinateSystem
     */
    ICoordinateSystem coordinateSystem() const;

protected:
    SPRange getRange() const;
private:
    ICoordinateSystem _csy;
    SPRange _envelope;
};
}

typedef Ilwis::IlwisData<Ilwis::CoordinateDomain> ICoordinateDomain;
#endif // COORDINATEDOMAIN_H
