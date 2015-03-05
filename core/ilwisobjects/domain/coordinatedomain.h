#ifndef COORDINATEDOMAIN_H
#define COORDINATEDOMAIN_H

#include "domain.h"

namespace Ilwis {
/**
 * The CoordinateDomain class implements a coordinate based type of domain
 * all coordinates are in x y z unless specified otherwise
 */
class CoordinateDomain : public Domain
{
public:
    /*!
     * Constructor for an empty CoordinateDomain
     */
    CoordinateDomain();

    /*!
     * \brief Constructor for a CoordinateDomain based on a Resource
     * requires a valid name and id.<br>
     * calls the parent constructor, which will check for a parent domain and remove this from its children.<br>
     * after that it will check for:<br>
     * if it is of the itCOVERAGE type both the coordinatesystem and the envelope will be set<br>
     * if it is of the itCOORDSYSTEM type only the coordinatesystem will be set
     * \sa Resource
     * \sa Domain
     * \sa IlwisObject
     * \param resource The Resource
     */
    CoordinateDomain(const Resource& resource);

    /*!
     * Changes the range of this domain<br>
     * copies this pointer to the range.
     * \param vr the new range
     */
    void setRange(Range *vr);

    /*!
     * Checks if this domain contains a certain coordinate can both be 2D and 3D<br>
     * 2D -> row col<br>
     * 3D -> row col height
     * \param value The coordinate to be checked
     * \return cSELF if the coordinate is contained in this domain or cNONE if it is not
     */
    Domain::Containement contains(const QVariant& value) const;

    /*!
     * Checks if this domain is compatible with another domain.<br>
     * it is not compatible if the other domain is not valid,or when the other is not a CoordinateDomain<br>
     * compatibility requires both to have the same coodinate system.
     * \param dom The other domain
     * \return True when compatible
     */
    bool isCompatibleWith(const Ilwis::IlwisObject *dom, bool strict=false) const;

    /*!
     * Query to the value of this coordinate variant to a string<br>
     * prints the coordinate in x y z
     * \param v The coordinate in Qvariant form
     * \return The coordinate in String form
     */
    QVariant impliedValue(const QVariant& v) const;

    //@override
    IlwisTypes valueType() const;

    //@override
    IlwisTypes ilwisType() const;

    /*!
     * Query to the ICoordinateSystem of this domain<br>
     * can be null when it is not yet set
     * \return the ICoordinateSystem
     */
    ICoordinateSystem coordinateSystem() const;

    /**
     * empty function only here to fill the interface
     */
    void range(Range *);

protected:
    SPRange getRange() const;
private:
    ICoordinateSystem _csy;
    SPRange _envelope;
};
}

typedef Ilwis::IlwisData<Ilwis::CoordinateDomain> ICoordinateDomain;
#endif // COORDINATEDOMAIN_H
