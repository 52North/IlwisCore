#include "../../core/kernel.h"
#include "../../core/ilwisobjects/ilwisdata.h"
#include "../../core/util/range.h"
#include "../../core/ilwisobjects/domain/domain.h"
#include "../../core/ilwisobjects/domain/domainitem.h"
#include "../../core/ilwisobjects/domain/itemdomain.h"
#include "../../core/ilwisobjects/domain/numericrange.h"
#include "../../core/ilwisobjects/domain/itemrange.h"
#include "../../core/ilwisobjects/domain/interval.h"
#include "../../core/ilwisobjects/domain/identifieritem.h"
#include "../../core/ilwisobjects/domain/thematicitem.h"
#include "../../core/ilwisobjects/domain/intervalrange.h"
#include "../../core/ilwisobjects/domain/identifierrange.h"
#include "../../core/ilwisobjects/domain/colorrange.h"
#include "../../core/ilwisobjects/domain/coloritem.h"
#include "../../core/ilwisobjects/domain/rangeiterator.h"

#include "pythonapi_pyobject.h"
#include "pythonapi_range.h"
#include "pythonapi_qvariant.h"

#include "pythonapi_error.h"

#define BIGTIME 1e150

using namespace pythonapi;

Range::Range(){
}

Range::~Range(){
}

Range::Range(Ilwis::Range *rng) : _range(rng) {

}

bool Range::__bool__() const
{
    return _range->isValid();
}

std::string Range::__str__()
{
    return _range->toString().toStdString();
}

IlwisTypes Range::ilwisType()
{
    return itANY;
}

IlwisTypes Range::valueType() const
{
    return _range->valueType();
}

PyObject *Range::ensure(const PyObject *value, bool inclusive) const
{
   std::unique_ptr<QVariant> v(PyObject2QVariant(value));
   QVariant var = _range->ensure(*v, inclusive);
   return QVariant2PyObject(var);
}

bool Range::isContinuous() const
{
    return _range->isContinuous();
}

PyObject *Range::impliedValue(const PyObject *value) const {
    std::unique_ptr<QVariant> v(PyObject2QVariant(value));
    QVariant var = _range->impliedValue(*v);
    return QVariant2PyObject(var);
}

bool Range::contains(const PyObject *value, bool inclusive) const
{
   std::unique_ptr<QVariant> v(PyObject2QVariant(value));
   return _range->contains(*v, inclusive);
}

//----------------------------------------------------------------------------
NumericRange::NumericRange(double mi, double ma, double resolution)
{
    _range.reset(new Ilwis::NumericRange(mi, ma, resolution))    ;
}

NumericRange::NumericRange(const NumericRange &vr) : NumericRange(vr.min(), vr.max(), vr.resolution())
{
}

NumericRange::NumericRange(Ilwis::NumericRange* nr){
    _range.reset(nr);
}

NumericRange::NumericRange(){
    _range.reset(new Ilwis::NumericRange());
}

NumericRange::~NumericRange(){

}

bool NumericRange::contains(double v, bool inclusive) const
{
    return static_cast<const Ilwis::NumericRange*>(_range.get())->contains(v, inclusive);
}

double NumericRange::max() const
{
    return static_cast<const Ilwis::NumericRange*>(_range.get())->max();
}

double NumericRange::min() const
{
    return static_cast<const Ilwis::NumericRange*>(_range.get())->min();
}

void NumericRange::setMax(double value)
{
    static_cast<Ilwis::NumericRange*>(_range.get())->max(value);
}

void NumericRange::setMin(double value)
{
    static_cast<Ilwis::NumericRange*>(_range.get())->min(value);
}

double NumericRange::distance() const
{
    return static_cast<const Ilwis::NumericRange*>(_range.get())->distance();
}

void NumericRange::setResolution(double resolution)
{
    static_cast<Ilwis::NumericRange*>(_range.get())->resolution(resolution);
}

double NumericRange::resolution() const
{
    return static_cast<const Ilwis::NumericRange*>(_range.get())->resolution();
}

void NumericRange::set(const NumericRange &vr)
{
    setResolution(vr.resolution());
    setMax(vr.max());
    setMin(vr.min());
}

