#ifndef GRIDCOVERAGECONNECTOR_H
#define GRIDCOVERAGECONNECTOR_H

namespace Ilwis {
class BaseGrid;

namespace Ilwis3{

class RasterCoverageConnector : public CoverageConnector
{
public:
    RasterCoverageConnector(const Ilwis::Resource &resource, bool load=true,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject *data, const IOOptions &options);
    bool storeMetaData(Ilwis::IlwisObject *obj, const IOOptions& options = IOOptions());
    bool loadData(Ilwis::IlwisObject *, const IOOptions& options = IOOptions()) ;
    bool storeBinaryData(Ilwis::IlwisObject *obj);
    QString format() const;

    Ilwis::IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource,bool load = true,const IOOptions& options=IOOptions());

    void calcStatistics(const IlwisObject *obj,NumericStatistics::PropertySets set) const;

private:
    qint64 conversion(QFile& file,Ilwis::Grid *grid, int &count);
    //qint64 noconversionneeded(QFile &file, Ilwis::Grid *grid, int &count);
    double value(const char *block, int index) const;
    void setStoreType(const QString &storeType);
    bool loadMapList(IlwisObject *data, const Ilwis::IOOptions &options);
    bool storeMetaDataMapList(Ilwis::IlwisObject *obj);
    QString getGrfName(const IRasterCoverage &raster);
    bool setDataType(IlwisObject *data, const Ilwis::IOOptions &options);
    void loadBlock(UPGrid &grid, QFile &file, quint32 blockIndex, quint32 fileBlock);
    void updateConverter(const IniFile & odf);

    template<typename T> bool save(std::ofstream& output_file,const RawConverter& conv, const IRasterCoverage& raster, const Size<>& sz) const{
        //TODO optimize this
        PixelIterator pixiter(raster, BoundingBox(sz));
        int  i=0;
        T c = 0;
        const char *ptr = reinterpret_cast<const char*>(&c);
        //std::ofstream dum("d:/dummyout.txt",ios_base::out | ios_base::trunc);
        for_each(pixiter, pixiter.end(), [&](double& v){
            c = conv.real2raw(v);
             ++i;
            output_file.write(ptr, sizeof(T));
            //dum << v << " ";
       });
        return true;
    }

    bool save(std::ofstream& output_file, const IRasterCoverage& raster, const Size<>& sz) const{
        //TODO optimize this
        PixelIterator pixiter(raster, BoundingBox(sz));
        int  i=0;
        double c = 0;
        const char *ptr = reinterpret_cast<const char*>(&c);
        //std::ofstream dum("d:/dummyout.txt",ios_base::out | ios_base::trunc);
        for_each(pixiter, pixiter.end(), [&](double& v){
            c = v;
             ++i;
            output_file.write(ptr, sizeof(double));
       });
        return true;
    }

    vector<QUrl> _dataFiles;
    int _storesize;
    IlwisTypes _storetype;
    IlwisTypes _dataType;
    QString createBandName(const Ilwis::IRasterCoverage &raster, quint32 i);
};
}
}

#endif // GRIDCOVERAGECONNECTOR_H
