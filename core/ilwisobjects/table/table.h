#ifndef TABLE_H
#define TABLE_H

#include <QSqlDatabase>
#include <unordered_map>

#include "Kernel_global.h"

namespace Ilwis {

static const quint32 NEW_RECORD=1e9;

/*!
 * Generic interface for tabular data in Ilwis. In Ilwis a table is a structure with rows and columns in 2D. There are two main implementations of the table
 * - FlatTable. A in memory 2D grid of values. It is has no connection to a database. It exists in places were performance is needed
 * - DatabaseTable. A 2D grid of values that is based on a database. It is more flexible than a FlatTable but has less performance.
 * FlatTables and DatabaseTables can be converted to each other.
 */

class KERNELSHARED_EXPORT Table : public IlwisObject
{
public:

    ~Table() {}

    /*!
     * return the number of rows in a table
     *
     * \return rows in the table
     */
    virtual quint32 recordCount() const=0;

    /*!
     * returns the number of columns in a table
     *
     * \return columns in the table
     */
    virtual quint32 columnCount() const=0;

    /*!
     * sets the number of rows in a table. If the number of rows is bigger than the current number of rows is the extra records will be filled with null values
     * if the number is smaller the excess records will be deleted
     *
     * \param r the new row number
     */
    virtual void recordCount(quint32 r)=0;

    /*!
     * Creates a table based on the definitions in the columndefinition members. Has no effect on an already vreated table. The latter is no error and will only generate a warning in the issue logger;
     *
     * \return true if succesfull
     */
    virtual bool createTable() =0;

    /*!
     * adds a column to the current set of columns of this table
     *
     * \sa IDomain
     * \param name the name of a column must be unique among the columns
     * \param domain the domain of the new column
     * \return true if the column is succesfully added, false if some error occurred. The nature of the error can be found in the issue logger
     */
    virtual bool addColumn(const QString &name, const IDomain &domain)=0;

    /*!
     * adds a column to the current set of columns of this table
     *
     * \param name the name of a column must be unique among the columns
     * \param domain the domain of the new column
     * \return true if the column is succesfully added, false if some error occurred. The nature of the error can be found in the issue logger
     */
    virtual bool addColumn(const QString &name, const QString& domainname)=0;

    /*!
     * adds a column to the column definitions
     *
     * \param def
     * \return  true if succesful. Duplicate columns are not allowed
     */
    virtual bool addColumn(const ColumnDefinition& def)=0;

    /*!
     * retrieves the definition of the column.
     *
     * \sa ColumnDefinition
     * \param nme name of the column definition to be retrieved
     * \return a column definition or an invalid defintion if a non existing column was retrieved
     */
    virtual ColumnDefinition columndefinition(const QString& nme) const=0;

    /*!
     * retrieves the definition of the column.
     *
     * \sa ColumnDefinition
     * \param index index of the column definition to be retrieved
     * \return a column definition or an invalid defintion if a non existing column was retrieved
     */
    virtual ColumnDefinition columndefinition(quint32 index) const=0;

    /*!
     * retrieves the definition of the column.
     *
     * \sa ColumnDefinition
     * \param index index of the column definition to be retrieved
     * \return a column definition or an invalid defintion if a non existing column was retrieved
     */
    virtual ColumnDefinition& columndefinition(quint32 index) = 0;

    /**
     * sets a new column definition
     *
     * \sa ColumnDefinition
     * @param coldef the new columndefinition to be set
     */
    virtual void columndefinition(const ColumnDefinition& coldef) = 0;
    virtual void newRecord() = 0;

    /*!
     * retrieves a record from a table. A record contains all the fields for one row. This method is implemented in the derivatives
     * of table as how records are organized is quite different between the derivatives
     *
     * \param n the row/record number of the record
     * \return A filled variantlist or an empty list if an error occurred. The nature of the error can be found in the issue logger
     */
    virtual std::vector<QVariant> record(quint32 n) const = 0;