NumericRangeIterator NumericRange::__iter__(){
    return NumericRangeIterator(this);
}

NumericRangeIterator NumericRange::begin(){
    Ilwis::NumericRange* ilwRng = static_cast<Ilwis::NumericRange*>(_range.get());
    Ilwis::NumericRangeIterator* ilwIt = new Ilwis::NumericRangeIterator(Ilwis::begin(*ilwRng));
    NumericRangeIterator pyIt(ilwIt);
    pyIt.setRange(this);
    return pyIt;
}

NumericRangeIterator NumericRange::end(){
    Ilwis::NumericRange* ilwRng = static_cast<Ilwis::NumericRange*>(_range.get());
    Ilwis::NumericRangeIterator* ilwIt = new Ilwis::NumericRangeIterator(Ilwis::end(*ilwRng));
    NumericRangeIterator pyIt(ilwIt);
    pyIt.setRange(this);
    return pyIt;
}

void NumericRange::clear()
{
    static_cast<Ilwis::NumericRange*>(_range.get())->clear();
}

//-------------------------------------------------------------
void ItemRange::remove(const std::string &name)
{
    static_cast<Ilwis::ItemRange*>(_range.get())->remove(name.c_str());
}

quint32 ItemRange::count()
{
    return static_cast<Ilwis::ItemRange*>(_range.get())->count();
}

void ItemRange::clear()
{
    static_cast<Ilwis::ItemRange*>(_range.get())->clear();
}

//------------------------------------------------------------

NumericItemRange::NumericItemRange()
{
    _range.reset(new Ilwis::IntervalRange());
}


void NumericItemRange::add(std::string name, double min, double max, double resolution){

    QString label = QString::fromStdString(name);
    Ilwis::Interval *numitem;
    if ( label == sUNDEF)
        return;
    if(resolution == 0){
        numitem = new Ilwis::Interval(label, { min, max});
    } else {
        numitem = new Ilwis::Interval(label, { min, max, resolution});
    }

    static_cast<Ilwis::ItemRange*>(_range.get())->add(numitem);
}

void NumericItemRange::add(PyObject *item)
{
    if (CppTupleElementCount(item) == 3 || CppTupleElementCount(item) == 4){
        QString label = QString::fromStdString(CppTupleElement2String(item,0));
        if ( label == sUNDEF)
            return;

        double rmin = CppTupleElement2Double(item, 1);
        if(rmin == rUNDEF)
            rmin = (double)CppTupleElement2Long(item, 1);
        double rmax = CppTupleElement2Double(item,2);
        if(rmax == rUNDEF)
            rmax = (double)CppTupleElement2Long(item, 2);
        if ( rmin == rUNDEF || rmax == rUNDEF)
            return;
        Ilwis::Interval *numitem;
        if ( CppTupleElementCount(item) == 4){
            double resolution = CppTupleElement2Double(item,3);
            if(resolution == rUNDEF)
                resolution = (double)CppTupleElement2Long(item, 3);
            if ( resolution == rUNDEF)
                return;
            numitem = new Ilwis::Interval(label, { rmin, rmax,resolution});
        }else
            numitem = new Ilwis::Interval(label, { rmin, rmax});

        static_cast<Ilwis::ItemRange*>(_range.get())->add(numitem);
    }
}

PyObject* NumericItemRange::listAll(){
    int entries = this->count();
    PyObject* list = newPyList(entries);
    for(int i=0; i < entries; i++){
        Ilwis::DomainItem* item = static_cast<Ilwis::IntervalRange*>(_range.get())->item(i).data();

        PyObject* tuple = newPyTuple(2);
        setTupleItem(tuple, 0, PyBuildString(static_cast<Ilwis::Interval*>(item)->name().toStdString()));
        setTupleItem(tuple, 1, PyBuildString(static_cast<Ilwis::Interval*>(item)->range().toString().toStdString()));

        setListItem(list, i, tuple);
    }
    return list;
}

double NumericItemRange::index(double v)
{
    return static_cast<Ilwis::IntervalRange*>(_range.get())->index(v);
}

