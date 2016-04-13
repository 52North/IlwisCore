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

QString OSHelper::operatingSystem() {
    QString operatingSystem;
    #ifdef Q_OS_WIN
    operatingSystem = "windows";
    #endif
    #ifdef Q_OS_LINUX
       operatingSystem = "linux";
    #endif
    return operatingSystem;
}


QString OSHelper::createFileUrlFromParts(const QString& left, const QString& right) {
    QString operatingSystem;
    #ifdef Q_OS_WIN
    return "file:///" + left + right;
    #endif
    #ifdef Q_OS_LINUX
       return "file://" + left + right;
    #endif
}


QString OSHelper::ensureUniqueFilename(QString fileFullPath)
{
    quint16 idx = 1;
    int lastPoint = fileFullPath.lastIndexOf(".");
    QString fileNameNoExt = fileFullPath.left(lastPoint);
    QString fileNameExtension = fileFullPath.right(fileFullPath.length() - lastPoint - 1);

    while (idx < std::numeric_limits<quint16>::max()) {
        QString tempFilename = QString("%1_%2.%3").arg(fileNameNoExt).arg(idx).arg(fileNameExtension);
        QFileInfo fileInfo(tempFilename);

        if (!fileInfo.exists()) {
            return tempFilename;
        }
        ++idx;
    }

    qint64 fallback = QDateTime::currentMSecsSinceEpoch();

    return QString("%1_%2.%3").arg(fileNameNoExt).arg(fallback).arg(fileNameExtension);
}
