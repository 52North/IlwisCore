#ifndef FEATURECONNECTOR_H
#define FEATURECONNECTOR_H

namespace geos {
    namespace geom{
    class Polygon;
    }
}
namespace Ilwis {

class FeatureCoverage;

namespace Ilwis3{

class BinaryIlwis3Table;

class FeatureConnector : public CoverageConnector
{
public:
    FeatureConnector(const Ilwis::Resource &resource, bool load,const IOOptions& options=IOOptions());
    bool loadMetaData(Ilwis::IlwisObject *data, const IOOptions &options);
    bool loadData(Ilwis::IlwisObject *obj, const IOOptions& options = IOOptions());
    bool storeMetaData(IlwisObject *obj, const IOOptions &options) ;
    bool storeBinaryData(IlwisObject *obj);

    static ConnectorInterface *create(const Resource &resource, bool load,const IOOptions& options=IOOptions());
    Ilwis::IlwisObject *create() const;
    void calcStatistics(const IlwisObject *obj,NumericStatistics::PropertySets set) const;


    QString format() const;
private:
    bool loadBinaryPoints(FeatureCoverage *fcoverage);
    bool loadBinarySegments(FeatureCoverage *fcoverage);
    bool loadBinaryPolygons(FeatureCoverage *fcoverage);
    bool loadBinaryPolygons30(FeatureCoverage *fcoverage, ITable &tbl);
    bool loadBinaryPolygons37(FeatureCoverage *fcoverage, ITable& tbl);
    //bool readRing(QDataStream &stream,boost::geometry::model::ring<Coordinate2d>& ring);
    geos::geom::CoordinateArraySequence *readRing(QDataStream &stream, double &outerArea);
    bool getRings(Ilwis::FeatureCoverage *fcoverage, qint32 startIndex, const BinaryIlwis3Table &topTable, const BinaryIlwis3Table& polTable, std::vector<geos::geom::LinearRing *> *rings);
    bool isForwardStartDirection(const BinaryIlwis3Table &topTable, qint32 colForward, qint32 colBackward, qint32 colCoords, long index);

    void writeCoords(std::ofstream &output_file, const std::vector<geos::geom::Coordinate>* coords, bool singleton=false);
    bool storeMetaPolygon(FeatureCoverage *fcov, const QString &dataFile);
    bool storeMetaLine(FeatureCoverage *fcov, const QString &dataFile);
    bool storeMetaPoint(FeatureCoverage *fcov, const QString &filepath);
    bool storeBinaryDataPolygon(Ilwis::FeatureCoverage *fcov, const QString &baseName);
    bool storeBinaryDataPoints(FeatureCoverage *fcov, const QString &baseName);
    bool storeBinaryDataLine(FeatureCoverage *fcov, const QString &baseName);
    bool storeMetaData(Ilwis::FeatureCoverage *fcov, IlwisTypes type);
    bool storeBinaryData(Ilwis::FeatureCoverage *fcov, bool isMulti, IlwisTypes type);

    void storeColumn(const QString &colName, const QString &domName, const QString &domInfo, const QString &storeType);

    void writeCoord(std::ofstream& output_file, const geos::geom::Coordinate& crd) {
        double x = crd.x;
        double y = crd.y;
        output_file.write((char *)&x, 8);
        output_file.write((char *)&y, 8);
    }
    void writeLine(const geos::geom::LineString *line, std::ofstream &output_file, long raw);
    void writePolygon(const geos::geom::Polygon *polygon, std::ofstream &output_file, double raw);
    void writePoint(const geos::geom::Point *point, std::ofstream &output_file, long raw);
    void addFeatures(map<quint32, vector<geos::geom::Geometry *> > &geometries, FeatureCoverage *fcoverage, const std::vector<double>& featureValues, IlwisTypes tp);
    bool storeBinaryDataTable(IlwisObject *obj, IlwisTypes tp, const QString &baseName);
    void storeSegment(const UPGeometry &geom, const FeatureCoverage *fcov, std::ofstream &output_file, double &raw);
    void storePolygon(const UPGeometry &geom, const FeatureCoverage *fcov, std::ofstream &output_file, double &raw);
    void storePoint(const UPGeometry &geom, const FeatureCoverage *fcov, std::ofstream &output_file, double &raw);
    void countPlainGeometries(FeatureCoverage *fcov, quint32 &points, quint32 &segments, quint32 &polygons);
};
}
}

#endif // FEATURECONNECTOR_H