qint32 NumericItemRange::gotoIndex(qint32 index, qint32 step) const
{
    return static_cast<Ilwis::IntervalRange*>(_range.get())->gotoIndex(index, step);
}

NumericItemRange *NumericItemRange::clone()
{
    NumericItemRange* newRange = new NumericItemRange();
    newRange->_range.reset(static_cast<Ilwis::IntervalRange*>(this->_range.get()->clone()));
    return newRange;
}

//------------------------------------------------------------
IndexedItemRange::IndexedItemRange()
{
    _range.reset(new Ilwis::IndexedIdentifierRange());
}

void IndexedItemRange::add(PyObject *item){
    Ilwis::IndexedIdentifier* ident;
    if(PyTupleCheckExact(item)){
        if(CppTupleElementCount(item) >=1){
            QString label = QString::fromStdString(CppTupleElement2String(item, 0));
            if(CppTupleElementCount(item) >=2){
                quint32 index = CppTupleElement2ulonglong(item, 1);
                if(CppTupleElementCount(item) >=3){
                    quint32 count = CppTupleElement2ulonglong(item, 1);
                    ident = new Ilwis::IndexedIdentifier(label, index, count);
                } else {
                    ident = new Ilwis::IndexedIdentifier(label, index);
                }
            } else {
                ident = new Ilwis::IndexedIdentifier(label);
            }
        }
    } else if (PyUnicodeCheckExact(item)){
        QString label = QString::fromStdString(CppString2stdString(item));
        ident = new Ilwis::IndexedIdentifier(label);
    }
    static_cast<Ilwis::IndexedIdentifierRange*>(_range.get())->add(ident);

}

qint32 IndexedItemRange::gotoIndex(qint32 index, qint32 step) const
{
    return static_cast<Ilwis::IndexedIdentifierRange*>(_range.get())->gotoIndex(index, step);
}

IndexedItemRange *IndexedItemRange::clone()
{
    IndexedItemRange* newRange = new IndexedItemRange();
    newRange->_range.reset(static_cast<Ilwis::IndexedIdentifierRange*>(this->_range.get()->clone()));
    return newRange;
}

//------------------------------------------------------------
NamedItemRange::NamedItemRange()
{
    _range.reset(new Ilwis::NamedIdentifierRange());
}

void NamedItemRange::add(PyObject *item)
{
    QString name = QString::fromStdString(CppString2stdString(item));
    Ilwis::NamedIdentifier *id = new Ilwis::NamedIdentifier(name);
    static_cast<Ilwis::ItemRange*>(_range.get())->add(id);

}

PyObject* NamedItemRange::listAll(){
    int entries = this->count();
    PyObject* list = newPyList(entries);
    for(int i=0; i < entries; i++){
        Ilwis::DomainItem* item = static_cast<Ilwis::NamedIdentifierRange*>(_range.get())->item(i).data();
        setListItem(list, i, PyBuildString(static_cast<Ilwis::NamedIdentifier*>(item)->name().toStdString()));
    }
    return list;
}

qint32 NamedItemRange::gotoIndex(qint32 index, qint32 step) const
{
    return static_cast<Ilwis::NamedIdentifierRange*>(_range.get())->gotoIndex(index, step);
}

NamedItemRange *NamedItemRange::clone()
{
    NamedItemRange* newRange = new NamedItemRange();
    newRange->_range.reset(static_cast<Ilwis::NamedIdentifierRange*>(this->_range.get()->clone()));
    return newRange;
}

//-----------------------------------------------------------

ThematicRange::ThematicRange()
{
    _range.reset(new Ilwis::ThematicRange());
}

void ThematicRange::add(std::string name, std::string id, std::string descr){
    QString description = sUNDEF, code = sUNDEF;
    QString label = QString::fromStdString(name);

    if(label == sUNDEF)
        return;
    if(id != "")
        code = QString::fromStdString(id);
    if(descr != "")
        description = QString::fromStdString(descr);

    Ilwis::ThematicItem *titem = new Ilwis::ThematicItem({label, code, description}, count());
    static_cast<Ilwis::ItemRange*>(_range.get())->add(titem);
}

