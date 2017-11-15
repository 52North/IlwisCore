#ifndef TABLE_H
#define TABLE_H

#include <QSqlDatabase>
#include <unordered_map>

#include "record.h"
#include "kernel_global.h"

namespace Ilwis {

class ColumnDefinition;

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

    virtual ~Table() {}

    /*!
     * return the number of rows in a table.<br>
     * cannot be negative.
     *
     * \return rows in the table
     */
    virtual quint32 recordCount() const=0;

    /*!
     * returns the number of columns in a table.<br>
     * cannot be negative.
     *
     * \return columns in the table
     */
    virtual quint32 columnCount() const=0;

    /*!
     * returns the primary key column name of the table.<br>
     * If not specified, returns sUNDEF.
     *
     * \return columns in the table
     */
    virtual QString primaryKey() const = 0;

    /*!
     * sets the number of rows in a table. If the number of rows is bigger than the current number of rows is the extra records will be filled with
     * appropriate defaults depending on the domain.<br>
     * if the number is smaller the excess records will be deleted. <br>
     * Negative values are not allowed.
     *
     * \param r the new row number
     */
    virtual void recordCount(quint32 r)=0;

    /*!
     * Creates a table based on the definitions in the columndefinition members. Will load the data in the specified amount of collumns and rows
     *
     * \return true if succesfull
     */
    virtual bool createTable() =0;

    /*!
     * adds a column to the current set of columns of this table<br>
     * the name string must be unique and the domain must be valid.<br>
     * if this collumn creates a duplicate nothing will happen.<br>
     * the collumn will be filled with appropriate defaults depending on the domain<br>
     *
     * \sa IDomain
     * \param name the name of a column must be unique among the columns
     * \param domain the domain of the new column
     * \param readonly if the column shall be treated read only (false is default)
     * \return true if the column is succesfully added, false if some error occurred. The nature of the error can be found in the issue logger
     */
    virtual bool addColumn(const QString &name, const IDomain &domain,const bool readonly=false)=0;

    /*!
     * adds a column to the current set of columns of this table. <br>
     * duplicate columns wil be skipped<br>
     * The new collumn contents will be set to appropriate defaults depending on the domain<br>
     * The name string must be unique in the set of collumns and the domainname must be valid
     *
     * \param name the name of a column must be unique among the columns
     * \param domain the domain of the new column
     * \param readonly if the column shall be treated read only (false is default)
     * \return true if the column is succesfully added, false if some error occurred. The nature of the error can be found in the issue logger
     */
    virtual bool addColumn(const QString &name, const QString& domainname,const bool readonly=false)=0;

    /*!
     * adds a column to the column definitions. <br>
     * ColumnDefinition must be valid.<br>
     * The new column contents will be set to appropriate defaults depending on the domain
     *
     * \param def The ColumnDefinition of the new column
     * \return  true if succesful. Duplicate columns are not allowed
     */
    virtual bool addColumn(const ColumnDefinition& def)=0;

    /*!
     * retrieves the definition of the column with the given name.<br>
     * if there is no collumn with that name an invalid ColumnDefinition will be returned.
     *
     * \sa ColumnDefinition
     * \param nme name of the column definition to be retrieved
     * \return a column definition or an invalid defintion if a non existing column was retrieved
     */
    virtual ColumnDefinition columndefinition(const QString& nme) const=0;

    /*!
     * retrieves the definition of the column at the given index. <br>
     * if there is no collumn at this index an invalid ColumnDefinition will be returned.
     *
     * \sa ColumnDefinition
     * \param index index of the column definition to be retrieved
     * \return a column definition or an invalid defintion if a non existing column was retrieved
     */
    virtual ColumnDefinition columndefinition(quint32 index) const=0;

    /*!
     * retrieves a reference to the definition of the column.<br>
     * if there is no collumn at this index nullptr will be returned.
     *
     * \sa ColumnDefinition
     * \param index index of the ColumnDefinition to be retrieved
     * \return a reference to columndefinition or an invalid defintion if a non existing column was retrieved
     */
    virtual ColumnDefinition& columndefinitionRef(quint32 index) = 0;
    virtual ColumnDefinition &columndefinitionRef(const QString &nme) = 0;

     /**
     * sets a new column definition. The new definition must be valid.
     *
     * \sa ColumnDefinition
     * @param coldef the new ColumnDefinition to be set
     */
    virtual void columndefinition(const ColumnDefinition& coldef) = 0;

    /**
     * Adds a new record to this table
     * It will be filled with appropriate default values depending on the domain(s)
     */
    virtual Record& newRecord() = 0;
    virtual void removeRecord(quint32 rec) = 0;

    /*!
     * retrieves a record from a table. A record contains all the fields for one row. This method is implemented in the derivatives<br>
     * of table as how records are organized is quite different between the derivatives
     *
     * \param n the row/record number of the record
     * \return A filled variantlist or an empty list if an error occurred. The nature of the error can be found in the issue logger
     */
    virtual Record& recordRef(quint32 n) = 0;
    virtual const Record& record(quint32 n) const = 0;

