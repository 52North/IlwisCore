#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include "kernel.h"
#include "ilwisdata.h"
#include "geometries.h"
#include "coordinatesystem.h"
#include "connectorinterface.h"
#include "ilwisobjectconnector.h"
#include "domain.h"
#include "numericdomain.h"
#include "range.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "basetable.h"
#include "coverage.h"
#include "rastercoverage.h"
#include "postgresqlconnector.h"
#include "postgresqldatabaseutil.h"
#include "postgresqlrasterconnector.h"

using namespace Ilwis;
using namespace Postgresql;

PostgresqlRasterConnector::PostgresqlRasterConnector(const Ilwis::Resource &resource, bool load, const IOOptions &options)
: PostgresqlConnector(resource, load,options)
, _nrBands(0)
, _outDb(false)
, _newStyleST_AsBinary(false)
, _x_pixels_tile(0)
, _y_pixels_tile(0)
, _headerSize(61)
, _bandHeaderSize(1)
{
}

PostgresqlRasterConnector::~PostgresqlRasterConnector()
{
}

IlwisObject *PostgresqlRasterConnector::create() const
{
   return new RasterCoverage(_resource);
}

ConnectorInterface *PostgresqlRasterConnector::create(const Ilwis::Resource &resource, bool load, const IOOptions &options)
{
    return new PostgresqlRasterConnector(resource, load, options);
}