void ThematicRange::add(PyObject *item)
{
    int elements = CppTupleElementCount(item);

    if ( elements > 0 && elements <=3 ) {
        QString description = sUNDEF, code = sUNDEF;
        QString label = QString::fromStdString(CppTupleElement2String(item,0));
        if ( label == sUNDEF)
            return;
        if ( elements >= 2){
            code = QString::fromStdString(CppTupleElement2String(item,1));
        }
        if ( elements == 3){
            description = QString::fromStdString(CppTupleElement2String(item,2));
        }
        Ilwis::ThematicItem *titem = new Ilwis::ThematicItem({label, code, description}, count());
        static_cast<Ilwis::ItemRange*>(_range.get())->add(titem);
    }
}

PyObject* ThematicRange::listAll(){
    int entries = this->count();
    PyObject* list = newPyList(entries);
    for(int i=0; i < entries; i++){
        Ilwis::DomainItem* item = static_cast<Ilwis::ThematicRange*>(_range.get())->item(i).data();

        PyObject* tuple = newPyTuple(3);
        setTupleItem(tuple, 0, PyBuildString(static_cast<Ilwis::ThematicItem*>(item)->name().toStdString()));
        setTupleItem(tuple, 1, PyBuildString(static_cast<Ilwis::ThematicItem*>(item)->code().toStdString()));
        setTupleItem(tuple, 2, PyBuildString(static_cast<Ilwis::ThematicItem*>(item)->description().toStdString()));

        setListItem(list, i, tuple);
    }
    return list;
}

ThematicRange *ThematicRange::clone()
{
    ThematicRange* newRange = new ThematicRange();
    newRange->_range.reset(static_cast<Ilwis::ThematicRange*>(this->_range.get()->clone()));
    return newRange;
}


//-----------------------------------------------------------------

ColorRange::ColorRange(){

}

ColorRange::ColorRange(IlwisTypes tp, ColorModel clrmodel)
{
   /* int enumVal = clrmodel;
    Ilwis::ColorRangeBase::ColorModel ilwCol = static_cast<Ilwis::ColorRangeBase::ColorModel>(enumVal);
    _range.reset(Ilwis::ColorRangeBase(tp, ilwCol));*/
}

ColorModel ColorRange::defaultColorModel() const
{
    int enumVal = static_cast<const Ilwis::ContinuousColorRange*>(_range.get())->defaultColorModel();
    ColorModel colormodel = static_cast<ColorModel>(enumVal);
    return colormodel;
}

void ColorRange::defaultColorModel(ColorModel m)
{
    int enumVal = m;
    Ilwis::ColorRangeBase::ColorModel ilwCol = static_cast<Ilwis::ColorRangeBase::ColorModel>(enumVal);
    static_cast<Ilwis::ContinuousColorRange*>(_range.get())->defaultColorModel(ilwCol);
}

Color ColorRange::toColor(PyObject *v, ColorModel colortype, const std::string& name)
{
    if ( PyTupleCheckExact(v))
        return Color(colortype, v);
    else if ( PyUnicodeCheckExact(v)){
        QRegExp separ("[(]|,| |[)]");
        QStringList parts = QString::fromStdString(CppString2stdString(v)).split(separ);
        if(parts.last().isEmpty())
            parts.removeLast();
        PyObject* list = newPyTuple(6);
        bool ok1,ok2,ok3, ok4, ok5=true;
        if ( parts.size() >= 5){
            setTupleItem(list, 0, PyBuildDouble(parts[1].toDouble(&ok1)));
            setTupleItem(list, 1, PyBuildDouble(parts[2].toDouble(&ok2)));
            setTupleItem(list, 2, PyBuildDouble(parts[3].toDouble(&ok3)));
            setTupleItem(list, 3, PyBuildDouble(parts[4].toDouble(&ok4)));
            parts.size()== 6 ? setTupleItem(list, 4, PyBuildDouble(parts[5].toDouble(&ok5))) : rUNDEF;
            if(! (ok1 && ok2 && ok3 && ok4 && ok5))
                return Color();

            ColorRange helper = ColorRange();
            std::string type = (parts[0].toLower()).toStdString();
            colortype = helper.stringToColorModel(type);

            return Color(colortype, list, name);
        }
    }
    return Color();
}

