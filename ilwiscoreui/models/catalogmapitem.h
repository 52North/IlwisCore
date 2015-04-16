#ifndef CATALOGMAPITEM_H
#define CATALOGMAPITEM_H

#include "coverage.h"
#include "georeference.h"
#include "ilwiscoreui_global.h"

namespace Ilwis{
class Coverage;
typedef IlwisData<Coverage> ICoverage;

}
class ILWISCOREUISHARED_EXPORT CatalogMapItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name CONSTANT)
public:
    CatalogMapItem();
    CatalogMapItem(const Ilwis::ICoverage& cov, const Ilwis::IGeoReference &grf, QObject* parent);
    ~CatalogMapItem();

    QString latLonEnvelope() const;
    QString catalogItemType();
    QString name() const;

    Q_INVOKABLE QVariantMap drawEnvelope() const;
private:
    Ilwis::BoundingBox _boundingBox;
    Ilwis::ICoverage _coverage;
    Ilwis::IGeoReference _screenGrf;


};

#endif // CATALOGMAPITEM_H
