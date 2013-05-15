#include "ilwis.h"
#include "size.h"

using namespace Ilwis;

Size::Size() : QSize(0,0), _top(0)
{

}

Size::Size(qint32 x, qint32 y, qint32 z) : QSize(x,y), _top(z) {
    check();
}

Size::Size(const Size& sz) : QSize(sz.xsize(), sz.ysize()){
    _top = sz.zsize();
}

Size::Size(const QSize& sz) : QSize(sz.width(), sz.height()){
    _top=1;
}

Size& Size::operator+=(const Size& sz){
    QSize::operator +=(sz);
    _top += sz.zsize();
    check();
    return *this;
}

Size& Size::operator-=(const Size& sz){
    QSize::operator -=(sz);
    _top -= sz.xsize();
    check();


    return *this;
}

Size& Size::operator*=(double f){
    QSize::operator *=(f);
    _top *= f;
    check();

    return *this;
}

QSize Size::toQSize() const
{
    return QSize(xsize(), ysize());
}

qint32 Size::xsize() const{
    return width();
}
qint32 Size::ysize() const{
    return height();
}
qint32 Size::zsize() const{
    return _top;
}
void Size::xsize(qint32 x){
    rwidth() = x;
}

void Size::ysize(qint32 y){
    rheight() = y;
}

void Size::zsize(qint32 z){
    _top = z;
}

quint64 Size::totalSize() const{
    return xsize() * ysize() * zsize();
}

bool Size::contains(qint32 x, qint32 y, qint32 z) const
{
    return x <= xsize() && y <= ysize() && z <= zsize();
}

bool Size::isValid() const{
    return QSize::isValid() && _top > 0;
}
bool Size::isNull() const{
    return QSize::isNull() && _top == 0;
}

void Size::check()
{
    if ( width() < 0)
        setWidth(0);
    if ( height() < 0)
        setHeight(0);
    if (_top < 0)
        _top = 0;
}

//----------------------------------------
Size operator+(const Size& sz1, const Size& sz2){
    return Size(sz1.xsize() + sz2.xsize(), sz1.ysize() + sz2.ysize(), sz1.zsize() + sz2.zsize());
}

Size operator-(const Size& sz1, const Size& sz2){
    return Size(sz1.xsize() - sz2.xsize(), sz1.ysize() - sz2.ysize(), sz1.zsize() - sz2.zsize());
}

bool Ilwis::operator==(const Ilwis::Size& sz1, const Ilwis::Size& sz2){
    return sz1.xsize() == sz2.xsize() && sz1.ysize() == sz2.ysize() && sz1.zsize() == sz2.zsize();
}

bool Ilwis::operator!=(const Ilwis::Size& sz1, const Ilwis::Size& sz2){
    return operator ==(sz1, sz2);
}

