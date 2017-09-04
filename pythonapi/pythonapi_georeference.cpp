#include "../../core/kernel.h"
#include "../../core/ilwisobjects/ilwisobject.h"
#include "../../core/ilwisobjects/ilwisdata.h"

#include "../../core/util/geometries.h"
#include "../../core/ilwiscontext.h"
#include "../../core/catalog/catalog.h"
#include "../../core/ilwisobjects/geometry/coordinatesystem/coordinatesystem.h"
#include "../../core/ilwisobjects/geometry/georeference/georeference.h"

#include "pythonapi_georeference.h"
#include "pythonapi_error.h"

namespace pythonapi {

    GeoReference::GeoReference(const Ilwis::IGeoReference& gr): IlwisObject(new Ilwis::IIlwisObject(gr)){
    }

    GeoReference::GeoReference(const std::string& resource){
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
        Ilwis::IGeoReference gr(input, itGEOREF);
        if (gr.isValid())
            this->_ilwisObject = std::shared_ptr<Ilwis::IIlwisObject>(new Ilwis::IIlwisObject(gr));
    }

    GeoReference* GeoReference::toGeoReference(Object *obj){
        GeoReference* ptr = dynamic_cast<GeoReference*>(obj);
        if(!ptr)
            throw InvalidObject("cast to GeoReference not possible");
        return ptr;
    }

    CoordinateSystem GeoReference::coordinateSystem() const{
        return CoordinateSystem(Ilwis::ICoordinateSystem(this->ptr()->as<Ilwis::GeoReference>()->coordinateSystem()));
    }

    void GeoReference::setCoordinateSystem(const CoordinateSystem &csy){
        this->ptr()->as<Ilwis::GeoReference>()->coordinateSystem(csy.ptr()->as<Ilwis::CoordinateSystem>());
    }

    Coordinate GeoReference::pixel2Coord(const PixelD& pixel) const{
        return Coordinate(this->ptr()->as<Ilwis::GeoReference>()->pixel2Coord(pixel.data()));
    }

    Coordinate GeoReference::pixel2Coord(const Pixel& pixel) const{
        return Coordinate(this->ptr()->as<Ilwis::GeoReference>()->pixel2Coord(pixel.data()));
    }

    PixelD GeoReference::coord2Pixel(const Coordinate& coord) const{
        return PixelD(this->ptr()->as<Ilwis::GeoReference>()->coord2Pixel(coord.data()));
    }

    Envelope GeoReference::box2Envelope(const Box& box) const{
        return Envelope(this->ptr()->as<Ilwis::GeoReference>()->pixel2Coord(box.data()));
    }

    Box GeoReference::envelope2Box(const Envelope& box) const{
        return Box(this->ptr()->as<Ilwis::GeoReference>()->coord2Pixel(box.data()));
    }

    double GeoReference::pixelSize() const{
        return this->ptr()->as<Ilwis::GeoReference>()->pixelSize();
    }

    Size GeoReference::size() const{
        return Size(this->ptr()->as<Ilwis::GeoReference>()->size());
    }

    void GeoReference::setSize(const Size& sz){
        this->ptr()->as<Ilwis::GeoReference>()->size(sz.data());
    }

    bool GeoReference::centerOfPixel() const{
        return this->ptr()->as<Ilwis::GeoReference>()->centerOfPixel();
    }

    void GeoReference::setCenterOfPixel(bool yesno){
        this->ptr()->as<Ilwis::GeoReference>()->centerOfPixel(yesno);
    }

    bool GeoReference::isCompatible(const GeoReference& other) const{
        return this->ptr()->as<Ilwis::GeoReference>()->isCompatible(other.ptr()->as<Ilwis::GeoReference>());
    }

    bool GeoReference::compute(){
        return this->ptr()->as<Ilwis::GeoReference>()->compute();
    }

    IlwisTypes GeoReference::ilwisType() const{
        return itGEOREF;
    }

    const QString GeoReference::getStoreFormat() const {
        return "georef";
    }

} // namespace pythonapi
