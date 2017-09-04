#ifndef WFSFEATURE_H
#define WFSFEATURE_H

#include "wfsconnector_global.h"

#include "kernel.h"
#include "geometries.h"

#include "identity.h"
#include "resource.h"

class QUrl;
class QString;

namespace Ilwis {
namespace Wfs {

class WFSCONNECTORSHARED_EXPORT WfsFeature : public Resource
{
public:
    WfsFeature();
    WfsFeature(const QUrl& rawfeatureUrl, const QUrl& normalizedUrl=QUrl());
    ~WfsFeature();

    QUrl getFeatureUrl();

    QString title() const;
    void setTitle(QString title="");

    Envelope bbox() const;
    void setBBox(Envelope &bbox);

private:
    QString _title;
    QString _abstract;
    Envelope _bbox;
};


}
}
#endif // WFSFEATURE_H
