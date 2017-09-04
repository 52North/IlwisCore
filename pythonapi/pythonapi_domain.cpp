#include "../../core/kernel.h"
#include "../../core/ilwiscontext.h"
#include "../../core/catalog/catalog.h"
#include "../../core/ilwisobjects/ilwisobject.h"
#include "../../core/ilwisobjects/ilwisdata.h"
#include "../../core/util/range.h"
#include "../../core/ilwisobjects/domain/domain.h"
#include "../../core/ilwisobjects/domain/numericdomain.h"
#include "../../core/ilwisobjects/domain/domainitem.h"
#include "../../core/ilwisobjects/domain/itemrange.h"
#include "../../core/ilwisobjects/domain/interval.h"
#include "../../core/ilwisobjects/domain/intervalrange.h"
#include "../../core/ilwisobjects/domain/itemdomain.h"
#include "../../core/ilwisobjects/domain/identifieritem.h"
#include "../../core/ilwisobjects/domain/identifierrange.h"
#include "../../core/ilwisobjects/domain/identifieritem.h"
#include "../../core/ilwisobjects/domain/thematicitem.h"
#include "../../core/ilwisobjects/domain/colordomain.h"
#include "../../core/ilwisobjects/domain/coloritem.h"
#include "../../core/ilwisobjects/domain/textdomain.h"
#include "../../core/ilwisobjects/domain/coloritem.h"

#include "pythonapi_pyobject.h"
#include "pythonapi_range.h"
#include "pythonapi_domain.h"
#include "pythonapi_qvariant.h"
#include "pythonapi_error.h"

using namespace pythonapi;

Domain::Domain(){
}

Domain::Domain(const Ilwis::IDomain &domain) : IlwisObject(new Ilwis::IIlwisObject(domain)){
}

Domain::~Domain(){
}

bool Domain::isStrict() const{
    return this->ptr()->as<Ilwis::Domain>()->isStrict();
}

void Domain::setStrict(bool yesno){
    this->ptr()->as<Ilwis::Domain>()->setStrict(yesno);
}

PyObject* Domain::impliedValue(PyObject *value) const{
    return value;
}

Domain Domain::parent() const{
    if(this->ptr()->as<Ilwis::Domain>()->parent().isValid()){
        unsigned long long id = this->ptr()->as<Ilwis::Domain>()->parent()->id();
        Ilwis::IDomain domain;
        domain.prepare(id);
        return Domain (domain);
    } else {
        throw Ilwis::ErrorObject(QString("No parent domain found"));
    }
}

void Domain::setParent(const Domain &dom){
    this->ptr()->as<Ilwis::Domain>()->setParent(dom.ptr()->as<Ilwis::Domain>());
}

Range& Domain::getRange(){
    Range* retRan = new Range(this->ptr()->as<Ilwis::Domain>()->range().data());
    return *retRan;
}

std::string Domain::contains(PyObject* value) const{
    std::unique_ptr<QVariant> var(PyObject2QVariant(value));
    Domain::Containement dc = (Domain::Containement)this->ptr()->as<Ilwis::Domain>()->contains(*var);
    switch(dc){
    case 1: return "cSELF";
                break;
    case 2: return "cPARENT";
                break;
    case 3: return "cDECLARED";
                break;
    case 0: return "cNONE";
        break;
    default: return "Not found anywhere";
    }
}

bool Domain::isCompatibleWith(const Domain &dom) const{
    return this->ptr()->as<Ilwis::Domain>()->isCompatibleWith(dom.ptr()->ptr());
}

void Domain::setRange(const Range &rng){
    return this->ptr()->as<Ilwis::Domain>()->range(rng._range->clone());
}

IlwisTypes Domain::valueType() const
{
    return this->ptr()->as<Ilwis::Domain>()->valueType();
}

Domain *Domain::toDomain(Object *obj){
    Domain* ptr = dynamic_cast<Domain*>(obj);
    if(!ptr)
        throw InvalidObject("cast to Domain not possible");
    return ptr;
}

const QString Domain::getStoreFormat() const {
    return "domain";
}

//--------------------------------------------------------------

NumericDomain::NumericDomain(){
    Ilwis::INumericDomain numdom;
    numdom.prepare();
    if (numdom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(numdom));
}

