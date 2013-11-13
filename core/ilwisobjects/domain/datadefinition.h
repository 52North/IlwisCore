#ifndef DATADEFINITION_H
#define DATADEFINITION_H

namespace Ilwis {
/*!
 * The datadefinition class is closely related to domain. It defines the data as used by the object (whatever the object may be, e.g. a raster-coverage).
 * The domain concept is a general concept. Meaning that the range is uses it often much broader  than the actual range of data used by the object. A value
 * domain may be valid for  {-1e300 ,0-1e300} while in the coverage the actual range is 0-10. This difference is captured by the datadefinition.
 */
class KERNELSHARED_EXPORT DataDefinition
{
public:
    /*!
     * Copy constructor
     * \param def DataDefinition to be copied onto the new one
     */
    DataDefinition(const DataDefinition &def);

    /*!
     * Constructs a new DataDefinition based on an IDomain and a Range
     *
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
     * Assigns the values of the other DataDefinition to this
     *(copy constructor)
     * \param def1 the other DataDefinition
     * \return a copy of the other DataDefinition
     */
    DataDefinition& operator=(const DataDefinition& def1);

    /*!
     * Changes the range of this DataDefinition to the one specified
     * *invalid range will make this DataDefinition invalid*
     * \param vr The new Range
     */
    void range(Ilwis::Range *vr);

    /*!
     *
     */
    template<typename T=Range> QSharedPointer<T> range() const{
        return _range.staticCast<T>();
    }

    /*!
     * Query for the domain of this DataDefinition
     * \return The IDomain of this DataDefinition
     */
    IDomain domain() const;

    /*!
     * Sets a new domain to this DataDefinition
     * Also sets the range of this domain as the new range of this DataDefinition
     *
     * \param dom The new Domain
     */
    void domain(const IDomain& dom);

    /*!
     * Checks if this Datadefinition is valid
     * A DataDefinition is valid when its Domain is valid
     *
     * \return true when valid
     */
    bool isValid() const;

    /*!
     * Checks if this DataDefinition is compatible with some other DataDefinition
     * They are Compatible when they are both valid and their IDomain 's are compatible
     * \param def the other DataDefinition
     * \return true when compatible
     */
    bool isCompatibleWith(const DataDefinition& def) const;

    /*!
     * Merges 2 DataDefinitions into 1 new DataDefinition
     * if 1 is not valid and the other is, the valid definition will be returned
     * \param def1 The first definition to be merged
     * \param def2 The second definition to be merged
     * \return The merged DataDefinition, one of the 2 starters (if 1 was invalid) OR an empty definition if both were invalid
     */
    static DataDefinition merge(const DataDefinition &def1, const DataDefinition &def2);

protected:
    IDomain _domain;
    SPRange _range;
    SPRange _stretchRange;
};

KERNELSHARED_EXPORT bool operator==(const DataDefinition& def1, const DataDefinition& def2);
KERNELSHARED_EXPORT bool operator!=(const DataDefinition& def1, const DataDefinition& def2);

}

#endif // DATADEFINITION_H
