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
     * Constructs a new ColumnDefinition using a name, DataDefinition an a index<br>
     * The index cannot be negative.
     *
     * \sa DataDefinition
     * @param name the name of the new ColumnDefinition (recommended not to be null/ """);
     * @param def The Datadefinition of the new ColumnDefinition
     * @param colindex the index of the new ColumnDefinition
     */
    ColumnDefinition(const QString& name, const DataDefinition& def, quint64 colindex);

    /**
     * Creates a new ColumnDefinition with just a Domain and no range
     * @param nm
     * @param dom
     * @param colindex
     */
    ColumnDefinition(const QString& nm, const IDomain &dom, quint64 colindex=i64UNDEF);

    /**
     * @brief isValid
     * @return
     */
    bool isValid() const;

    /**
     * @brief type
     * @return
     */
    QString type() const;

    /**
     * @brief datadef
     * @return
     */
    const DataDefinition &datadef() const;

    /**
     * @brief datadef
     * @return
     */
    DataDefinition &datadef();

    /**
     * @brief isMultiple
     * @return
     */
    bool isMultiple() const;

    /**
     * @brief multiple
     * @param yesno
     */
    void multiple(bool yesno);

    /**
     * @brief columnindex
     * @param idx
     */
    void columnindex(quint64 idx);

    /**
     * @brief columnindex
     * @return
     */
    quint64 columnindex() const;

    /**
     * @brief isChanged
     * @return
     */
    bool isChanged() const;

    /**
     * @brief changed
     * @param yesno
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
