#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "connectorinterface.h"
#include "basetable.h"
#include "databasetable.h"

using namespace Ilwis;

DatabaseTable::DatabaseTable() : _sqlCreateDone(false)
{
}

DatabaseTable::DatabaseTable(const Resource& resource) : BaseTable(resource), _sqlCreateDone(false)
{
}

DatabaseTable::~DatabaseTable()
{
    drop();
}

void DatabaseTable::drop() {
    if ( internalName() != sUNDEF && _sqlCreateDone) {
        QSqlQuery db( _database);
        if (!db.exec(QString("Drop table %1").arg(internalName()))) {
            kernel()->issues()->logSql(db.lastError());
        }
    }
    _dataloaded = false;
    _sqlCreateDone = false;
}

QString DatabaseTable::internalName() const
{
    if ( name() != sUNDEF)
        return QString("%1_%2").arg(name()).arg(id());
    return sUNDEF;
}

void DatabaseTable::setDatabase(const QSqlDatabase &base)
{
    _database = base;
}

QSqlDatabase DatabaseTable::database() const
{
    return _database;
}

bool DatabaseTable::createTable()
{
    if (_sqlCreateDone)
        return true;

    if (!BaseTable::createTable())
        return false;

    QString stmt = QString("Create table %1 (").arg(internalName());
    stmt += "record_index INTEGER";
    for(int i = 0; i < _columnDefinitionsByName.size(); ++i) {
        QString ty = valueType2DataType(_columnDefinitionsByIndex[i].datadef().domain()->valueType());
        stmt += ",";
        stmt += _columnDefinitionsByIndex[i].name() + " " + ty;
    }
    stmt += ")";
    QSqlQuery db(_database);
    if(!db.exec(stmt)) {
        kernel()->issues()->logSql(db.lastError());
        return false;
    }
    setValid(true);
    _sqlCreateDone = true;

    return true;
}

bool DatabaseTable::addColumn(const QString &name, const IDomain &domain){
    if(!BaseTable::addColumn(name, domain))
        return false;
    if(!_sqlCreateDone)
        createTable();
    else {
        QString type = valueType2DataType(_columnDefinitionsByName[name].datadef().domain()->valueType());
        QSqlQuery db(_database);
        QString query = QString("ALTER TABLE %1 ADD %2 %3 ").arg(internalName()).arg(name).arg(type);
        if ( !db.exec(query)){
            kernel()->issues()->logSql(db.lastError());
            return false;
        }
    }
    return true;
}

std::vector<QVariant> DatabaseTable::record(quint32 n) const {
    if (!const_cast<DatabaseTable *>(this)->initLoad())
        return std::vector<QVariant>();

    QSqlQuery db(_database);
    QString query = QString("Select * from %1 where record_index=%2 ").arg(internalName().arg(n));
    if ( db.exec(query)){
        if ( db.next()) {
            QSqlRecord rec = db.record();
            std::vector<QVariant> values(rec.count());
            for(int i = 0; i < rec.count(); ++i) {
                values[i] = rec.value(i);
            }
            return values;


        } else{
            kernel()->issues()->log(TR("Invalid record number in record query"));
        }
        kernel()->issues()->logSql(db.lastError());
    }
    return std::vector<QVariant>();
}

void DatabaseTable::record(quint32 rec, const std::vector<QVariant> &vars, quint32 offset)
{
    if (!const_cast<DatabaseTable *>(this)->initLoad())
        return ;
     QSqlQuery db(_database);
    if ( rec >= _rows || rec == iUNDEF) {
        QString stmt = QString("INSERT INTO %1 " ).arg(internalName());
        QString columnPart;
        QString valuePart;
        for(int count=0; count < vars.size(); ++count) {
            QString dataType = valueType2DataType(_columnDefinitionsByIndex[offset + count].datadef().domain()->valueType());
            bool needQuotes = dataType == "TEXT";

            if ( count > 0) {
                columnPart += ",";
                valuePart += ",";
            }
            columnPart +=  _columnDefinitionsByIndex[offset + count].name();
            if ( needQuotes )
                valuePart += QString("'%1'").arg(vars[count].toString());
            else
                valuePart += QString("%1").arg(vars[count].toString());
        }
        stmt += "(" + columnPart + ")" + "VALUES(" + valuePart + ")";
        if ( !db.exec(stmt)){
            kernel()->issues()->logSql(db.lastError());
            return ;
        }
    }
    else if ( rec < _rows) {
        QString stmt = QString("UPDATE %1 SET " ).arg(internalName());
        QString rest;
        for(int count=0; count < vars.size(); ++count) {
            int index = offset + count;
            QString dataType = valueType2DataType(_columnDefinitionsByIndex[index].datadef().domain()->valueType());
            bool needQuotes = dataType == "TEXT";
            if ( count > 0)
                rest += ",";
            if ( needQuotes )
                rest += QString(" %1='%2' ").arg( _columnDefinitionsByIndex[index].name()).arg(vars[count].toString());
            else
                rest += QString(" %1=%2 ").arg( _columnDefinitionsByIndex[index].name()).arg(vars[count].toString());
        }
        stmt += rest + QString(" where record_index=%1").arg(rec);
        if ( !db.exec(stmt)){
            kernel()->issues()->logSql(db.lastError());
            return ;
        }
    }
}