Color ColorRange::toColor(quint64 clrint, ColorModel clrModel, const std::string& name){
    Ilwis::ColorRangeBase::ColorModel ilwModel = static_cast<Ilwis::ColorRangeBase::ColorModel>(clrModel);
    QColor ilwCol = Ilwis::ColorRangeBase::toColor(clrint, ilwModel);

    std::string colStr = (Ilwis::ColorRangeBase::toString(ilwCol, ilwModel)).toStdString();
    Color result = ColorRange::toColor(PyBuildString(colStr),clrModel, name);
    return result;
}

std::string ColorRange::toString(const Color &clr, ColorModel clrType)
{
    QString colors;
    switch(clrType){
    case ColorModel::cmCYMKA:
        colors += QString("CMYKA(%1,%2,%3,%4,%5)").arg(clr.getItem("cyan")).arg(clr.getItem("magenta")).arg(clr.getItem("yellow")).arg(clr.getItem("black")).arg(clr.getItem("alpha"));
        break;
    case ColorModel::cmHSLA:
        colors += QString("HSLA(%1,%2,%3,%4)").arg(clr.getItem("hue")).arg(clr.getItem("saturation")).arg(clr.getItem("lightness")).arg(clr.getItem("alpha"));
        break;
    case ColorModel::cmRGBA:
        colors += QString("RGBA(%1,%2,%3,%4)").arg(clr.getItem("red")).arg(clr.getItem("blue")).arg(clr.getItem("green")).arg(clr.getItem("alpha"));
        break;
    case ColorModel::cmGREYSCALE:
        break;
    case ColorModel::cmNONE:
        break;
    }
    return colors.toStdString();
}

ColorModel ColorRange::stringToColorModel(std::string clrmd){
    if(clrmd == "rgba"){
        return ColorModel::cmRGBA;
    } else if(clrmd == "cmyka"){
        return ColorModel::cmCYMKA;
    } else if(clrmd == "hsla"){
        return ColorModel::cmHSLA;
    } else if(clrmd == "greyscale"){
        return ColorModel::cmGREYSCALE;
    }else{
        return ColorModel::cmRGBA;
    }
}

Color ColorRange::qColorToColor(QColor qCol, const std::string& name) const{
    Ilwis::ColorRangeBase::ColorModel ilwMod = static_cast<const Ilwis::ContinuousColorRange*>(_range.get())->defaultColorModel();
    std::string colStr = (Ilwis::ColorRangeBase::toString(qCol, ilwMod)).toStdString();
    ColorModel type = static_cast<ColorModel>(ilwMod);
    return toColor(PyBuildString(colStr),type, name);
}

QColor ColorRange::colorToQColor(const Color& pyCol) const{
    QString colorStr = QString::fromStdString(pyCol.toString());
    Ilwis::ColorRangeBase::ColorModel ilwColor = static_cast<Ilwis::ColorRangeBase::ColorModel>(pyCol.getColorModel());
    return Ilwis::ContinuousColorRange::toColor(QVariant(colorStr), ilwColor);
}

//--------------------------------------------------------------------------------

ContinuousColorRange::ContinuousColorRange()
{
    _range.reset(new Ilwis::ContinuousColorRange());
}

ContinuousColorRange::ContinuousColorRange(const Color &clr1, const Color &clr2)
{
    QString color1 = QString::fromStdString(clr1.toString());
    QString color2 = QString::fromStdString(clr2.toString());
    Ilwis::ColorRangeBase::ColorModel ilwColor = static_cast<Ilwis::ColorRangeBase::ColorModel>(clr1.getColorModel());

    QColor col1 = Ilwis::ContinuousColorRange::toColor(QVariant(color1), ilwColor);
    QColor col2 = Ilwis::ContinuousColorRange::toColor(QVariant(color2), ilwColor);
     _range.reset(new Ilwis::ContinuousColorRange(col1, col2, ilwColor));
}

ContinuousColorRange *ContinuousColorRange::clone() const
{
    ContinuousColorRange* colRa = new ContinuousColorRange();
    colRa->_range.reset(this->_range.get()->clone());
    return colRa;
}

