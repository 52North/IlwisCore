#include <QVector>
#include "version.h"
#include "errorobject.h"
#include "kernel.h"
#include "ilwis.h"


using namespace Ilwis;

const QString Ilwis::Version::interfaceVersion = QString("iv40");
const QString Ilwis::Version::cacheVersion = QString("ilwiscache1");

Version::Version(){
}


bool Version::supportsBinaryVersion(Version::BinaryVersion version, bool throwError) {
    foreach(BinaryVersion ver, binaryVersions)
        if ( ver == version)
            return true;

    if ( throwError) {
        QString vers = binaryVersionNumberToString(version);
        throw ErrorObject(TR("This binary version( %1) of the data file(s) is not supported").arg(vers));
    }
    return false;
}

bool Version::supportsODFVersion(const QString& version, bool throwError){
    foreach(QString ver, supportedODFVersions){
        if ( ver == version)
            return true;
    }

    if ( throwError)
        throw ErrorObject(TR("This Object definition version( %1) of the data file(s) is not supported").arg(version));

    return false;
}
void Version::addBinaryVersion(Version::BinaryVersion v) {
    int index = binaryVersions.indexOf(v);
    if ( index < 0)
        binaryVersions.push_back(v);
}
void Version::addODFVersion(const QString& v) {
    int index = supportedODFVersions.indexOf(v);
    if ( index <  0)
        supportedODFVersions.push_back(v);

}

QString Version::binaryVersionNumberToString(Version::BinaryVersion v) {
    switch (v) {
    case Version::bvFORMAT14:
        return "Format 1.4";
    case Version::bvFORMAT20:
        return "Format 2.0";
    case Version::bvFORMAT30:
        return "Format 3.0";
    case Version::bvFORMATFOREIGN:
        return "Foreign Format";
    case Version::bvPOLYGONFORMAT37:
        return "Polygon Format 3.7";
    case Version::bvILWISOBJECTS1:
        return "ilwisobjects20140811";
    default:
        return "?";
    };
    return "?";
}

bool Version::supportsModuleInterfaceVersion(const QString &version, const QString& module, bool throwError) const{
    int index = moduleVersions.indexOf(version);
    if ( index >= 0)
        return true;

    if ( throwError) {
        QString sVers = "TODO:";
        throw ErrorObject(TR("This module interface version( %1) of the plugin %2 is not supported").arg(sVers).arg(module));
    }
    return false;

}
void Version::addModuleInterfaceVersion(const QString &inf) {
    int index = moduleVersions.indexOf(inf);
    if ( index < 0)
        moduleVersions.push_back(inf);

}

QString Version::buildType() const
{
#ifdef ILWIS_VERSION_TYPE
    return ILWIS_VERSION_TYPE;
#endif
    return sUNDEF;
}

QString Version::verionNumber() const
{
#ifdef ILWIS_VERSION_NUMBER
    return QString::number(ILWIS_VERSION_NUMBER);
#endif
    return sUNDEF;
}

QString Version::buildNumber() const
{
#ifdef ILWIS_BUILDNR
    return QString::number(ILWIS_BUILDNR);
#endif
    return sUNDEF;
}





