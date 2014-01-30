#ifndef ILWISCONTEXT_H
#define ILWISCONTEXT_H

#include "Kernel_global.h"
#include <QThreadStorage>
#include <QJsonObject>

namespace Ilwis{

class Catalog;

/*!
 * \brief The IlwisContext class A singleton object that can be reached from everywhere in the system that gives access to a number of properties that describe the context of an Ilwis system
 *
 *The context gives access to properties like working location, systemlocation, memory properties etc.
 */
class KERNELSHARED_EXPORT IlwisContext
{
public:
    friend KERNELSHARED_EXPORT IlwisContext* context();

    IlwisContext();
    ~IlwisContext();

    void loadConfigFile(QFileInfo configFile);
    void addSystemLocation(const QUrl &resource);
    void removeSystemLocation(const QUrl &resource);
    QFileInfo ilwisFolder() const;
    Catalog* workingCatalog() const;
    void setWorkingCatalog(const Ilwis::Catalog &cat);
    QUrl cacheLocation() const;
    quint64 memoryLeft() const;
    quint64 changeMemoryLeft(quint64 amount);
    QJsonObject configuration() const;

private:
    void init();
    static IlwisContext *_context;

    std::vector<QUrl> _systemlocations;
    //QThreadStorage<Catalog *> _workingCatalog;
    Catalog *_workingCatalog;
    quint64 _memoryLimit;
    quint64 _memoryLeft;
    QFileInfo _ilwisDir;
    QJsonObject _configuration;
    QUrl _cacheLocation;
};
KERNELSHARED_EXPORT IlwisContext* context();
}




//KERNELSHARED_EXPORT Ilwis::IlwisContext* context();

#endif // ILWISCONTEXT_H
