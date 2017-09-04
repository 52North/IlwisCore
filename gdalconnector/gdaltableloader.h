#ifndef GDALTABLELOADER_H
#define GDALTABLELOADER_H

namespace Ilwis{
namespace Gdal{

struct FillerColumnDef;

class GdalTableLoader{
    public:
        GdalTableLoader();
        ~GdalTableLoader();
        void loadMetaData(Ilwis::Table *attTable, OGRLayerH hLayer);
        void setColumnCallbacks(Table *attTable, OGRLayerH hLayer);
        void loadRecord(Table *attTable, OGRFeatureH hFeature, std::vector<QVariant>& record); // most cases first column is featureid and thus already set
    private:
        std::vector<FillerColumnDef*> _columnFillers;

        QVariant fillStringColumn(OGRFeatureH featureH, int colIntex);
        QVariant fillDoubleColumn(OGRFeatureH featureH, int colIntex);
        QVariant fillIntegerColumn(OGRFeatureH featureH, int colIntex);
        QVariant fillDateColumn(OGRFeatureH featureH, int colIntex);
        QVariant fillTimeColumn(OGRFeatureH featureH, int colIntex);
        QVariant fillDateTimeColumn(OGRFeatureH featureH, int colIntex);
};

struct FillerColumnDef{
    FillerColumnDef(QVariant(GdalTableLoader::* func)(OGRFeatureH,  int), int i): fillFunc(func), index(i){}
    QVariant (GdalTableLoader::* fillFunc)(OGRFeatureH,  int);
    int index;
};

}
}

#endif // GDALTABLELOADER_H
