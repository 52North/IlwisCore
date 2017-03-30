#include <QSqlQuery>
#include "raster.h"
#include "pixeliterator.h"
#include "blockiterator.h"
#include "rasterfilter.h"

using namespace Ilwis;

RasterFilter::RasterFilter() : _valid(false)
{
}

bool RasterFilter::isValid() const
{
    return _valid;
}

//-------------------------------------
LinearGridFilter::LinearGridFilter(const QString &name)
{
    definition(name);
}

bool LinearGridFilter::fillDef(int xsize, int ysize, const QStringList& numbers){
    _rows = xsize;
    _columns = ysize;
    int index = 0;
    double sum = 0;
    _filterdef.resize( ysize);
    for(std::vector<double>& row : _filterdef){
        row.resize(xsize);
        for(double& v : row)    {
            v = numbers[index++] .toDouble() ;
            sum += v;
        }
    }
    _gain = sum != 0 ? 1.0 / sum : 1.0;

    return true;
}

bool LinearGridFilter::makeCustomFilter(const QString& definition){
    QString def = definition.mid(5, definition.size() - 5);
    QStringList parts = def.split(",");
    if ( parts.size() == 1 || parts.size()==2)    {
        QStringList numbers = parts[0].split(" ");
        if ( numbers.size() % 2 == 0){
            kernel()->issues()->log(TR("Custom linear filters must have odd sizes"));
            return false;
        }
        double root = std::sqrt(numbers.size() );
        if (std::abs(root - (int)root )> EPS5){
            kernel()->issues()->log(TR("Custom linear filters must be square if no dimensions are given"));
            return false;
        }
        fillDef(root,root,numbers);
        if ( parts.size() == 2){
            _gain = parts[1].toDouble() ;
        }
    }else {
        if ( parts.size() == 3 || parts.size() == 4){
            int xsize = parts[0].toInt();
            int ysize = parts[1].toInt();
            QStringList numbers = parts[2].split(" ");
            fillDef(xsize, ysize, numbers);
        }
        if (parts.size() == 4){
            _gain = parts[3].toDouble() ;
        }
    }
    return true;
}

bool LinearGridFilter::definition(const QString &name)
{
    if ( name.indexOf("code=") != -1){
        _valid =  makeCustomFilter(name);
        return _valid;
    }
    QString query = QString("select * from filters where code='%1'").arg(name.toLower());
    InternalDatabaseConnection stmt;
    if (stmt.exec(query)) {
        if ( stmt.next()) {
            bool ok1,ok2,ok3;
            _columns = stmt.value("columns").toUInt(&ok1);
            _rows = stmt.value("rows").toUInt(&ok2);
            if ( _rows % 2 != 0 && _columns % 2 != 0) {
                _gain = stmt.value("gain").toDouble(&ok3);
                if (ok1 && ok2 && ok3){
                    QString def = stmt.value("definition").toString();
                    QStringList parts = def.split(" ");
                    if (parts.size() == (_rows * _columns) && _rows > 0 && _columns > 0) {
                        _filterdef.resize(_rows);
                        for(quint32 y=0; y < _rows && ok1; ++y){
                            _filterdef[y].resize(_columns);
                            for(quint32 x=0; x < _columns; ++x){
                                double v = parts[y * _columns + x].toDouble(&ok1);
                                if (!ok1) break;
                                _filterdef[y][x] = v;
                            }
                        }
                        _valid = true;
                    }
                }
            }
        }
    }
    if ( !_valid)
        ERROR2(ERR_NO_INITIALIZED_2,"filter",name);
    return _valid;
}

double LinearGridFilter::applyTo(const GridBlock& block)
{
    double v = 0;
    qint32 dy = _rows/2;
    qint32 dx = _columns/2;
    for(qint32 y=-dy; y <= dy; ++y) {
        for(qint32 x=-dx ; x <= dx; ++x) {
            double val = block(x , y , 0);
            if ( val != rUNDEF && v != rUNDEF){
                v +=  val * _filterdef[y + dy][x + dx];
            }else {
                v = rUNDEF;
            }

        }
    }
    if ( v != rUNDEF)
        v *= _gain;

    return v;
}

QSize LinearGridFilter::size() const
{
    return QSize(_columns, _rows);
}

//-------------------------------------------------------------------
RankOrderGridFilter::RankOrderGridFilter(const QString &name)
{
    QString query = QString("select * from filters where code='%1'").arg(name.toLower());
    InternalDatabaseConnection stmt;
    if (stmt.exec(query)) {
        if ( stmt.next()) {
            bool ok1,ok2;
            _columns = stmt.value("columns").toUInt(&ok1);
            _rows = stmt.value("rows").toUInt(&ok2);
            _valid = true;
            if (ok1 && ok2 ){
                _index = stmt.value("definition").toInt(&_valid);
            }else
                _valid = false;
        }
    }
    if ( !_valid)
        ERROR2(ERR_NO_INITIALIZED_2,"filter",name);
}

double RankOrderGridFilter::applyTo(const GridBlock &block)
{
    std::vector<double> values = block.toVector();
    std::sort(values.begin(), values.end());
    if ( _index  < values.size()) {
        return values[_index];
    }
    return rUNDEF;
}

void RankOrderGridFilter::colrow(quint32 col, quint32 row)
{
    _rows = row;
    _columns = col;
}

void RankOrderGridFilter::index(quint32 index)
{
    _index = index;
}

QSize RankOrderGridFilter::size() const
{
   return QSize(_columns, _rows);
}
