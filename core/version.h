#ifndef VERSION_H
#define VERSION_H

#include "kernel_global.h"
#include "module.h"


namespace Ilwis {

class KERNELSHARED_EXPORT Version {
public:
    Version();
    enum BinaryVersion{ bvFORMAT14, bvFORMAT20, bvFORMAT30, bvFORMATFOREIGN, bvUNKNOWN, bvPOLYGONFORMAT37, bvILWISOBJECTS1};

    bool supportsBinaryVersion(Ilwis::Version::BinaryVersion version, bool throwError = true);
    bool supportsODFVersion(const QString& version, bool throwError=true);
    void addBinaryVersion(Ilwis::Version::BinaryVersion v);
    void addODFVersion(const QString& v);
    bool supportsModuleInterfaceVersion(const QString& inf, const QString& module,bool throwError=true) const;
    void addModuleInterfaceVersion(const QString& inf);
    const static QString interfaceVersion;
    const static QString IlwisShortName;
    const static QString cacheVersion;

    QString buildType() const;
    QString verionNumber() const;
    QString buildNumber() const;
private:
    static QString binaryVersionNumberToString(Ilwis::Version::BinaryVersion v) ;
    QVector<QString> supportedODFVersions;
    QVector<BinaryVersion> binaryVersions;
    QVector<QString> moduleVersions;

};

}

#endif // VERSION_H
