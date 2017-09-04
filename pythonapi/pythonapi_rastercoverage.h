#ifndef PYTHONAPI_RASTERCOVERAGE_H
#define PYTHONAPI_RASTERCOVERAGE_H

#include "pythonapi_coverage.h"
#include "pythonapi_geometry.h"
#include "pythonapi_pixeliterator.h"
#include "pythonapi_georeference.h"

namespace Ilwis {
    class RasterCoverage;
    typedef IlwisData<RasterCoverage> IRasterCoverage;
}

namespace pythonapi {

    class DataDefinition;
    class Domain;

    class RasterCoverage : public Coverage{
         friend class PixelIterator;
         friend class Engine;
         friend class Catalog;
    private:
         RasterCoverage(const Ilwis::IRasterCoverage& coverage);
         QVariant resolveIndex(PyObject* pyTrackIndex);
         std::string dateToString(int datepart);
         QString toId(QString val);
    protected:
         virtual const QString getStoreFormat() const;
    public:
        RasterCoverage();
        RasterCoverage(std::string resource);
        ~RasterCoverage();
        RasterCoverage* operator+ (RasterCoverage &rc);
        RasterCoverage* operator+ (double value);
        RasterCoverage* __radd__(double value);
        RasterCoverage* operator-(RasterCoverage &rc);
        RasterCoverage* operator- (double value);
        RasterCoverage* __rsub__(double value);
        RasterCoverage* operator* (RasterCoverage &rc);
        RasterCoverage* operator* (double value);
        RasterCoverage* __rmul__(double value);
        RasterCoverage* __truediv__ (RasterCoverage &rc);
        RasterCoverage* __truediv__ (double value);
        RasterCoverage* __rtruediv__(double value);
        RasterCoverage* operator>(double value);
        RasterCoverage* operator<(double value);
        RasterCoverage* operator>(RasterCoverage &rc);
        RasterCoverage* operator<(RasterCoverage &rc);
        RasterCoverage* operator>=(double value);
        RasterCoverage* operator<=(double value);
        RasterCoverage* operator>=(RasterCoverage &rc);
        RasterCoverage* operator<=(RasterCoverage &rc);
        RasterCoverage* __and__(double value); // &
        RasterCoverage* __or__(double value); // |
        RasterCoverage* __xor__(double value); // ^
        RasterCoverage* __rand__(double value); // &
        RasterCoverage* __ror__(double value); // |
        RasterCoverage* __rxor__(double value); // ^
        RasterCoverage* __and__(RasterCoverage &rc); // &
        RasterCoverage* __or__(RasterCoverage &rc); // |
        RasterCoverage* __xor__(RasterCoverage &rc); // ^
        RasterCoverage* __eq__(double value); // ==
        RasterCoverage* __ne__(double value); // !=
        RasterCoverage* __eq__(RasterCoverage &rc); // ==
        RasterCoverage* __ne__(RasterCoverage &rc); // !=
        PixelIterator __iter__();
        double coord2value(const Coordinate& c);
        double pix2value(const Pixel& pix);
        double pix2value(const PixelD& pix);

        Size size();
        void setSize(const Size& sz);
        void unload();

        CoordinateSystem coordinateSystem();

        GeoReference geoReference();
        void setGeoReference(const GeoReference& gr);

        DataDefinition& datadef() const;
        void setDataDef(DataDefinition* datdef);
        void setDataDef(Domain& dm);

        NumericStatistics* statistics(int mode=0, int bins=0);

        static RasterCoverage* toRasterCoverage(Object *obj);

        PixelIterator begin();
        PixelIterator end();

        PixelIterator band(PyObject* pyTrackIndex);
        void addBand(PyObject* pyTrackIndex, PixelIterator* pyIter);
        void setBandDefinition(PyObject* pyTrackIndex, const DataDefinition& datdef);

        void setStackDefinition(const Domain& dom, PyObject* items);
        quint32 indexOf(const std::string& variantId) const;
        quint32 indexOf(double domainItem) const;
        quint32 indexOf(PyObject* obj) const;
        std::string atIndex(quint32 idx) const;
        PyObject* indexes() const;
        quint32 countStackDomainItems() const;
        Domain stackDomain() const;
        void clear();

        RasterCoverage select(std::string geomWkt);
        RasterCoverage select(Geometry& geom);
        RasterCoverage* reprojectRaster(std::string newName, quint32 epsg, std::string interpol);

        RasterCoverage* clone();

        pythonapi::Envelope envelope();
    };

}
#endif // PYTHONAPI_RASTERCOVERAGE_H
