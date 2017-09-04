#include <QUrl>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

#include "kernel.h"
#include "geometries.h"
#include "mastercatalog.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "inifile.h"
#include "ilwisdata.h"
#include "ilwis3connector.h"
#include "ellipsoid.h"
#include "ellipsoidconnector.h"

using namespace Ilwis;
using namespace Ilwis3;

ConnectorInterface *EllipsoidConnector::create(const Resource &resource, bool load, const IOOptions &options) {
    return new EllipsoidConnector(resource, load, options);

}

EllipsoidConnector::EllipsoidConnector(const Resource &resource, bool load, const IOOptions &options) : Ilwis3Connector(resource, load, options)
{
}

bool EllipsoidConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{

    QString ell = _odf->value("CoordSystem","Ellipsoid");
    if ( ell == "?") {
        QString datum = _odf->value("CoordSystem","Datum");
        if ( datum != sUNDEF) {
            return createEllipsoidFromDatum(datum, data);
        }
        kernel()->issues()->log(TR("no ellipsoid defined in  %1").arg(data->name()));
        return false;
    }

    QString code = name2Code(ell, "ellipsoid");
    if ( code == sUNDEF){
        kernel()->issues()->log(TR("Couldn't find a code for ellipsoid %1").arg(ell));
        return false;
    }
    return createEllipsoidFromCode(code, data);

}

bool EllipsoidConnector::createEllipsoidFromDatum(const QString &name, IlwisObject *data) {
    QString datum = name;
    QString area = _odf->value("CoordSystem","Datum Area");
    if ( area != sUNDEF )
        datum = datum + "|" + area;
    QString code = name2Code(datum,"datum");
    if ( code == "?"){
        kernel()->issues()->log(TR("No datum code for this alias, ellipsoid %1 can not be found").arg(datum));
        return false;
    }
    InternalDatabaseConnection stmt;
    QString query = QString("Select ellipsoid from datum where code='%1'").arg(code);
    if (stmt.exec(query)) {
        if ( stmt.next()) {
            QString ell = stmt.value(0).toString();
            return createEllipsoidFromCode(ell, data);
        }
    } else {
        kernel()->issues()->logSql(stmt.lastError());
    }
    return false;

}

 bool EllipsoidConnector::createEllipsoidFromCode(const QString& code, IlwisObject *data) {
    QString query = QString("Select * from ellipsoid where code = '%1'").arg(code);
    InternalDatabaseConnection db;
    if (db.exec(query)) {
        if ( db.next()) {
           Ellipsoid *ellipsoid =  static_cast<Ellipsoid *>(data);
           ellipsoid->fromInternal(db.record());
           ellipsoid->setConnector(this, IlwisObject::cmINPUT, ioOptions()); // we are internal, no connector needed
           return true;
        }
    }else
       kernel()->issues()->logSql(db.lastError());
    return false;
}

 IlwisObject *EllipsoidConnector::create() const
 {
     return new Ellipsoid(_resource);
 }