QVariant DatabaseTable::cell(quint32 index, quint32 rec) const{
    auto iter = _columnDefinitionsByIndex.find(index);
    if (iter == _columnDefinitionsByIndex.end()) {
        ERROR2(ERR_ILLEGAL_VALUE_2,"Column index", name());
        return QVariant();
    }
    return cell(iter.value().name(), rec);
}
QVariant DatabaseTable::cell(const QString& col, quint32 rec) const{
    if (!const_cast<DatabaseTable *>(this)->initLoad())
        return QVariant();

    quint32 index = columnIndex(col);
    if ( index == iUNDEF)
        return QVariant();
    QSqlQuery db(_database);
    QString stmt = QString("Select * from %1 where record_index=%2 ").arg(col).arg(rec);
    if ( db.exec(stmt)){
        if ( db.next()) {
            QSqlRecord rec = db.record();
            QVariant value;
            for(int i = 0; i < rec.count(); ++i) {
                value = rec.value(i);
            }
            return value;


        } else{
            kernel()->issues()->log(TR("Invalid record number in record query"));
        }
        kernel()->issues()->logSql(db.lastError());
    }
    return QVariant();

}
void DatabaseTable::cell(quint32 index, quint32 rec, const QVariant &var) {
    auto iter = _columnDefinitionsByIndex.find(index);
    if (iter == _columnDefinitionsByIndex.end()) {
        ERROR2(ERR_ILLEGAL_VALUE_2,"Column index", name());
        return;
    }
    cell(iter.value().name(), rec, var);
}

void DatabaseTable::cell(const QString &col, quint32 rec, const QVariant &var)
{
    if (!const_cast<DatabaseTable *>(this)->initLoad())
        return ;
    QString stmt;
    const ColumnDefinition& def = _columnDefinitionsByName[col];
    QString dataType = valueType2DataType(def.datadef().domain()->valueType());
    if ( dataType == sUNDEF) {
        kernel()->issues()->log(TR("Invalid datatype in column definition"));
        return;
    }
    if ( rec >= _rows){
        stmt = QString("INSERT INTO %1 " ).arg(internalName());
        QString columnPart;
        QString valuePart;

        bool needQuotes = dataType == "TEXT";

        columnPart +=  def.name();
        if ( needQuotes )
            valuePart += QString("'%1'").arg(var.toString());
        else
            valuePart += QString("%1").arg(var.toString());
        stmt += "(" + columnPart + ")" + "VALUES(" + valuePart + ")";
     } else if ( rec < _rows) {
        stmt = QString("UPDATE %1 SET " ).arg(internalName());
        QString rest;

        bool needQuotes = dataType == "TEXT";
        if ( needQuotes )
            rest += QString(" %1='%2' ").arg( def.name()).arg(var.toString());
        else
            rest += QString(" %1=%2 ").arg( def.name()).arg(var.toString());
        stmt += rest + QString(" where record_index=%1").arg(rec);
    }
    QSqlQuery db(_database);
    if ( !db.exec(stmt)){
        kernel()->issues()->logSql(db.lastError());
        return ;
    }



}

std::vector<QVariant> DatabaseTable::column(quint32 index) const {
    auto iter = _columnDefinitionsByIndex.find(index);
    if (iter == _columnDefinitionsByIndex.end()) {
        ERROR2(ERR_ILLEGAL_VALUE_2,"Column index", name());
        return std::vector<QVariant>();
    }
    return column(iter.value().name());
}

std::vector<QVariant> DatabaseTable::column(const QString& nme)  const{
    if (!const_cast<DatabaseTable *>(this)->initLoad())
        return std::vector<QVariant>();

    QSqlQuery db(_database);
    QString query = QString("Select %1 from %2").arg(nme,internalName());
    if ( db.exec(query)){
        if ( db.next()) {
            std::vector<QVariant> values;
            do {
                values.push_back(db.value(0)) ;
            } while(db.next());
            return values;


        } else{
            kernel()->issues()->log(TR(ERR_COLUMN_MISSING_2).arg(nme, name()));
        }
        kernel()->issues()->logSql(db.lastError());
    }
    return std::vector<QVariant>();
}

