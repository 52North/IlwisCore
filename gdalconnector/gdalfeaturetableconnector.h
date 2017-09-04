#ifndef GDALFEATURETABLECONNECTOR_H
#define GDALFEATURETABLECONNECTOR_H

namespace Ilwis{
    namespace Gdal {

        class GdalFeatureTableConnector : public GdalConnector{
            public:
                GdalFeatureTableConnector(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());

                IlwisObject *create() const;
                static ConnectorInterface *create(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());

                bool loadMetaData(IlwisObject* data,const IOOptions&);
                bool storeMetaData(Ilwis::IlwisObject *obj);

                bool loadData(IlwisObject *data, const IOOptions& options = IOOptions());
                bool storeBinaryData(IlwisObject* obj);
        private:

        };

    }
}
#endif // GDALFEATURETABLECONNECTOR_H