NumericDomain::NumericDomain(const std::string &resource){
    QString input (QString::fromStdString(resource));
    input.replace('\\','/');
    // if it is file:// (or http:// etc) leave it untouched; if not, append file:// and the working catalog path if it is missing
    if (input.indexOf("://") < 0) {
        int pos = input.indexOf('/');
        if (pos > 0) {
            if ((input.count('/') > 1) || QFileInfo(input).exists()) // full path starting with drive-letter (MS-DOS-style)
                input = "file:///" + input;
            else // container object without path, e.g. myfile.hdf/subdataset: look for it in workingCatalog()
                input = "file:///" + Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + input;
        }  else if (pos == 0) // full path starting with path-separator (UNIX-style)
            input = "file://" + input;
        else { // pos < 0: file without path, or new object
            QString file = Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + input;
            if (QFileInfo (file).exists()) // file without path
                input = "file:///" + file;
        }
    }
    Ilwis::INumericDomain numdom(input, itNUMERICDOMAIN);
    if (numdom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(numdom));
}

NumericDomain *NumericDomain::toNumericDomain(Object *obj){
    NumericDomain* ptr = dynamic_cast<NumericDomain*>(obj);
    if(!ptr)
        throw InvalidObject("cast to NumericDomain not possible");
    return ptr;
}

NumericDomain::NumericDomain(const Ilwis::INumericDomain &domain): Domain(Ilwis::IDomain(domain)){
}

//---------------------------------------------------------------

ItemDomain::ItemDomain(const Range& rng){
    IlwisTypes vt = rng.valueType();
    switch (vt) {
    case itNUMERICITEM:{
        Ilwis::IIntervalDomain interdom;
        interdom.prepare();
        if (interdom.isValid())
            this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(interdom));
        setRange(rng);
    }
        break;
    case itTHEMATICITEM:{
        Ilwis::IThematicDomain themdom;
        themdom.prepare();
        if(themdom.isValid())
            this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(themdom));
        setRange(rng);
    }
        break;
    case itINDEXEDITEM:{
        Ilwis::IIndexedIdDomain inddom;
        inddom.prepare();
        if(inddom.isValid())
            this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(inddom));
        setRange(rng);
    } break;
    case itNAMEDITEM:{
        Ilwis::INamedIdDomain namdom;
        namdom.prepare();
        if(namdom.isValid())
            this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(namdom));
        setRange(rng);
    }
        break;
    default:
        break;
    }
}

void ItemDomain::range(const Range &rng){
    Ilwis::ItemRange* itemRan =  static_cast<Ilwis::ItemRange*>(rng._range->clone());
    IlwisTypes vt = rng.valueType();
    switch (vt) {
    case itNUMERICITEM:{
        this->ptr()->as<Ilwis::ThematicDomain>()->setRange(*itemRan);
    }
        break;
    case itTHEMATICITEM:{
        this->ptr()->as<Ilwis::ThematicDomain>()->setRange(*itemRan);
    }
        break;
    case itINDEXEDITEM:{
        this->ptr()->as<Ilwis::IndexedIdDomain>()->setRange(*itemRan);
    }
        break;
    case itNAMEDITEM:{
        this->ptr()->as<Ilwis::NamedIdDomain>()->setRange(*itemRan);
    }
        break;
    default:
        break;
    }
}

quint32 ItemDomain::count()
{
    return this->ptr()->as<Ilwis::IntervalDomain>()->count();
}

void ItemDomain::setTheme(const std::string &theme)
{
    this->ptr()->as<Ilwis::IntervalDomain>()->setTheme(QString::fromStdString(theme));
}

std::string ItemDomain::theme()
{
    QString theme = this->ptr()->as<Ilwis::IntervalDomain>()->theme();
    return theme.toStdString();
}

void ItemDomain::removeItem(const std::string& nme)
{
    IlwisTypes vt = valueType();

    switch (vt) {
    case itNUMERICITEM:{
        this->ptr()->as<Ilwis::IntervalDomain>()->removeItem(QString::fromStdString(nme));
    }
        break;
    case itTHEMATICITEM:{
        this->ptr()->as<Ilwis::ThematicDomain>()->removeItem(QString::fromStdString(nme));
    }
        break;
    case itINDEXEDITEM:{
        this->ptr()->as<Ilwis::IndexedIdDomain>()->removeItem(QString::fromStdString(nme));
    }
        break;
    case itNAMEDITEM:{
        this->ptr()->as<Ilwis::NamedIdDomain>()->removeItem(QString::fromStdString(nme));
    }
        break;
    default:
        break;
    }
}

