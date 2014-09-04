#ifndef ILWISOBJECTCONNECTOR_H
#define ILWISOBJECTCONNECTOR_H

#include <QBuffer>
#include "kernel_global.h"
#include "kernel.h"
#include "connectorinterface.h"
#include "mastercatalog.h"
#include "oshelper.h"

namespace Ilwis {
class CatalogConnector;
typedef std::unique_ptr<Ilwis::CatalogConnector> UPCatalogConnector;

class KERNELSHARED_EXPORT IlwisObjectConnector : public QObject, public ConnectorInterface
{
    Q_OBJECT
public:
    IlwisObjectConnector(const Ilwis::Resource &resource, bool,const IOOptions& options=IOOptions() );
    virtual ~IlwisObjectConnector();

    virtual IlwisObject *create() const = 0;
    IlwisTypes type() const;
    Resource& source() ;
    const Resource &source() const;
    bool dataIsLoaded() const;

protected:
    template<class T> T setObject(const QString& propeprty, const QUrl& defaultName) {

        T object;
        bool ok;
        auto id = _resource[propeprty].toLongLong(&ok);
        if (ok){
            Resource resource = mastercatalog()->id2Resource(id);
            if ( resource.code() != sUNDEF)
                ok = object.prepare(id);
        }
        if (!ok)
            object.prepare(OSHelper::neutralizeFileName(defaultName.toString()));

        return object;
    }

    Resource _resource;
    bool _binaryIsLoaded;
    std::recursive_mutex _mutex;

signals:
    void dataAvailable(QBuffer *buf, bool lastBlock);
public slots:



};
}

#endif // ILWISOBJECTCONNECTOR_H