bool PostgresqlRasterConnector::loadMetaData(IlwisObject *data, const IOOptions &options)
{
    RasterCoverage *coverage = static_cast<RasterCoverage *>(data);
    PostgresqlParameters params (_resource.url(true).toString());
    PostgresqlDatabaseUtil pgUtil(params);
    QList<MetaRasterColumn> metaGeometries;
    pgUtil.getMetaForRasterColumns(metaGeometries);
    _newStyleST_AsBinary = false;
    QString sqlBuilder = "SELECT pg_type.typname FROM pg_proc, pg_type WHERE proname='st_asbinary' and pg_proc.proargtypes[1]=pg_type.oid and pg_type.typname='bool' limit 1";
    QSqlQuery query = pgUtil.doQuery(sqlBuilder);
    if (query.next())
        _newStyleST_AsBinary = true;
    ICoordinateSystem crs;
    double x_pixel_size = 0;
    double y_pixel_size = 0;
    QString bandPixelType = "";
    sqlBuilder = "SELECT ST_SRID(" + params.column() + "), ST_ScaleX(" + params.column() + "), ST_ScaleY(" + params.column() + "), ST_Width(" + params.column() + "), ST_Height(" + params.column() + "), ST_NumBands(" + params.column() + ") FROM " + params.schema() + "." + params.table() + (params.rasterID().isEmpty() ? (" WHERE " + params.column() + " IS NOT NULL LIMIT 1") : (" WHERE rid=" + params.rasterID()));
    query = pgUtil.doQuery(sqlBuilder);
    if (query.next()) {
        _srid = query.value(0).toString();
        ICoordinateSystem crs;
        QString code = "code=epsg:" + _srid;
        if ( !crs.prepare(code, itCONVENTIONALCOORDSYSTEM)) {
            ERROR1("Could not prepare crs with %1.", code);
        }
        x_pixel_size = query.value(1).toDouble();
        y_pixel_size = query.value(2).toDouble();
        _x_pixels_tile = query.value(3).toUInt();
        _y_pixels_tile = query.value(4).toUInt();
        _nrBands = query.value(5).toUInt();
    }

    Envelope bbox;
    sqlBuilder = "SELECT ST_Extent(" + params.column() + "::geometry) FROM " + params.schema() + "." + params.table() + (!params.rasterID().isEmpty() ? (" WHERE rid=" + params.rasterID()) : "");
    query = pgUtil.doQuery(sqlBuilder);
    if (query.next()) {
        QString extent = query.value(0).toString();
        if (!extent.isEmpty())
            bbox = Envelope (extent);
    }

    if (bbox.isValid() && x_pixel_size != 0 && y_pixel_size != 0) {
        quint64 width = abs(round((bbox.max_corner().x - bbox.min_corner().x) / x_pixel_size));
        quint64 height = abs(round((bbox.max_corner().y - bbox.min_corner().y) / y_pixel_size));
        Size<> size(width, height, _nrBands);
        QString grfs;
        if (crs.isValid())
            grfs = QString("code=georef:type=corners,csy=%1,envelope=%2,gridsize=%3").arg(crs->id()).arg(bbox.toString()).arg(size.toString());
        else
            grfs = QString("code=georef:type=corners,csy=unknown,envelope=%1,gridsize=%2").arg(bbox.toString()).arg(size.toString());
        IGeoReference georeference(grfs);
        coverage->georeference(georeference);
        coverage->gridRef()->prepare(0,size);
        if (crs.isValid())
            coverage->coordinateSystem(crs);
        coverage->envelope(bbox);
        std::vector<double> v;
        for (quint32 band = 0; band < _nrBands; ++band)
            v.push_back(band);
        coverage->stackDefinitionRef().setSubDefinition(IDomain("count"),v);
    }
    _pixelTypes.resize(_nrBands);
    _noDataValues.resize(_nrBands);
    _hasNodataValue.resize(_nrBands);
    for (quint32 band = 0; band < _nrBands; ++band) {
        sqlBuilder = "SELECT ST_BandPixelType(" + params.column() + "," + (band+1) + "), ST_BandNoDataValue(" + params.column() + "," + (band+1) + ") FROM " + params.schema() + "." + params.table() + (params.rasterID().isEmpty() ? (" WHERE " + params.column() + " IS NOT NULL LIMIT 1") : (" WHERE rid=" + params.rasterID()));
        qDebug()<<sqlBuilder;
        query = pgUtil.doQuery(sqlBuilder);
        if (query.next()) {
            QString bandPixelType = query.value(0).toString();
            IDomain dom;
            _pixelTypes[band] = getStoreType(bandPixelType, dom);
            if (!query.value(1).isNull()) {
                double bandNodataValue = getNodataVal(query.value(1).toString(), bandPixelType);
                _noDataValues[band] = bandNodataValue;
                _hasNodataValue[band] = true;
            } else {
                _noDataValues[band] = 0;
                _hasNodataValue[band] = false;
            }
            if(!dom.isValid()) {
                ERROR1(ERR_FIND_SYSTEM_OBJECT_1, "domain for postgres");
                ERROR1(ERR_NO_INITIALIZED_1, source().name());
                return false;
            }
            coverage->datadefRef(band).domain(dom);
            bool fHaveStats = false;
            double rMin, rMax;
            if (params.rasterID().isEmpty()) {
                sqlBuilder = "SELECT ST_SummaryStats('" + params.table() + "', '" + params.column() + "'," + (band+1) +")"; // To be tested!! Note that posrgres 2.2 has a different function name
                query = pgUtil.doQuery(sqlBuilder);
                if(query.next()) {
                    QString stats = query.value(0).toString();
                    stats = stats.mid(1, stats.size() - 2); // remove ( and )
                    QStringList statlist = stats.split(",");
                    rMin = statlist[4].toDouble();
                    rMax = statlist[5].toDouble();
                    fHaveStats = true;
                }
            }
            if (fHaveStats)
                coverage->datadefRef(band).range(new NumericRange(rMin, rMax, dom->range<NumericRange>()->resolution()));
            else
                coverage->datadefRef(band).range(new NumericRange()); // provide an invalid NumericRange here, in order to force computing raster statistics when needed
        }
    }

    // coverage->setBandDefinition(0, coverage->datadef()); // obsolete?
    return true;
}

