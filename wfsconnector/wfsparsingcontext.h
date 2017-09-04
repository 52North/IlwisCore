#ifndef WFSPARSINGCONTEXT_H
#define WFSPARSINGCONTEXT_H

#include "wfsconnector_global.h"

namespace Ilwis {
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsParsingContext
{
public:
    WfsParsingContext();

    QString srsName() const;
    int srsDimension() const;
    QString geometryAtttributeName() const;
    QMap<QString,QString> namespaceMappings() const;
    QString featureType()const;
    Resource resource() const;
    QString currentItem() const;

    void setSrsName(QString srsName);
    void setSrsDimension(int srsDimension);
    void setGeometryAtttributeName(QString geometryAtttributeName);
    void addNamespaceMapping(QString prefix, QString uriNamespace);
    void setFeatureType(QString featureType);
    void setResource(Resource resource);
    void setCurrentItem(QString item);

private:
    QString _srsName;
    int _srsDimension;
    QString _currentItem;
    QString _featureType;
    QString _geometryAttributeName;
    QMap<QString,QString> _namespaceMappings;
    Resource _resource;
};
}
}

#endif // WFSPARSINGCONTEXT_H
