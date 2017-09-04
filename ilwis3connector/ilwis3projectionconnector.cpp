#include <QUrl>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QStringList>

#include "kernel.h"
#include "ilwisangle.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "ilwisobjectconnector.h"
#include "catalogexplorer.h"
#include "catalogconnector.h"
#include "inifile.h"
#include "factory.h"
#include "abstractfactory.h"
#include "ilwisobjectfactory.h"
#include "ilwisdata.h"
#include "ilwis3connector.h"
#include "ellipsoid.h"
#include "geodeticdatum.h"
#include "projection.h"
#include "ilwis3projectionconnector.h"

using namespace Ilwis;
using namespace Ilwis3;

ConnectorInterface *ProjectionConnector::create(const Resource &resource, bool load,const IOOptions& options) {
    return new ProjectionConnector(resource, load, options);

}

ProjectionConnector::ProjectionConnector(const Resource &resource, bool load, const IOOptions &options) : Ilwis3Connector(resource, load, options)
{
    QString prj = _odf->value("CoordSystem", "Projection");
    if (prj != sUNDEF)  {
        _internalCode = name2Code(prj,"projection");
        //_type = itPROJECTION;

    }
}

bool ProjectionConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    IOOptions iooptions = options.isEmpty() ? ioOptions() : options;
    if (_odf.isNull())
        return false;
    QString prj = _odf->value("CoordSystem","Projection");
    if ( prj == sUNDEF) {
        kernel()->issues()->log(TR("No projection found in %1").arg(_resource.name()));
        return false;
    }
    QString code = name2Code(prj,"projection");
    InternalDatabaseConnection db;
    QString query = QString("Select * from projection where code = '%1'").arg(code);
    if ( db.exec(query)) {
        if ( db.next()) {
            QSqlRecord rec = db.record();
            Projection *proj = static_cast<Projection *>(data);
            proj->fromInternal(rec);
            proj->setAuthority(rec.field("authority").value().toString());
            proj->setWkt(rec.field("wkt").value().toString());
            proj->setConnector(this, IlwisObject::cmINPUT, iooptions);
            proj->code(code);
            QStringList keys = _odf->childKeys("Projection");
            foreach(QString key,keys) {
                Projection::ProjectionParamValue pv = mapKeyToEnum(key);
                QVariant keyValue = _odf->value("Projection", QString("%1").arg(key));
                proj->setParameter(pv,keyValue);
            }
            return true;

        } else {
            kernel()->issues()->log(TR("Couldn't find projection for code=%1").arg(code));
        }
    } else {
        kernel()->issues()->logSql(db.lastError());
    }
    return false;

}

Projection::ProjectionParamValue ProjectionConnector::mapKeyToEnum(const QString& key) const {

    if ( key == "Zone") {
        return Projection::pvZONE;
    } else if ( key == "Northern Hemisphere")
        return Projection::pvNORIENTED;
    return Projection::pvNONE;
}

IlwisObject *ProjectionConnector::create() const
{
    QString url = QString("ilwis://factory/projections/%1").arg(_internalCode);
    Resource resource = mastercatalog()->name2Resource(url, itPROJECTION);
    if ( !resource.isValid()) {
        kernel()->issues()->log(TR(ERR_COULDNT_CREATE_OBJECT_FOR_2).arg("Projection",url));
    }
    const IlwisObjectFactory *factory =  kernel()->factory<IlwisObjectFactory>("ProjectionFactory",resource);
    if ( factory) {
        return factory->create(resource);
    }

    return 0;
}
