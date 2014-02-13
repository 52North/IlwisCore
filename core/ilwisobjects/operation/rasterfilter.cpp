#include <QSqlQuery>
#include "kernel.h"
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

bool LinearGridFilter::definition(const QString &name)
{
    QString query = QString("select * from filters where code='%1'").arg(name.toLower());
    QSqlQuery stmt(kernel()->database());
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
            v +=  val * _filterdef[y + dy][x + dx];
        }
    }
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
    QSqlQuery stmt(kernel()->database());
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