bool PostgresqlRasterConnector::loadData(IlwisObject *data, const IOOptions &options)
{
    quint32 layer = source().hasProperty("bandindex") ? source()["bandindex"].toUInt(): iUNDEF;
    RasterCoverage *coverage = static_cast<RasterCoverage *>(data);
    PostgresqlParameters params (_resource.url(true).toString());
    PostgresqlDatabaseUtil pgUtil(params);
    QList<MetaRasterColumn> metaRasters;
    pgUtil.getMetaForRasterColumns(metaRasters);

    UPGrid& grid = coverage->gridRef();
    //blocklimits: key = band number, value= blocks needed from this band
    quint32 linesPerBlock = grid->maxLines();
    quint32 totalLines =grid->size().ysize();
    std::map<quint32, std::vector<quint32> > blocklimits;
    if ( layer == iUNDEF)
        blocklimits = grid->calcBlockLimits(options);
    else{
        for(int i = 0; i < totalLines / linesPerBlock; ++i)
            blocklimits[layer].push_back(i);
    }
    std::vector<double> values;;
    for(const auto& layer : blocklimits) { // loop over layers
        quint32 linesLeft = totalLines - grid->maxLines() * layer.second[0];
        quint32 layerNr = layer.first;
        int inLayerBlockIndex = layer.second[0] % grid->blocksPerBand(); // blocknumber within a layer
        for(const auto& blockIndex : layer.second) { // loop over the blocknumbers (over all layers)
            quint32 noItems = grid->blockSize(blockIndex);
            if ( noItems == iUNDEF)
                continue;

            values.resize(noItems);

            quint32 gridblock_x_start = 0;
            quint32 gridblock_y_start = inLayerBlockIndex * linesPerBlock;
            quint32 gridblock_x_size = grid->size().xsize();
            quint32 gridblock_y_size = std::min(linesPerBlock, linesLeft);

            QString sqlBuilder;
            if (!params.rasterID().isEmpty()) { // each record is an independent raster
                sqlBuilder = "SELECT ST_AsBinary(" + (_outDb ? (_newStyleST_AsBinary ? (params.column() + ",TRUE") : ("ST_MapAlgebraExpr(" + params.column() + ",1,NULL,'[rast]')")) : params.column()) + ") FROM " + params.schema() + "." + params.table() + " WHERE rid=" + params.rasterID();
                qDebug()<<sqlBuilder;
            } else { // tiled-raster: each record is a tile of a raster; the entire table represents the raster
                const IGeoReference & gr = coverage->georeference();
                Coordinate crd1 = gr->pixel2Coord(Pixel(gridblock_x_start + 0.5, gridblock_y_start + 0.5));
                Coordinate crd2 = gr->pixel2Coord(Pixel(gridblock_x_start + gridblock_x_size - 1.5, gridblock_y_start + gridblock_y_size - 1.5));
                sqlBuilder = "SELECT ST_AsBinary(" + (_outDb ? (_newStyleST_AsBinary ? (params.column() + ",TRUE") : ("ST_MapAlgebraExpr(" + params.column() + ",1,NULL,'[rast]')")) : params.column()) + ") FROM " + params.schema() + "." + params.table() + " WHERE ST_Intersects(" + params.column() + ", ST_GeomFromEWKT('SRID=" + _srid + ";LINESTRING(" + crd1.x + " " + crd1.y + "," + crd2.x + " " + crd2.y +")')) ORDER BY ST_UpperLeftY(" + params.column() + "), ST_UpperLeftX(" + params.column() + ")";
                qDebug()<<sqlBuilder;
            }

            QSqlQuery query = pgUtil.doQuery(sqlBuilder);

            // Extend gridblock to multiples of postgres-tiles
            quint32 iLeft = gridblock_x_start - gridblock_x_start % _x_pixels_tile;
            quint32 iRight = gridblock_x_start + gridblock_x_size + (gridblock_x_start + gridblock_x_size) % _x_pixels_tile;
            quint32 iTop = gridblock_y_start - gridblock_y_start % _y_pixels_tile;
            quint32 iBottom = gridblock_y_start + gridblock_y_size + (gridblock_y_start + gridblock_y_size) % _y_pixels_tile;
            quint32 tilerow = 0;
            quint32 tilecol = 0;

            while (query.next()) {
                quint32 jMinX = ((tilecol == 0) && (gridblock_x_start > iLeft)) ? ((gridblock_x_start - iLeft) % _x_pixels_tile) : 0;
                quint32 jMaxX = ((tilecol == gridblock_x_size / _x_pixels_tile) && (gridblock_x_start + gridblock_x_size - 1 < iRight)) ? (_x_pixels_tile - (iRight - gridblock_x_size - gridblock_x_start + 1) % _x_pixels_tile) : _x_pixels_tile;
                quint32 jMinY = ((tilerow == 0) && (gridblock_y_start > iTop)) ? ((gridblock_y_start - iTop) % _y_pixels_tile) : 0;
                quint32 jMaxY = ((tilerow == gridblock_y_size / _y_pixels_tile) && (gridblock_y_start + gridblock_y_size - 1 < iBottom)) ? (_y_pixels_tile - (iBottom - gridblock_y_size - gridblock_y_start + 1) % _y_pixels_tile) : _y_pixels_tile;

                QByteArray bytes = query.value(0).toByteArray();
                const char * tileHex = bytes.constData();
                tileHex += 2 + 2 * (_headerSize + _bandHeaderSize);
                switch(_pixelTypes[layerNr]) {
                case itBOOL:
                    break;
                case itINT8:
                    tileHex += 2;
                    for (quint32 jy = jMinY; jy < jMaxY; ++jy) {
                        for (quint32 jx = jMinX; jx < jMaxX; ++jx) {
                            char b = hex2dec(&tileHex[2 * jy * _x_pixels_tile + 2 * jx]);
                            values[tilerow * _y_pixels_tile * gridblock_x_size + tilecol * _x_pixels_tile + jy * gridblock_x_size + jx] = (_hasNodataValue[layerNr] && (_noDataValues[layerNr] == b)) || std::isnan(b) || std::isinf(b) ? rUNDEF : b;
                        }
                    }
                    break;
                case itUINT8:
                    tileHex += 2;
                    for (quint32 jy = jMinY; jy < jMaxY; ++jy) {
                        for (quint32 jx = jMinX; jx < jMaxX; ++jx) {
                            unsigned char b = hex2dec(&tileHex[2 * jy * _x_pixels_tile + 2 * jx]);
                            values[tilerow * _y_pixels_tile * gridblock_x_size + tilecol * _x_pixels_tile + jy * gridblock_x_size + jx] = (_hasNodataValue[layerNr] && (_noDataValues[layerNr] == b)) || std::isnan(b) || std::isinf(b) ? rUNDEF : b;
                        }
                    }
                    break;
                case itINT16:
                    tileHex += 2 * sizeof(qint16);
                    {
                        qint16 hl;
                        char * c = (char*)(&hl);
                        for (quint32 jy = jMinY; jy < jMaxY; ++jy) {
                            for (quint32 jx = jMinX; jx < jMaxX; ++jx) {
                                c[0] = hex2dec(&tileHex[4 * jy * _x_pixels_tile + 4 * jx]);
                                c[1] = hex2dec(&tileHex[4 * jy * _x_pixels_tile + 4 * jx + 2]);
                                values[tilerow * _y_pixels_tile * gridblock_x_size + tilecol * _x_pixels_tile + jy * gridblock_x_size + jx] = (_hasNodataValue[layerNr] && (_noDataValues[layerNr] == hl)) || std::isnan(hl) || std::isinf(hl) ? rUNDEF : hl;
                            }
                        }
                    }
                    break;
                case itUINT16:
                    tileHex += 2 * sizeof(quint16);
                    {
                        quint16 hl;
                        char * c = (char*)(&hl);
                        for (quint32 jy = jMinY; jy < jMaxY; ++jy) {
                            for (quint32 jx = jMinX; jx < jMaxX; ++jx) {
                                c[0] = hex2dec(&tileHex[4 * jy * _x_pixels_tile + 4 * jx]);
                                c[1] = hex2dec(&tileHex[4 * jy * _x_pixels_tile + 4 * jx + 2]);
                                values[tilerow * _y_pixels_tile * gridblock_x_size + tilecol * _x_pixels_tile + jy * gridblock_x_size + jx] = (_hasNodataValue[layerNr] && (_noDataValues[layerNr] == hl)) || std::isnan(hl) || std::isinf(hl) ? rUNDEF : hl;
                            }
                        }
                    }
                    break;
                case itINT32:
                    tileHex += 2 * sizeof(qint32);
                    {
                        qint32 hl;
                        char * c = (char*)(&hl);
                        for (quint32 jy = jMinY; jy < jMaxY; ++jy) {
                            for (quint32 jx = jMinX; jx < jMaxX; ++jx) {
                                c[0] = hex2dec(&tileHex[8 * jy * _x_pixels_tile + 8 * jx]);
                                c[1] = hex2dec(&tileHex[8 * jy * _x_pixels_tile + 8 * jx + 2]);
                                c[2] = hex2dec(&tileHex[8 * jy * _x_pixels_tile + 8 * jx + 4]);
                                c[3] = hex2dec(&tileHex[8 * jy * _x_pixels_tile + 8 * jx + 6]);
                                values[tilerow * _y_pixels_tile * gridblock_x_size + tilecol * _x_pixels_tile + jy * gridblock_x_size + jx] = (_hasNodataValue[layerNr] && (_noDataValues[layerNr] == hl)) || std::isnan(hl) || std::isinf(hl) ? rUNDEF : hl;
                            }
                        }
                    }
                    break;
                case itUINT32:
                    tileHex += 2 * sizeof(quint32);
                    {
                        quint32 hl;
                        char * c = (char*)(&hl);
                        for (quint32 jy = jMinY; jy < jMaxY; ++jy) {
                            for (quint32 jx = jMinX; jx < jMaxX; ++jx) {
                                c[0] = hex2dec(&tileHex[8 * jy * _x_pixels_tile + 8 * jx]);
                                c[1] = hex2dec(&tileHex[8 * jy * _x_pixels_tile + 8 * jx + 2]);
                                c[2] = hex2dec(&tileHex[8 * jy * _x_pixels_tile + 8 * jx + 4]);
                                c[3] = hex2dec(&tileHex[8 * jy * _x_pixels_tile + 8 * jx + 6]);
                                values[tilerow * _y_pixels_tile * gridblock_x_size + tilecol * _x_pixels_tile + jy * gridblock_x_size + jx] = (_hasNodataValue[layerNr] && (_noDataValues[layerNr] == hl)) || std::isnan(hl) || std::isinf(hl) ? rUNDEF : hl;
                            }
                        }
                    }
                    break;
                case itFLOAT:
                    tileHex += 2 * sizeof(float);
                    {
                        float f;
                        char * c = (char*)(&f);
                        for (quint32 jy = jMinY; jy < jMaxY; ++jy) {
                            for (quint32 jx = jMinX; jx < jMaxX; ++jx) {
                                c[0] = hex2dec(&tileHex[8 * jy * _x_pixels_tile + 8 * jx]);
                                c[1] = hex2dec(&tileHex[8 * jy * _x_pixels_tile + 8 * jx + 2]);
                                c[2] = hex2dec(&tileHex[8 * jy * _x_pixels_tile + 8 * jx + 4]);
                                c[3] = hex2dec(&tileHex[8 * jy * _x_pixels_tile + 8 * jx + 6]);
                                values[tilerow * _y_pixels_tile * gridblock_x_size + tilecol * _x_pixels_tile + jy * gridblock_x_size + jx] = (_hasNodataValue[layerNr] && (_noDataValues[layerNr] == f)) || std::isnan(f) || std::isinf(f) ? rUNDEF : f;
                            }
                        }
                    }
                    break;
                case itDOUBLE:
                    tileHex += 2 * sizeof(double);
                    {
                        double d;
                        char * c = (char*)(&d);
                        for (quint32 jy = jMinY; jy < jMaxY; ++jy) {
                            for (quint32 jx = jMinX; jx < jMaxX; ++jx) {
                                c[0] = hex2dec(&tileHex[16 * jy * _x_pixels_tile + 16 * jx]);
                                c[1] = hex2dec(&tileHex[16 * jy * _x_pixels_tile + 16 * jx + 2]);
                                c[2] = hex2dec(&tileHex[16 * jy * _x_pixels_tile + 16 * jx + 4]);
                                c[3] = hex2dec(&tileHex[16 * jy * _x_pixels_tile + 16 * jx + 6]);
                                c[4] = hex2dec(&tileHex[16 * jy * _x_pixels_tile + 16 * jx + 8]);
                                c[5] = hex2dec(&tileHex[16 * jy * _x_pixels_tile + 16 * jx + 10]);
                                c[6] = hex2dec(&tileHex[16 * jy * _x_pixels_tile + 16 * jx + 12]);
                                c[7] = hex2dec(&tileHex[16 * jy * _x_pixels_tile + 16 * jx + 14]);
                                values[tilerow * _y_pixels_tile * gridblock_x_size + tilecol * _x_pixels_tile + jy * gridblock_x_size + jx] = (_hasNodataValue[layerNr] && (_noDataValues[layerNr] == d)) || std::isnan(d) || std::isinf(d) ? rUNDEF : d;
                            }
                        }
                    }
                    break;
                }
                if (tilecol * _x_pixels_tile > gridblock_x_size) {
                    tilecol = 0;
                    ++tilerow;
                } else
                    ++tilecol;
            }

            grid->setBlockData(blockIndex, values);

            if(!moveIndexes(linesPerBlock, linesLeft, inLayerBlockIndex))
                break;
        }
    }

    _binaryIsLoaded = true;
    return true;
}

