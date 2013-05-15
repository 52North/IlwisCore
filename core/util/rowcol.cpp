#include <QString>
#include <QRegExp>
#include <QStringList>
#include "ilwis.h"
#include "rowcol.h"

using namespace Ilwis;

RowCol::RowCol() : _row(rUNDEF), _col(rUNDEF)
{
}

RowCol::RowCol(double r, double c)
{
  _row = r;
  _col = c;
}

RowCol::RowCol(QString &pair)
{
    QStringList parts = pair.split(QRegExp("[\\s,;]"));
    if ( parts.size() == 2) {
        _row = parts[0].toDouble();
        _col = parts[1].toDouble();
    } else {
        *this = rcUNDEF;
    }
}

double RowCol::row(bool subpixel) const
{
    if ( subpixel)
        return _row;
    return (qint32)_row;
}

void RowCol::setRow(double r)
{
    _row = r;
}

double RowCol::col(bool subpixel) const
{
    if( subpixel)
        return _col;
    return (qint32)_col;
}

void RowCol::setCol(double c)
{
    _col = c;
}

bool RowCol::operator ==(const RowCol &rc) const
{
    if ( isValid())
        return rc.row() == row() && rc.col() == col();
    return false;
}

bool RowCol::operator !=(const RowCol &rc) const
{
    return ! (operator ==(rc));
}

bool RowCol::isValid() const
{
    return _row != rUNDEF && _col != rUNDEF;
}

QString RowCol::toString() const
{
    return QString("(%1,%2)").arg(_row,_col);
}