void ItemDomain::addItem(PyObject* item)
{ 
    IlwisTypes vt = valueType();

    switch (vt) {
    case itNUMERICITEM:{
        if (CppTupleElementCount(item) == 3 || CppTupleElementCount(item) == 4){
            QString label = QString::fromStdString(CppTupleElement2String(item,0));
            if ( label == sUNDEF)
                return;

            double rmin = CppTupleElement2Double(item, 1);
            double rmax = CppTupleElement2Double(item,2);
            if ( rmin == rUNDEF || rmax == rUNDEF)
                return;
            Ilwis::Interval *numitem;
            if ( CppTupleElementCount(item) == 4){
                double resolution = CppTupleElement2Double(item,3);
                if ( resolution == rUNDEF)
                    return;
                numitem = new Ilwis::Interval(label, { rmin, rmax,resolution});
            }else
                numitem = new Ilwis::Interval(label, { rmin, rmax});

            this->ptr()->as<Ilwis::NamedIdDomain>()->addItem(numitem);
        }
    }
        break;
    case itTHEMATICITEM:{
        if ( CppTupleElementCount(item) > 0 && CppTupleElementCount(item) <=3 ) {
            QString description = sUNDEF, code = sUNDEF;
            QString label = QString::fromStdString(CppTupleElement2String(item,0));
            if ( label == sUNDEF)
                return;
            if ( CppTupleElementCount(item) >= 2){
                code = QString::fromStdString(CppTupleElement2String(item,1));
            }
            if ( CppTupleElementCount(item) == 3){
                description = QString::fromStdString(CppTupleElement2String(item,2));
            }
            Ilwis::ThematicItem *titem = new Ilwis::ThematicItem({label, code, description}, count());

            this->ptr()->as<Ilwis::ThematicDomain>()->addItem(titem);
        }
    }
        break;
    case itINDEXEDITEM:{
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
        this->ptr()->as<Ilwis::IndexedIdDomain>()->addItem(ident);
    }
        break;
    case itNAMEDITEM:{
        QString name = QString::fromStdString(CppString2stdString(item));
        Ilwis::NamedIdentifier *id = new Ilwis::NamedIdentifier(name);
        this->ptr()->as<Ilwis::NamedIdDomain>()->addItem(id);
    }
        break;
//    case itPALETTECOLOR:{
//        if ( CppTupleElementCount(item) > 0 && CppTupleElementCount(item) <=2 ){
//            ColorPalette helper = ColorPalette();
//            Color col = Color(CppTupleElement2String(item,0), PyTupleGetItem(item,1));
//            QColor qCol = helper.colorToQColor(col);
//            Ilwis::ColorItem* colitem = new Ilwis::ColorItem(qCol);
//            this->ptr()->as<Ilwis::ItemDomain>()->add(colitem);
//        }
//    }
    default:
        break;
    }
}

PyObject *ItemDomain::item(int index, bool labelOnly)
{
    Ilwis::SPDomainItem domitem = this->ptr()->as<Ilwis::IntervalDomain>()->item(index);
    IlwisTypes vt = valueType();
    switch (vt) {
    case itNUMERICITEM:{
        Ilwis::Interval *numitem = domitem->as<Ilwis::Interval>();
        PyObject* list;
        if ( labelOnly)
            list = newPyTuple(1);
        else
            list = newPyTuple(4);
        setTupleItem(list, 0, PyUnicodeFromString(numitem->name().toUtf8()));
        if (!labelOnly){
            setTupleItem(list, 1, PyFloatFromDouble(numitem->range().min()));
            setTupleItem(list, 2, PyFloatFromDouble(numitem->range().max()));
            setTupleItem(list, 3, PyFloatFromDouble(numitem->range().resolution()));
        }
        return list;
    }
        break;
    case itTHEMATICITEM:{
        Ilwis::ThematicItem *titem = domitem->as<Ilwis::ThematicItem>();
        PyObject* list;
        if ( labelOnly)
            list = newPyTuple(1);
        else
            list = newPyTuple(3);
        setTupleItem(list, 0, PyUnicodeFromString(titem->name().toUtf8()));
        if(!labelOnly){
            setTupleItem(list, 1, PyUnicodeFromString(titem->code().toUtf8()));
            setTupleItem(list, 2, PyUnicodeFromString(titem->description().toUtf8()));
        }
        return list;
    }
        break;
    case itINDEXEDITEM:{
        Ilwis::IndexedIdentifier *indexed = domitem->as<Ilwis::IndexedIdentifier>();
        return PyBuildString(indexed->name().toStdString());
    }
        break;
    case itNAMEDITEM:{
        Ilwis::NamedIdentifier *named = domitem->as<Ilwis::NamedIdentifier>();
        return PyBuildString(named->name().toStdString());
    }
        break;
//    case itPALETTECOLOR:{
//        Ilwis::ColorPalette* colPal = new Ilwis::ColorPalette();
//        colPal->add(domitem);
//        PyObject* list = newPyTuple(1);
//        setTupleItem(list, 0, PyUnicodeFromString(colPal->toString().toLatin1()));
//        return list;
//    }
//        break;
    default:
        break;
    }
    return 0;
}