bool PostgresqlRasterConnector::moveIndexes(quint32& linesPerBlock, quint32& linesLeft, int& inLayerBlockIndex)
{
    ++inLayerBlockIndex;
    if ( linesLeft < linesPerBlock )
        return false;
    linesLeft -= linesPerBlock;
    return true;
}

bool PostgresqlRasterConnector::store(IlwisObject *data, const IOOptions &options)
{  
    return true;
}

IlwisTypes PostgresqlRasterConnector::getStoreType(QString pixel_type, IDomain & dom) const
{
    IlwisTypes stPostgres;
    QString domName;
    if (pixel_type == "1BB") {
        domName = NumericDomain::standardNumericDomainName(0, 1, 1);
        stPostgres = itINT8;
    } else if (pixel_type == "2BUI") {
        domName = NumericDomain::standardNumericDomainName(0, 3, 1);
        stPostgres = itINT8;
    } else if (pixel_type == "4BUI") {
        domName = NumericDomain::standardNumericDomainName(0, 15, 1);
        stPostgres = itINT8;
    } else if (pixel_type == "8BUI") {
        domName = NumericDomain::standardNumericDomainName(0, 255, 1);
        stPostgres = itUINT8;
    } else if (pixel_type == "8BSI") {
        domName = NumericDomain::standardNumericDomainName(-128, 127, 1);
        stPostgres = itINT8;
    } else if (pixel_type == "16BSI") {
        domName = NumericDomain::standardNumericDomainName(-32768 + 2, 32767 - 2, 1);
        stPostgres = itINT16;
    } else if (pixel_type == "16BUI") {
        domName = NumericDomain::standardNumericDomainName(0, 65535 - 2, 1);
        stPostgres = itUINT16;
    } else if (pixel_type == "32BSI") {
        domName = NumericDomain::standardNumericDomainName(-2147483647L - 1 + 2, 2147483647L - 2, 1);
        stPostgres = itINT32;
    } else if (pixel_type == "32BUI") {
        domName = NumericDomain::standardNumericDomainName(0, 0xffffffffUL - 2, 1);
        stPostgres = itUINT32;
    } else if (pixel_type == "32BF") {
        domName = NumericDomain::standardNumericDomainName(-1e100, 1e100, 0.0);
        stPostgres = itFLOAT;
    } else if (pixel_type == "64BF") {
        domName = NumericDomain::standardNumericDomainName(-1e100, 1e100, 0.0);
        stPostgres = itDOUBLE;
    } else {
        domName = NumericDomain::standardNumericDomainName(-1e100, 1e100, 0.0);
        stPostgres = itDOUBLE;
    }
    dom.prepare(domName);
    return stPostgres;
}

