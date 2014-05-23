#ifndef OSHELPER_H
#define OSHELPER_H

namespace Ilwis {
class KERNELSHARED_EXPORT OSHelper
{
public:
    OSHelper();

    static QString neutralizeFileName(const QString& somelocation, bool allNames=false) ;
    static bool isAbsolute(const QString& fragment);
    static bool isFileName(const QString& filename);
    static QString operatingSystem();
};
}

#endif // OSHELPER_H
