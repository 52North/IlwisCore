#include "kernel.h"
#include "version.h"
#include "geometries.h"
#include "ilwisdata.h"
#include "ellipsoid.h"
#include "factory.h"
#include "connectorinterface.h"
#include "versionedserializer.h"
#include "ellipsoidserializerv1.h"

using namespace Ilwis;
using namespace Stream;

VersionedSerializer *EllipsoidSerializerV1::create(QDataStream& stream)
{
    return new EllipsoidSerializerV1(stream);
}

EllipsoidSerializerV1::EllipsoidSerializerV1(QDataStream& stream) : VersionedSerializer(stream)
{
}

bool EllipsoidSerializerV1::store(IlwisObject *obj,const IOOptions& options)
{
    if (!VersionedSerializer::store(obj, options))
        return false;
    Ellipsoid *ell = static_cast<Ellipsoid *>(obj);
    _stream << ell->majorAxis() << ell->flattening()<< ell->authority();

    return true;
}

bool EllipsoidSerializerV1::loadMetaData(IlwisObject *obj, const IOOptions &options)
{
    if (!VersionedSerializer::loadMetaData(obj, options))
        return false;
    double majorAxis, flattening;
    QString authority;
    _stream >> majorAxis >> flattening >> authority;
    Ellipsoid *ellipsoid = static_cast<Ellipsoid *>(obj);
    ellipsoid->setEllipsoid(majorAxis,flattening);
    ellipsoid->setAuthority(authority);


    return true;
}


