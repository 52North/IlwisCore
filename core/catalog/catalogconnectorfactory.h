#ifndef CATALOGCONNECTORFACTORY_H
#define CATALOGCONNECTORFACTORY_H

#include "Kernel_global.h"

namespace Ilwis {

class CatalogConnector;

class KERNELSHARED_EXPORT CatalogConnectorFactory : public AbstractFactory
{
public:
    CatalogConnectorFactory();
    QList<CatalogConnector *> create(const QUrl &location) const;
    void add(ConnectorCreate cc);

private:
    QList<ConnectorCreate > _creatorsPerObject;


};
}

#endif // CATALOGCONNECTORFACTORY_H
