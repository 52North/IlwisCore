#ifndef DATABASETABLE_H
#define DATABASETABLE_H

#include "kernel_global.h"

namespace Ilwis {
/*!
 Represents a table in the database Ilwis-Objects uses. In the default implementation it uses the internal sqllite database
 that qt has.
 */
class KERNELSHARED_EXPORT DatabaseTable : public BaseTable
{
public:

    /**
     * Constructs an empty DatabaseTable and sets the sql create flag to false.
     */
    DatabaseTable();

    /*!
     * The constructor for a DatabaseTable with a a Resource that is registered in the mastercatalog.<br>
     * This resource must have a name and an Id.<br>
     * Code and description are optional. <br>
     * And sets the sql create flag to false.
     *
     * \sa IlwisObject
     * \sa Resource
     * \param source Resource that has to be used
     */
    DatabaseTable(const Resource& resource);

    ~DatabaseTable();

    void setDatabase(const QSqlDatabase& base);
    QSqlDatabase database() const;
    //@override
    bool createTable();

    //@override
    bool addColumn(const QString &name, const IDomain &domain);

    //@override
    bool prepare();

    //@override
    bool isValid() const;

    /**
     * Will try to construct the name of this table using the resource of this DatabaseTable. <br>
     * The String will be of the following form : "<name>_<id>". <br>
     * If it fails sUNDEF will be returned.
     *
     * @return String representation of the name of this DatabaseTable
     */
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
        IDomain dom = columndefinition(col).datadef().domain<>();
        if (!dom.isValid()) {
            kernel()->issues()->log(TR(ERR_INVALID_PROPERTY_IN_4).arg("domain", "table", "column",col));
            return sUNDEF;
        }
        IlwisTypes ty = dom->valueType();
        QString query = "Update %1 set %2=" ;
        if ( ty < 256)
            query += "?";
        else if ( ty == itSTRING) //TODO: other value cases
               query = "'?'";
        return executeStatement(query,col, values);
    }

    //@override
    std::vector<QVariant> record(quint32 n) const ;

    //@override
    void record(quint32 rec, const std::vector<QVariant> &vars, quint32 offset=0);

    //@override
    std::vector<QVariant> column(const QString& nme, quint32 start=0, quint32 stop=2e9) const;

    //@override
    std::vector<QVariant> column(quint32 index, quint32 start=0, quint32 stop=2e9) const;

    //@override
    void column(const QString& nme, const std::vector<QVariant>& vars, quint32 offset=0);

    //@override
    void column(quint32 index, const std::vector<QVariant> &vars, quint32 offset);

    //@override
    QVariant cell(const QString& col, quint32 rec, bool asRaw=true) const;

    //@override
    QVariant cell(quint32, quint32 rec, bool asRaw=true) const;

    //@override
    void setCell(const QString& col, quint32 rec, const QVariant& inputvar);

    //@override
    void setCell(quint32, quint32 rec, const QVariant& var);

    /**
     * Drops the data in this DatabaseTable. <br>
     * Also removes it from the database, and sets dataloaded and sql create to false. <br>
     * If anything fails the error can be found on the issuelogger
     */
    void drop();

    //@override
    IlwisTypes ilwisType() const;

    //@override
    std::vector<quint32> select(const QString &conditions) const;

    //@override
    IlwisObject *clone();

    quint32 newRecord();
    void removeRecord(quint32);
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
            //TODO: other domain types
            //TODO: the data types are somewhat dependent on the sql version used by the database
            //TODO: and at this moment I allow the tables to be connected to external databases though teh default is internal
            //TODO: it may be an idea to let the connector do a type mapping (textual of course) from a generalized types
            //TODO: ilwis is probably quite modest in its type use so that should be possible.
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