char PostgresqlRasterConnector::hex2dec(const char * str)
{
    char h = str[0];
    char l = str[1];
    char h1 = (h <= '9') ? (h - '0') : (h - 'a' + 10);
    char l1 = (l <= '9') ? (l - '0') : (l - 'a' + 10);
    char hl = (h1 << 4) | l1;
    return hl;
}

double PostgresqlRasterConnector::getNodataVal(QString val, QString & pixel_type) {

    if (pixel_type == "32BF") {
        float f;
        char * c = (char*)(&f);
        if (val == "Infinity") {
            c[0] = PostgresqlRasterConnector::hex2dec("00");
            c[1] = PostgresqlRasterConnector::hex2dec("00");
            c[2] = PostgresqlRasterConnector::hex2dec("80");
            c[3] = PostgresqlRasterConnector::hex2dec("7F");
            return f;
        } else if (val == "-Infinity") {
            c[0] = PostgresqlRasterConnector::hex2dec("00");
            c[1] = PostgresqlRasterConnector::hex2dec("00");
            c[2] = PostgresqlRasterConnector::hex2dec("80");
            c[3] = PostgresqlRasterConnector::hex2dec("FF");
            return f;
        } else if (val == "NaN") {
            c[0] = PostgresqlRasterConnector::hex2dec("00");
            c[1] = PostgresqlRasterConnector::hex2dec("00");
            c[2] = PostgresqlRasterConnector::hex2dec("C0");
            c[3] = PostgresqlRasterConnector::hex2dec("FF");
            return f;
        } else
            return val.toDouble();
    } else if (pixel_type == "64BF") {
        double d;
        char * c = (char*)(&d);
        if (val == "Infinity") {
            c[0] = PostgresqlRasterConnector::hex2dec("00");
            c[1] = PostgresqlRasterConnector::hex2dec("00");
            c[2] = PostgresqlRasterConnector::hex2dec("00");
            c[3] = PostgresqlRasterConnector::hex2dec("00");
            c[4] = PostgresqlRasterConnector::hex2dec("00");
            c[5] = PostgresqlRasterConnector::hex2dec("00");
            c[6] = PostgresqlRasterConnector::hex2dec("F0");
            c[7] = PostgresqlRasterConnector::hex2dec("7F");
            return d;
        } else if (val == "-Infinity") {
            c[0] = PostgresqlRasterConnector::hex2dec("00");
            c[1] = PostgresqlRasterConnector::hex2dec("00");
            c[2] = PostgresqlRasterConnector::hex2dec("00");
            c[3] = PostgresqlRasterConnector::hex2dec("00");
            c[4] = PostgresqlRasterConnector::hex2dec("00");
            c[5] = PostgresqlRasterConnector::hex2dec("00");
            c[6] = PostgresqlRasterConnector::hex2dec("F0");
            c[7] = PostgresqlRasterConnector::hex2dec("fF");
            return d;
        } else if (val == "NaN") {
            c[0] = PostgresqlRasterConnector::hex2dec("00");
            c[1] = PostgresqlRasterConnector::hex2dec("00");
            c[2] = PostgresqlRasterConnector::hex2dec("00");
            c[3] = PostgresqlRasterConnector::hex2dec("00");
            c[4] = PostgresqlRasterConnector::hex2dec("00");
            c[5] = PostgresqlRasterConnector::hex2dec("00");
            c[6] = PostgresqlRasterConnector::hex2dec("F8");
            c[7] = PostgresqlRasterConnector::hex2dec("FF");
            return d;
        } else
            return val.toDouble();
    } else
        return val.toDouble();
}
