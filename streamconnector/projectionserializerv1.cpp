#include "kernel.h"
#include "version.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "projection.h"
#include "factory.h"
#include "abstractfactory.h"
#include "projectionfactory.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "projectionserializerv1.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *ProjectionSerializerV1::create(QDataStream& stream)
{
    return new ProjectionSerializerV1(stream);
}

ProjectionSerializerV1::ProjectionSerializerV1(QDataStream &stream) : VersionedSerializer(stream)
{
}

bool ProjectionSerializerV1::store(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;
    Projection *proj = static_cast<Projection *>(obj);
    QString proj4Def = proj->toProj4();
    _stream << proj4Def;
    _stream << proj->authority();
    return true;

}

bool ProjectionSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::loadMetaData(obj, options))
        return false;
    const ProjectionFactory *factory =  kernel()->factory<ProjectionFactory>("ProjectionFactory","proj4");
    if ( !factory)
        return false;
    Projection *proj = static_cast<Projection *>(obj);
    QString proj4Def;
    _stream >> proj4Def;
    QString projtype = proj->code();
    ProjectionImplementation *impl = factory->create(projtype, proj4Def) ;
    if (!impl)
        return false;
    proj->setImplementation(impl);
    QString authority;
    _stream >> authority;
    proj->setAuthority(authority);
    return true;
}
