#ifndef COLUMN_H
#define COLUMN_H

namespace Ilwis {
/**
 * A ColumnDefinition Defines the values  in the column of a Table. These values have a domain and a range.
 * Additionally the index and the name of the defined column are kept.
 * For ease of used the changed and multiple flags are used.
 * The changed flag will be set whenever something changes within this ColumnDefinition, the multiple flag will be set when
 */
class KERNELSHARED_EXPORT ColumnDefinition : public Identity
{
public:
    /**
     * Constructs an empty ColumnDefinition
     */
    ColumnDefinition();

    /**
     * Copy constructor. Copies the fields of the supplied ColumnDefinition onto a new one.<br>
     * sets the multiple and changed flags to false.<br>
     * Does not copy the index. so a new one must be specified<br>
     * the index cannot be negative
     *
     * @param def the original ColumnDefinition
     * @param index the new index
     */
    ColumnDefinition(const ColumnDefinition& def, quint32 index);

    /**
     * Constructs a new ColumnDefinition using a name, DataDefinition an index.<br>
     * The index cannot be negative.
     *
     * \sa DataDefinition
     * @param name the name of the new ColumnDefinition (recommended not to be null/ """);
     * @param def The Datadefinition of the new ColumnDefinition
     * @param colindex the index of the new ColumnDefinition
     */
    ColumnDefinition(const QString& name, const DataDefinition& def, quint64 colindex);

    /**
     * Creates a new ColumnDefinition using a name, a Domain and an index. <br>
     * Because only a Domain is used the range will not be set. <br>
     * The index cannot be negative.
     * @param nm
     * @param dom
     * @param colindex
     */
    ColumnDefinition(const QString& nm, const IDomain &dom, quint64 colindex=i64UNDEF);

    /**
     * Checks if this ColumnDefinition is valid.<br>
     * To be valid a ColumnDefinition has to have a name that is not iUNDEF and a valid domain.
     *
     * \sa Domain
     * @return
     */
    bool isValid() const;

    /**
     * Query for the type of this object. <br>
     * Will return "Column".
     * @return "Column"
     */
    QString type() const;

    /**
     * Query for the DataDefinition of this ColumnDefinition.
     *
     * @return The DataDefinition
     */
    const DataDefinition &datadef() const;

    /**
     * Query for the DataDefinition of this ColumnDefinition.
     *
     * @return The DataDefinition
     */
    DataDefinition &datadef();

    /**
     * @brief isMultiple
     * @return
     */
    bool isMultiple() const;

    /**
     * Sets the multiple flag
     * @param yesno the new value of the multiple flag
     */
    void multiple(bool yesno);

    /**
     * Sets the index of the ColumnDefinition to the given value. <br>
     * The new index should not be negative.
     *
     * @param idx The new index
     */
    void columnindex(quint64 idx);

    /**
     * Query for the index of this ColumnDefinition.
     *
     * @return The Columnindex of this ColumnDefinition
     */
    quint64 columnindex() const;

    /**
     * Query for the changed flag.
     *
     * @return true if this ColumnDefinition has been changed
     */
    bool isChanged() const;

    /**
     * Sets the changed flag
     *
     * @param yesno the new value of the changed flag.
     */
    void changed(bool yesno);
private:
    DataDefinition _datadef;
    bool _multiple;
    bool _changed;

};

KERNELSHARED_EXPORT bool operator==(const ColumnDefinition& def1, const ColumnDefinition& def2);
KERNELSHARED_EXPORT bool operator!=(const ColumnDefinition& def1, const ColumnDefinition& def2);
}

#endif // COLUMN_H