PyObject* ContinuousColorRange::ensure(const PyObject *v, bool inclusive) const
{
    std::unique_ptr<QVariant> qvar(PyObject2QVariant(v));
    QVariant result = static_cast<Ilwis::ContinuousColorRange*>(_range.get())->ensure(*qvar, inclusive);
    return QVariant2PyObject(result);
}

bool ContinuousColorRange::containsVar(const PyObject *v, bool inclusive) const
{
    std::unique_ptr<QVariant> qvar(PyObject2QVariant(v));

    return _range.get()->contains(*qvar, inclusive);

}

bool ContinuousColorRange::containsColor(const Color &clr, bool inclusive) const
{
    QString color = QString::fromStdString(clr.toString());

    Ilwis::ColorRangeBase::ColorModel ilwColor = static_cast<const Ilwis::ContinuousColorRange*>(_range.get())->defaultColorModel();

    QColor col = Ilwis::ContinuousColorRange::toColor(QVariant(color), ilwColor);

    return _range.get()->contains(col, inclusive);

}

bool ContinuousColorRange::containsRange(ColorRange *v, bool inclusive) const
{
    return static_cast<const Ilwis::ContinuousColorRange*>(_range.get())->contains(static_cast<ContinuousColorRange*>(v)->_range.get(), inclusive);
}

Color ContinuousColorRange::impliedValue(const PyObject *v) const
{
    std::unique_ptr<QVariant> qvar(PyObject2QVariant(v));
    QVariant colVar = static_cast<const Ilwis::ContinuousColorRange*>(_range.get())->impliedValue(*qvar);
    QColor ilwCol = colVar.value<QColor>();

    return qColorToColor(ilwCol);

}


//---------------------------------------------------------------------------------------

ColorPalette::ColorPalette(){
    _range.reset(new Ilwis::ColorPalette());
}

Color ColorPalette::item(quint32 raw) const{
    Ilwis::SPDomainItem ilwItem = static_cast<const Ilwis::ColorPalette*>(_range.get())->item(raw);
    return itemToColor(ilwItem);
}

Color ColorPalette::item(const std::string& name) const{
    Ilwis::SPDomainItem ilwItem = static_cast<const Ilwis::ColorPalette*>(_range.get())->item(QString::fromStdString(name));
    return itemToColor(ilwItem);
}

Color ColorPalette::itemByOrder(quint32 raw) const{
    Ilwis::SPDomainItem ilwItem = static_cast<const Ilwis::ColorPalette*>(_range.get())->itemByOrder(raw);
    return itemToColor(ilwItem);
}

Color ColorPalette::color(int index){
    QColor ilwCol = static_cast<const Ilwis::ColorPalette*>(_range.get())->color(index);

    return qColorToColor(ilwCol);
}

void ColorPalette::add(const Color& pyColor){
    QColor ilwCol = colorToQColor(pyColor);
    Ilwis::ColorItem* ilwItem = new Ilwis::ColorItem(ilwCol);
    static_cast<Ilwis::ColorPalette*>(_range.get())->add(ilwItem);
}

void ColorPalette::remove(const std::string &name){
    static_cast<Ilwis::ColorPalette*>(_range.get())->remove(QString::fromStdString(name));
}

void ColorPalette::clear(){
    static_cast<Ilwis::ColorPalette*>(_range.get())->clear();
}

bool ColorPalette::containsColor(const Color &clr, bool inclusive) const{
    QColor ilwCol= colorToQColor(clr);
    return static_cast<Ilwis::ColorPalette*>(_range.get())->contains(QVariant(ilwCol), inclusive);
}

bool ColorPalette::containsRange(ColorRange *v, bool inclusive) const{
    return static_cast<const Ilwis::ColorPalette*>(_range.get())->contains(static_cast<ColorPalette*>(v)->_range.get(), inclusive);
}

quint32 ColorPalette::count(){
    return static_cast<const Ilwis::ColorPalette*>(_range.get())->count();
}

