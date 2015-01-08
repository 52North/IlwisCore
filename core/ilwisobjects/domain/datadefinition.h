#ifndef DATADEFINITION_H
#define DATADEFINITION_H


namespace Ilwis {

class Domain;
typedef IlwisData<Domain> IDomain;

class NumericRange;
typedef QSharedPointer<NumericRange> SPNumericRange;

class ColorRangeBase;

/*!
 * The datadefinition class is closely related to domain. It defines the data as used by the object (whatever the object may be, e.g. a raster-coverage).
 * The domain concept is a general concept. Meaning that the range is uses it often much broader  than the actual range of data used by the object. A value
 * domain may be valid for  {-1e300 ,0-1e300} while in the coverage the actual range is 0-10. This difference is captured by the datadefinition.
 */
class KERNELSHARED_EXPORT DataDefinition
{
public:
    /*!
     * Copy constructor<br>
     * copies the values of the given DataDefinition to a new one, will also copy 0 and NULL values
     * \param def DataDefinition to be copied onto the new one
     */
    DataDefinition(const DataDefinition &def);

    /*!
     * Constructs a new DataDefinition based on an IDomain and a Range<br>
     * sets the domain of the new DataDefinition as dm, and sets the range to rng, if rng != 0<br>
     * if rng = 0 (defaultvalue) it will set the domain->range() as range (the range will be equal to the domain)<br>
     * if rng != 0, it must be a valid range, which will than be set as the range of this DataDefinition
     *
     * \sa Domain
     * \param dm The Domain of the new DataDefinition
     * \param rng The range of the new DataDefinition
     */
    DataDefinition(const IDomain& dm, Ilwis::Range *rng=0);

    /*!
     * Constructor for an empty DataDefinition
     */
    DataDefinition();

    ~DataDefinition();

    /*!
     * Assigns the values of the other DataDefinition to this<br>
     *(copy constructor)
     * \param def1 the other DataDefinition
     * \return a copy of the other DataDefinition
     */
    DataDefinition& operator=(const DataDefinition& def1);

    /*!
     * Changes the range of this DataDefinition to the one specified<br>
     * the new range must be valid on the domain of this DataDefinition
     *
     * \param vr The new Range
     */
    void range(Ilwis::Range *vr);

    /*!
     *
     */
    template<typename T=Range> QSharedPointer<T> range() const{
          return _range.dynamicCast<T>();
    }



    /*!
     * Query for the domain of this DataDefinition<br>
     * can return invalid domains
     * \return The IDomain of this DataDefinition
     */
    template<typename T=Domain> IlwisData<T> domain() const{
        if (!_domain.isValid())
            return IlwisData<T>();
        return _domain.as<T>();
    }



    /*!
     * Sets a new domain to this DataDefinition<br>
     * Also sets the range of this domain as the new range of this DataDefinition if the domain can be converted to a range<br>
     * will not do anything to the range if the given domain is invalid
     *
     * \param dom The new Domain
     */
    void domain(const IDomain& dom);

    /*!
     * Checks if this Datadefinition is valid<br>
     * A DataDefinition is valid when its Domain is valid
     *
     * \sa Domain
     * \return true when valid
     */
    bool isValid() const;

    /*!
     * Checks if this DataDefinition is compatible with some other DataDefinition<br>
     * They are Compatible when they are both valid and their IDomain's are compatible<br>
     * def cannot be null
     * \sa Domain
     * \param def the other DataDefinition
     * \return true when compatible
     */
    bool isCompatibleWith(const DataDefinition& def) const;

    /*!
     * Merges 2 DataDefinitions into 1 new DataDefinition<br>
     * if 1 is not valid and the other is, the valid definition will be returned<br>
     * def 1 and def 2 cannot be null
     * \param def1 The first definition to be merged
     * \param def2 The second definition to be merged
     * \return The merged DataDefinition, one of the 2 starters (if 1 was invalid) OR an empty definition if both were invalid
     */
    static DataDefinition merge(const DataDefinition &def1, const DataDefinition &def2);

protected:
    IDomain _domain;
    SPRange _range;
};



KERNELSHARED_EXPORT bool operator==(const DataDefinition& def1, const DataDefinition& def2);
KERNELSHARED_EXPORT bool operator!=(const DataDefinition& def1, const DataDefinition& def2);

}

#endif // DATADEFINITION_H
