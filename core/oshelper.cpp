#include "kernel.h"
#include "oshelper.h"

using namespace Ilwis;

OSHelper::OSHelper()
{
}

QString OSHelper::neutralizeFileName(const QString& somelocation, bool allNames) {
    if ( !allNames && somelocation.indexOf("file://") == -1)
        return somelocation;

#ifdef Q_OS_WIN
    return somelocation.toLower();
#else
    return somelocation;
#endif
}

bool OSHelper::isAbsolute(const QString& fragment){
    return fragment.contains("://");
}

bool OSHelper::isFileName(const QString &absolutepath)
{
    // TODO, too simplistic, will be extended
#ifdef Q_OS_WIN
    return absolutepath.indexOf(":") == 1;
#endif
    return absolutepath.indexOf("/") == 0;
}
