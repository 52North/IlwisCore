#ifndef DATABASETABLE_H
#define DATABASETABLE_H

#include "Kernel_global.h"

namespace Ilwis {
/*!
 Represents a table in the database Ilwis-Objects uses. In the default implementation it uses the internal sqllite database
 that qt has.
 */
class KERNELSHARED_EXPORT DatabaseTable : public BaseTable
{
public:

    DatabaseTable();
    DatabaseTable(const Resource& resource);
    ~DatabaseTable();

    void setDatabase(const QSqlDatabase& base);
    QSqlDatabase database() const;
    /*!
    \se Ilwis::Table
     */
    bool createTable();
    /*!
    \se Ilwis::Table
     */
    bool addColumn(const QString &name, const IDomain &domain);
    bool prepare();
    /*!
    \se Ilwis::Table
     */
    bool isValid() const;
    QString internalName() const;

    template<class T> bool insertColumn(const QString& col, const QVector<T>& values, const IDomain& dom){
        if (!dom.isValid()) {
            kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_IN_4).arg("domain", "table", "column",col));
            return false;
        }
        if ( !_database.isValid()) {
            kernel()->issues()->log(name(),TR("Database is not initialized"));
            return false;
        }

        QString vt = valueType2DataType(dom->valueType());
        QString stmt;

        _columnDefinitionsByName[col] = ColumnDefinition(col,dom,_columns);
        _columnDefinitionsByIndex = _columnDefinitionsByName[col];
        _columns++;

        if ( vt == "Text")
            stmt = "Insert into %1 (%2) values('?')";
        else
            stmt = "Insert into %1 (%2) values(?)";
        return executeStatement(stmt,col,values);

        return true;

    }

    template<class T> bool updateColumn(const QString& col, const QVector<T>& values){
        if ( !_database.isValid()) {
            kernel()->issues()->log(name(),TR("Database is not initialized"));
            return false;
        }
        IDomain dom = _columnDefinitionsByName[col].datadef().domain();
        if (!dom.isValid()) {
            kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_IN_4).arg("domain", "table", "column",col));
            return sUNDEF;
        }
        IlwisTypes ty = dom->valueType();
        QString query = "Update %1 set %2=" ;
        if ( ty < 256)
            query += "?";
        else if ( ty == itSTRING) //TODO other value cases
               query = "'?'";
        return executeStatement(query,col, values);
    }

    /*!
    \se Ilwis::Table
     */
    std::vector<QVariant> record(quint32 n) const ;
    /*!
    \se Ilwis::Table
     */
    void record(quint32 rec, const std::vector<QVariant> &vars, quint32 offset=0);
    /*!
    \se Ilwis::Table
     */
    std::vector<QVariant> column(const QString& nme) const;
    std::vector<QVariant> column(quint32 index) const;
    /*!
    \se Ilwis::Table
     */
    void column(const QString& nme, const std::vector<QVariant>& vars, quint32 offset=0);
    void column(quint32 index, const std::vector<QVariant> &vars, quint32 offset);
    /*!
    \se Ilwis::Table
     */
    QVariant cell(const QString& col, quint32 rec, bool asRaw=true) const;
    QVariant cell(quint32, quint32 rec, bool asRaw=true) const;
    /*!
    \se Ilwis::Table
     */
    void cell(const QString& col, quint32 rec, const QVariant& var);
    void cell(quint32, quint32 rec, const QVariant& var);

    void drop();
    IlwisTypes ilwisType() const;

    std::vector<quint32> select(const QString &conditions) const;
    IlwisObject *copy();
private:
    QSqlDatabase _database;
    bool _sqlCreateDone;

    QString valueType2DataType(IlwisTypes ty) {
        QString vType=sUNDEF;
        if ( (ty >= 1024 &&  ty <= itINT64) || ((ty & itDOMAINITEM) != 0)) {
            vType = "Integer"        ;
        } else if ( ty >= 1024 &&ty <= itDOUBLE) {
            vType = "Real";
        } else if ( ty == itSTRING) {
            vType = "Text";
        } else {
            //TODO other domain types
            //TODO the data types are somewhat dependent on the sql version used by the database
            //TODO and at this moment I allow the tables to be connected to external databases though teh default is internal
            //TODO it may be an idea to let the connector do a type mapping (textual of course) from a generalized types
            //TODO ilwis is probably quite modest in its type use so that should be possible.
        }
        return vType;
    }

    template<class T> bool executeStatement(const QString& stmt, const QString& col, const QVector<T>& values) {
        QSqlQuery db(_database);
        QString query = stmt.arg(internalName(), col);
        db.prepare(query);
        QVariantList varlist;
        for(int i=0 ; i<values.size(); ++i){
            varlist << values[i];
        }
        db.addBindValue(varlist);
        if (!db.execBatch()) {
            kernel()->issues()->logSql(db.lastError());
            return false;
        }

        return true;
    }

    quint32 numberOfExistingRecords();
    bool initLoad();
    void copyTo(IlwisObject *obj);
};
typedef IlwisData<DatabaseTable> IDBTable;
}



#endif // DATABASETABLE_H
