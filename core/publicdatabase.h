#ifndef PUBLICDATABASE_H
#define PUBLICDATABASE_H

#include <QSqlDatabase>

class QSqlRecord;

namespace Ilwis {
/*!
The PublicDatabase class exposes the internal database of Ilwis to the outside world. The default implementation is an in memory sqllite database
 */
class KERNELSHARED_EXPORT PublicDatabase : public QSqlDatabase
{
public:
    /*!
     PublicDatabase initializes the database. Default sqllite is used but by using this constructor one can overrule it. Care has to be taken that the dialect of sql choosen matches.
     * \param db
     */

    PublicDatabase(const QSqlDatabase& db);
    PublicDatabase();
    /*!
     *  prepare initializes the public database
     *
     *this method loads all the system tables in the database. If an error occurs at this level ilwis will abort an log an appropriate message.
     */
    void prepare();

    /*!
     convenience method to translate a code to record of the database

     Codes are unique for a particular set of classes (e.g. ellipsoids). So if you know the code you can figure out which data
     belongs to it. To simplify getting the data for a code this method exist

     \param code unique identifier for an instance of a class
     \param table table in which the code is the key column
     \param rec the record with the data
     \return bool false if any errors did occur. The issue stack will have the exact error
    */
    bool code2Record(const QString& code, const QString& table, QSqlRecord& rec);
    QString findAlias(const QString& name, const QString& type, const QString& nspace);

private:
    void loadPublicTables();
    void insertFile(const QString &filename, QSqlQuery &sqlPublic);
    bool fillEllipsoidRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    bool fillDatumRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    bool fillProjectionRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    bool fillValueDomainRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    bool fillEpsgWithLatLonAxesOrderRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    void insertProj4Epsg(QSqlQuery &sqlPublic);
    bool doQuery(const QString &query, QSqlQuery &sqlPublic);
    bool fillFiltersRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    bool fillRepresentationRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    void insertItemDomains(QSqlQuery &sqlPublic);
    bool fillTeritoryRecord(const QStringList &parts, QSqlQuery &sqlPublic);
    void addRegionallEnvelopes();
};
}

#endif // PUBLICDATABASE_H
