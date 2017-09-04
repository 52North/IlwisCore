#ifndef POSTGRESQLRASTERCONNECTOR_H
#define POSTGRESQLRASTERCONNECTOR_H

namespace Ilwis {

class RasterCoverage;

namespace Postgresql {

class PostgresqlRasterConnector : public PostgresqlConnector
{
public:
    PostgresqlRasterConnector(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());
    ~PostgresqlRasterConnector();

    IlwisObject *create() const;
    static ConnectorInterface *create(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());

    bool loadMetaData(IlwisObject* data,const IOOptions& options=IOOptions());
    bool loadData(IlwisObject *data, const IOOptions &options=IOOptions());
    bool store(IlwisObject* data, const IOOptions &options);

private:
    bool moveIndexes(quint32 &linesPerBlock, quint32 &linesLeft, int &gdalindex);
    IlwisTypes getStoreType(QString pixel_type, IDomain & dom) const;
    static char hex2dec(const char * str);
    static double getNodataVal(QString val, QString & pixel_type);
    std::vector<IlwisTypes> _pixelTypes;
    std::vector<double> _noDataValues;
    std::vector<bool> _hasNodataValue;
    quint32 _nrBands;
    bool _outDb;
    bool _newStyleST_AsBinary;
    quint32 _x_pixels_tile;
    quint32 _y_pixels_tile;
    const int _headerSize;
    const int _bandHeaderSize;
    QString _srid;
};
}
}
#endif // POSTGRESQLRASTERCONNECTOR_H
