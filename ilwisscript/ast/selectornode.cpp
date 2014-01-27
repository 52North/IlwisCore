#include "kernel.h"
#include "errorobject.h"
#include "geometries.h"
#include "astnode.h"
#include "selectornode.h"

using namespace Ilwis;


Selector::Selector(const QString& tp) : ASTNode(tp),
    _variable(sUNDEF),
    _beginCol(sUNDEF),
    _endCol(sUNDEF),
    _beginRec(iUNDEF),
    _endRec(iUNDEF),
    _keyColumns(sUNDEF)
{
    _box = BoundingBox(Pixel(0,0), Pixel(0,0));
}

void Selector::setBounds(const QString& x1, const QString& y1, const QString& x2, const QString& y2){
    _box = BoundingBox(QString("(%1 %2, %3 %4)").arg(x1).arg(y1).arg(x2).arg(y2));
}

void Selector::setSelectorType(const QString& tp){
    _selectorType = tp;
}

BoundingBox Selector::box() const{
    return _box;

}

void Selector::setVariable(const QString& v) {
    _variable = v;
}

QString Selector::variable() const{
    return _variable;
}
QString Selector::selectorType() const{
    return _selectorType;
}

void Selector::beginCol(const QString &col)
{
    _beginCol = col;
}

QString Selector::beginCol() const
{
    return _beginCol;
}

void Selector::endCol(const QString &col)
{
    _endCol = col;
}

QString Selector::endCol() const
{
    return _endCol;
}

void Selector::beginRec(const QString &col)
{
    bool ok;
    _beginRec = col.toInt(&ok);
    if ( !ok)
        throw ErrorObject(TR("invalid record number used"));
}

quint32 Selector::beginRec() const
{
    return _beginRec;
}

void Selector::endRec(const QString &col)
{
    bool ok;
    _endRec = col.toInt(&ok);
    if ( !ok)
        throw ErrorObject(TR("invalid record number used"));
}

quint32 Selector::endRec() const
{
    return _endRec;
}

void Selector::keyColumns(const QString &keys)
{
    _keyColumns = keys;
}

QString Selector::keyColumns() const
{
    return _keyColumns;
}