Color ColorPalette::valueAt(quint32 index, ItemRange *rng){
    QColor ilwCol = static_cast<const Ilwis::ColorPalette*>(_range.get())->valueAt(index, static_cast<ColorPalette*>(rng)->_range.get());
    return qColorToColor(ilwCol);
}

Color ColorPalette::itemToColor(Ilwis::SPDomainItem item) const{
    Ilwis::SPColorItem citem = item.staticCast<Ilwis::ColorItem>();

    QColor ilwCol = citem->color();
    QString name = citem->name();

    return ColorRange::qColorToColor(ilwCol, name.toStdString());
}

qint32 ColorPalette::gotoIndex(qint32 index, qint32 step) const{
    return static_cast<const Ilwis::ColorPalette*>(_range.get())->gotoIndex(index, step);
}

void ColorPalette::add(PyObject* item){
    //from superclass, so class is not abstract
}

//---------------------------------------------------------------------------------------

TimeInterval::TimeInterval(IlwisTypes tp) : NumericRange(-BIGTIME, BIGTIME,0) {
    _range.reset(new Ilwis::TimeInterval(tp));
}

TimeInterval::TimeInterval(const PyObject* beg, const PyObject* end, std::string step, IlwisTypes tp):
    NumericRange(-BIGTIME, BIGTIME,0){
    QVariant* qvar = PyObject2QVariant(beg);
    Ilwis::Time ilwBeg = qvar->value<Ilwis::Time>();
    qvar = PyObject2QVariant(end);

    Ilwis::Time ilwEnd = qvar->value<Ilwis::Time>();
    Ilwis::Duration ilwStep {QString::fromStdString(step)};
    _range.reset(new Ilwis::TimeInterval(ilwBeg, ilwEnd, ilwStep, tp));

    delete qvar;
}

TimeInterval::TimeInterval(Ilwis::Range* ti){
    _range.reset(static_cast<Ilwis::TimeInterval*>(ti));
}

//std::string TimeInterval::__str__(){
//    QString begin = static_cast<Ilwis::TimeInterval*>(_range.get())->begin().toString();
//    QString end = static_cast<Ilwis::TimeInterval*>(_range.get())->end().toString();
//    return "Start: " + begin.toStdString() + ", End: " + end.toStdString();
//}

bool TimeInterval::contains(const std::string &value, bool inclusive) const
{
    return static_cast<Ilwis::TimeInterval*>(_range.get())->contains(QString::fromStdString(value));
}

bool TimeInterval::contains(const PyObject* value, bool inclusive) const
{
    std::unique_ptr<QVariant> qvar(PyObject2QVariant(value));
    if(qvar->value<Ilwis::Time>())
    {
        Ilwis::Time timeVal = qvar->value<Ilwis::Time>();
        return static_cast<Ilwis::TimeInterval*>(_range.get())->contains(timeVal);
    } else
        return static_cast<Ilwis::TimeInterval*>(_range.get())->contains(*qvar);
}


TimeInterval* TimeInterval::clone() const
{
    return new TimeInterval(static_cast<Ilwis::TimeInterval*>(_range.get())->clone());
}

void TimeInterval::begin(const PyObject *t)
{
    std::unique_ptr<QVariant> qvar(PyObject2QVariant(t));
    Ilwis::Time timeVal = qvar->value<Ilwis::Time>();
    static_cast<Ilwis::TimeInterval*>(_range.get())->begin(timeVal);
}

void TimeInterval::end(const PyObject *t)
{
    std::unique_ptr<QVariant> qvar(PyObject2QVariant(t));
    Ilwis::Time timeVal = qvar->value<Ilwis::Time>();
    static_cast<Ilwis::TimeInterval*>(_range.get())->end(timeVal);
}

PyObject* TimeInterval::begin() const
{
    QVariant* qvar = new QVariant(IVARIANT(static_cast<Ilwis::TimeInterval*>(_range.get())->begin()));
    return QVariant2PyObject(*qvar);
}

PyObject* TimeInterval::end() const
{
    QVariant* qvar = new QVariant(IVARIANT(static_cast<Ilwis::TimeInterval*>(_range.get())->end()));
    return QVariant2PyObject(*qvar);;
}