void DatabaseTable::column(quint32 index, const std::vector<QVariant> &vars, quint32 offset)
{
    auto iter = _columnDefinitionsByIndex.find(index);
    if (iter == _columnDefinitionsByIndex.end() ) {
        ERROR2(ERR_ILLEGAL_VALUE_2,"Column index", name());
        return ;
    }
    column(iter.value().name(), vars, offset)    ;
}

void DatabaseTable::column(const QString &nme, const std::vector<QVariant> &vars, quint32 offset)
{
    if (!const_cast<DatabaseTable *>(this)->initLoad())
        return ;

    quint32 index = columnIndex(nme);
    if ( index == iUNDEF)
        return ;
    _rows = numberOfExistingRecords();
    int outside = offset + vars.size() - _rows;
    int inside = offset + vars.size() - outside;
    QSqlQuery db(_database);
    QString dataType = valueType2DataType(_columnDefinitionsByName[nme].datadef().domain()->valueType());
    bool needQuotes = dataType == "TEXT";
    qint32 count = 0;
    if ( inside > 0) {
        QString stmt = QString("UPDATE %1 SET " ).arg(internalName());
        for(count=0; count < vars.size(); ++count) {
            QString rest;
            if ( needQuotes )
                rest += QString(" %1='%2' ").arg( _columnDefinitionsByIndex[index].name()).arg(vars[count].toString());
            else
                rest += QString(" %1=%2 ").arg( _columnDefinitionsByIndex[index].name()).arg(vars[count].toString());
            QString query = stmt + rest + QString(" where record_index=%1").arg(offset + count);
            if ( !db.exec(query)){
                kernel()->issues()->logSql(db.lastError());
                return ;
            }
        }

    }
    if ( outside > 0) {
        QString stmt = QString("INSERT INTO %1 " ).arg(internalName());
        for(count=0; count < vars.size(); ++count) {
            QString rest;
            if ( needQuotes )
                rest += QString("(%1,record_index) VALUES('%2', %3)").arg( _columnDefinitionsByIndex[index].name()).
                        arg(vars[count].toString()).
                        arg(_rows + count);
            else
                rest += QString("(%1,record_index) VALUES(%2, %3)").arg( _columnDefinitionsByIndex[index].name()).
                        arg(vars[count].toString()).
                        arg(_rows + count);

            QString query = stmt + rest;
            if ( !db.exec(query)){
                kernel()->issues()->logSql(db.lastError());
                return ;
            }
        }

    }
    _rows = _rows + std::max(0,outside);

}

bool DatabaseTable::prepare()
{
    if (!IlwisObject::prepare())
        return false;

    if ( _database.isValid()) {
        // if the table already knows about its rows and cols, dont do it again.
        // as this is about metadata it might well be that the table storage doesnt exists yet
        // in the database so querying is pointless. In other scenarios it might exist but it
        // is outside the scope of this method to arrange this.
        if (  Table::isValid())
            return true;

        _rows = numberOfExistingRecords();
        QSqlQuery db(_database);
        QString query = QString("select * from %1 limit 1").arg(internalName());
        if ( db.exec(query)) {
            if ( db.next()) {
               QSqlRecord rec = db.record();
               _columns = rec.count();

            }

        }else {
            kernel()->issues()->logSql(db.lastError());
            return false;
        }
        return true;
    } else{
        kernel()->issues()->log(name(),TR("Information for using the database is invalid or incomplete"));
    }
    return false;
}

bool DatabaseTable::isValid() const
{
    bool dbValid =  _database.isValid();
    return  Table::isValid() && dbValid && internalName()!= sUNDEF ;

}

IlwisTypes DatabaseTable::ilwisType() const
{
    return itDATABASETABLE;
}

quint32 DatabaseTable::numberOfExistingRecords()
{
    QSqlQuery db(_database);
    QString query = QString("Select count(*) from %1").arg(internalName());
    if ( db.exec(query)) {
        if ( db.next()) {
            return db.value(0).toInt();
        }
    } else {
        kernel()->issues()->logSql(db.lastError());
        return 0;
    }
    return 0;
}

bool DatabaseTable::initLoad()
{
    if(!_sqlCreateDone)
        if(!createTable())
            return false;

    return BaseTable::initLoad();
}