//-----------------------------------------------------------------------------------------

ColorDomain::ColorDomain()
{
    Ilwis::IColorDomain coldom;
    coldom.prepare();
    if (coldom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(coldom));
}

ColorDomain::ColorDomain(const std::string &resource)
{
    QString input (QString::fromStdString(resource));
    input.replace('\\','/');
    // if it is file:// (or http:// etc) leave it untouched; if not, append file:// and the working catalog path if it is missing
    if (input.indexOf("://") < 0) {
        int pos = input.indexOf('/');
        if (pos > 0) {
            if ((input.count('/') > 1) || QFileInfo(input).exists()) // full path starting with drive-letter (MS-DOS-style)
                input = "file:///" + input;
            else // container object without path, e.g. myfile.hdf/subdataset: look for it in workingCatalog()
                input = "file:///" + Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + input;
        }  else if (pos == 0) // full path starting with path-separator (UNIX-style)
            input = "file://" + input;
        else { // pos < 0: file without path, or new object
            QString file = Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + input;
            if (QFileInfo (file).exists()) // file without path
                input = "file:///" + file;
        }
    }
    Ilwis::IColorDomain coldom(input, itCOLORDOMAIN);
    if (coldom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(coldom));
}

IlwisTypes ColorDomain::ilwisType() const
{
    return itCOLORDOMAIN;
}

std::string ColorDomain::containsColor(const Color &value) const{
    ColorRange helper = ColorRange();
    QColor ilwCol = helper.colorToQColor(value);
    Domain::Containement dc = (Domain::Containement)this->ptr()->as<Ilwis::ColorDomain>()->contains(ilwCol);
    switch(dc){
    case 1: return "cSELF";
                break;
    case 2: return "cPARENT";
                break;
    case 3: return "cDECLARED";
                break;
    case 0: return "cNONE";
        break;
    default: return "Not found anywhere";
    }
}

//--------------------------------------------------------------------------------------------

TextDomain::TextDomain()
{
    Ilwis::ITextDomain txtdom;
    txtdom.prepare();
    if (txtdom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(txtdom));
}

TextDomain::TextDomain(const std::string &resource)
{
    QString input (QString::fromStdString(resource));
    input.replace('\\','/');
    // if it is file:// (or http:// etc) leave it untouched; if not, append file:// and the working catalog path if it is missing
    if (input.indexOf("://") < 0) {
        int pos = input.indexOf('/');
        if (pos > 0) {
            if ((input.count('/') > 1) || QFileInfo(input).exists()) // full path starting with drive-letter (MS-DOS-style)
                input = "file:///" + input;
            else // container object without path, e.g. myfile.hdf/subdataset: look for it in workingCatalog()
                input = "file:///" + Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + input;
        }  else if (pos == 0) // full path starting with path-separator (UNIX-style)
            input = "file://" + input;
        else { // pos < 0: file without path, or new object
            QString file = Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + input;
            if (QFileInfo (file).exists()) // file without path
                input = "file:///" + file;
        }
    }
    Ilwis::ITextDomain texdom(input, itTEXTDOMAIN);
    if (texdom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(texdom));

}


//---------------------------------------------------------------------------------------------
TimeDomain::TimeDomain()
{
    Ilwis::ITimeDomain timedom;
    timedom.prepare();
    if (timedom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(timedom));
}

TimeDomain::TimeDomain(const std::string& resource)
{
    QString input (QString::fromStdString(resource));
    input.replace('\\','/');
    // if it is file:// (or http:// etc) leave it untouched; if not, append file:// and the working catalog path if it is missing
    if (input.indexOf("://") < 0) {
        int pos = input.indexOf('/');
        if (pos > 0) {
            if ((input.count('/') > 1) || QFileInfo(input).exists()) // full path starting with drive-letter (MS-DOS-style)
                input = "file:///" + input;
            else // container object without path, e.g. myfile.hdf/subdataset: look for it in workingCatalog()
                input = "file:///" + Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + input;
        }  else if (pos == 0) // full path starting with path-separator (UNIX-style)
            input = "file://" + input;
        else { // pos < 0: file without path, or new object
            QString file = Ilwis::context()->workingCatalog()->filesystemLocation().toLocalFile() + '/' + input;
            if (QFileInfo (file).exists()) // file without path
                input = "file:///" + file;
        }
    }
    Ilwis::ITimeDomain timedom(input, itNUMERICDOMAIN);
    if (timedom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(timedom));
}

TimeDomain::TimeDomain(const Range& rng)
{
    Ilwis::ITimeDomain timedom;
    timedom.prepare();
    if (timedom.isValid())
        this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(timedom));
    this->setRange(rng);
}
