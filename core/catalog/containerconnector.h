#ifndef SOURCE_CONTAINER_H
#define SOURCE_CONTAINER_H

namespace Ilwis {
class KERNELSHARED_EXPORT ContainerConnector : public ConnectorInterface
{
public:
    enum FilterOptions{ foFULLPATHS=1, foNAMESONLY=2, foEXTENSIONFILTER=4};
    enum OpenMode { omTEXT, omBINARY};
    ContainerConnector();
    ContainerConnector(const Resource& resource);
    virtual ~ContainerConnector() {}

    virtual bool prepare() = 0;
    virtual std::vector<QUrl> sources(const QStringList& filter, int options=foFULLPATHS) const = 0;
    virtual QFileInfo toLocalFile(const QUrl& datasource) const = 0;
    Resource& source() ;
    Resource source() const;
    virtual bool isValid() const = 0;

private:
    Resource _container;

};
}

typedef std::unique_ptr<Ilwis::ContainerConnector> UPContainerConnector;

#endif // CONTAINER_H