    /*!
     * sets a record with values from variantlist. The list doesnt need to contain all the fields in a record but may contain a subset.<br>
     * Note that the fields are in consecutive order. It is up to the programmer that the order of fields in the list match the order of fields in the table
     *
     * \param rec record that has to be updated. If the record number is beyond the last record or the record is iUNDEF, the record will appended to the end
     * \param vars the list values to be set
     * \param offset starting point of the field to be set
     */
    virtual void record(quint32 rec, const std::vector<QVariant>& vars, quint32 offset=0) = 0;

    /*!
     * returns the content of a column with a name in a vector. Will return an empty vector if there is no collumn with that name
     *
     * \param nme name of the column to be returned
     * \param start The index at which it should start returning values(within the collumn)
     * \param stop The index at which it should stop returning values(within the collumn)
     * \return A filled variantlist or an empty list if an error occurred. The nature of the error can be found in the issue logger
     */
    virtual std::vector<QVariant> column(const QString& nme, quint32 start=0, quint32 stop=2e9) const = 0;

    /*!
     * returns the content of a column at a index in a vector. Will return an empty vector if there is no column with that name
     *
     * \param index the index of the column to be returned
     * \param start The index at which it should start returning values(within the collumn)
     * \param stop The index at which it should stop returning values(within the collumn)
     * \return A filled variantlist or an empty list if an error occurred. The nature of the error can be found in the issue logger
     */
    virtual std::vector<QVariant> column(quint32 index, quint32 start=0, quint32 stop=2e9) const = 0;

    /*!
     * sets a column with values from variantlist. The list doesnt need to contain all the rows in a column but may contain a subset.<br>
     * Note that the rows are in consecutive order.
     *
     * \param nme name of the column to be set
     * \param vars values of the rows
     * \param offset starting row form where the values are set. If the number of values to be added goes beyond the size of the table, new records will be added
     */
    virtual void column(const QString& nme, const std::vector<QVariant>& vars, quint32 offset=0) = 0;

    /*!
     * sets a column with values from variantlist. The list doesnt need to contain all the rows in a column but may contain a subset.<br>
     * Note that the rows are in consecutive order.
     *
     * \param index index of the column to be set
     * \param vars values of the rows
     * \param offset starting row form where the values are set. If the number of values to be added goes beyond the size of the table, new records will be added
     */
    virtual void column(const quint32 index, const std::vector<QVariant>& vars, quint32 offset=0) = 0;

    /*!
     * returns the value of a single record/field combination ( a cell).<br>
     * Disabling asRaw (setting it to false) has negative effects on performance.
     *
     * \param col column name
     * \param rec record number
     * \param asRaw when set to false it will return the String value on Thematic and Identifier domains
     * \return the value of the cell or an invalid qvariant if an error occured. The nature of the error can be found in the issue logger
     */
    virtual QVariant cell(const QString& col, quint32 rec, bool asRaw=true) const = 0;

    /*!
     * returns the value of a single record/field combination ( a cell).<br>
     * Disabling asRaw (setting it to false) has negative effects on performance.
     *
     * \param index column index
     * \param asRaw when set to false it will return the String value on Thematic and Identifier domains
     * \param rec record number
     * \return the value of the cell or an invalid qvariant if an error occured. The nature of the error can be found in the issue logger
     */
    virtual QVariant cell(const quint32 index, quint32 rec, bool asRaw=true) const = 0;

    /*!
     * Sets the value of a single cell
     *
     * \param col column name of the cell to be set
     * \param rec record number of the cell to be set
     * \param var the new value of the cell
     */
    virtual void setCell(const QString& col, quint32 rec, const QVariant& var) = 0;

    /*!
     * Sets the value of a single cell
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
     * @return the index of the column with the given name and iundef illegaal
     */
    virtual quint32 columnIndex(const QString& nme) const = 0;

    /**
     * Selects the indices of all collumns that meet the conditions specified in the conditions String<br>
     * so for example: if conditions is @code col1 < 5 AND col2 == "two" @endcode <br>
     * it will return all indices at which collumns 1 and 2 meet the requirements<br>
     *<br>
     * This function is still under development, only the most basic expressions are implemented at this time
     *
     * @param conditions
     * @return
     */
    virtual std::vector<quint32> select(const QString& conditions) const = 0;

    virtual void dataLoaded(bool yesno) = 0;
    virtual bool isDataLoaded() const = 0;
    virtual void initValuesColumn(const QString& colname) = 0;
    virtual IlwisData<Table> copyTable(const QString& name=sUNDEF)  = 0;

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
#define FEATUREVALUECOLUMN "feature_value"
#define FEATURETYPECOLUMN "feature_type"
#define TRACKINDEXCOLUMN "track_index"
#define TRACKVALUECOLUMN "track_value"
#define FEATUREIDDCOLUMN "ilwis_featureid"

#endif // TABLE_H