    /*!
     * sets a record with values from variantlist. The list doesnt need to contain all the fields in a record but may contain a subset.
     * Note that the fields are in consecutive order. It is up to the programmer that the order of fields in the list match the order of fields in the table
     *
     * \param rec record that has to be updated. It the record number is beyond the last record or the record is iUNDEF, the record will appended to the end
     * \param vars the list values to be set
     * \param offset starting point of the field to be set
     */
    virtual void record(quint32 rec, const std::vector<QVariant>& vars, quint32 offset=0) = 0;

    /*!
     * returns the content of a column
     *
     * \param nme name of the column to be returned
     * \return A filled variantlist or an empty list if an error occurred. The nature of the error can be found in the issue logger
     */
    virtual std::vector<QVariant> column(const QString& nme, quint32 start=0, quint32 stop=2e9) const = 0;

    /*!
     * returns the content of a column
     *
     * \param index the index of the column to be returned
     * \return A filled variantlist or an empty list if an error occurred. The nature of the error can be found in the issue logger
     */
    virtual std::vector<QVariant> column(quint32 index, quint32 start=0, quint32 stop=2e9) const = 0;

    /*!
     * sets a column with values from variantlist. The list doesnt need to contain all the rows in a column but may contain a subset.
     * Note that the rows are in consecutive order.
     *
     * \param nme name of the column to be set
     * \param vars values of the rows
     * \param offset starting row form where the values are set. If the number of values to be added goes beyond the size of the table, new records will be added
     */
    virtual void column(const QString& nme, const std::vector<QVariant>& vars, quint32 offset=0) = 0;

    /*!
     * sets a column with values from variantlist. The list doesnt need to contain all the rows in a column but may contain a subset.
     * Note that the rows are in consecutive order.
     *
     * \param index index of the column to be set
     * \param vars values of the rows
     * \param offset starting row form where the values are set. If the number of values to be added goes beyond the size of the table, new records will be added
     */
    virtual void column(const quint32 index, const std::vector<QVariant>& vars, quint32 offset=0) = 0;

    /*!
     * returns the value of a single record/field combination ( a cell).
     *
     * \param col column name
     * \param rec record number
     * \return the value of the cell or an invalid qvariant if an error occured. The nature of the error can be found in the issue logger
     */
    virtual QVariant cell(const QString& col, quint32 rec, bool asRaw=true) const = 0;

    /*!
     * returns the value of a single record/field combination ( a cell).
     *
     * \param index column index
     * \param rec record number
     * \return the value of the cell or an invalid qvariant if an error occured. The nature of the error can be found in the issue logger
     */
    virtual QVariant cell(const quint32 index, quint32 rec, bool asRaw=true) const = 0;

    /*!
     * Sets the value of a singel cell
     *
     * \param col column name of the cell to be set
     * \param rec record number of the cell to be set
     * \param var the new value of the cell
     */
    virtual void setCell(const QString& col, quint32 rec, const QVariant& var) = 0;

    /*!
     * Sets the value of a singel cell
     *
     * \param col index of the column of the cell to be set
     * \param rec record number of the cell to be set
     * \param var the new value of the cell
     */
    virtual void setCell(quint32 col, quint32 rec, const QVariant& var) = 0;

    /**
     * Translates a String with a column name into a column index
     *
     * @param nme the name of the column
     * @return the index of the column with the given name
     */
    virtual quint32 columnIndex(const QString& nme) const = 0;

    /**
     * @brief select
     * @param conditions
     * @return
     */
    virtual std::vector<quint32> select(const QString& conditions) const = 0;

protected:
    Table(const Resource& resource) :
    IlwisObject(resource)
    {}

    Table() : IlwisObject()
    {}

};
typedef IlwisData<Table> ITable;
}

Q_DECLARE_METATYPE(Ilwis::ITable)

// special columns
#define COVERAGEKEYCOLUMN "coverage_key"
#define FEATUREIDCOLUMN "feature_id"
#define FEATUREVALUECOLUMN "feature_value"
#define FEATURETYPECOLUMN "feature_type"

#endif // TABLE_H
