#ifndef SUPPORTLIBRARYLOADER_H
#define SUPPORTLIBRARYLOADER_H

namespace Ilwis {
class KERNELSHARED_EXPORT SupportLibraryLoader
{
public:
    SupportLibraryLoader(const QFileInfo& configPath );

    void loadLibraries() const;
    QString order2name(quint32 order) const;
    bool isValid() const;
private:
    std::map<quint32, QString> _order;
    QFileInfo _configLocation;
};

}

#endif // SUPPORTLIBRARYLOADER_H
