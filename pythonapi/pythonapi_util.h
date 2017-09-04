#ifndef PYTHONAPI_UTIL_H
#define PYTHONAPI_UTIL_H

#include "kernel.h"
#include "pythonapi_extension.h"
#include "containerstatistics.h"
#include "pythonapi_pyobject.h"
#include <memory>

namespace geos {
namespace geom {
    class Envelope;
}
}

namespace Ilwis{
    template<class CrdType, bool useDouble> class Location;
    class Coordinate;
    template<class CrdType> class Box;
    template<typename T> class Size;
    class PixelIterator;
    class IOOptions;
   // template<typename DataType> class ContainerStatistics;
}

namespace pythonapi {
/**
 * since the implementation of this template classes are not included in the header file,
 * only the previously instantiated types (currently only Pixel, PixelD, Envelope, Box, Size and SizeD) can be used.
 */
    class Coordinate;
    template<class CrdType> class PixelTemplate{
        template<class IlwisType, class PyType, typename T> friend class BoxTemplate;
        template<typename T> friend class SizeTemplate;
        friend class PixelIterator;
        friend class GeoReference;
        friend class RasterCoverage;
        public:
            PixelTemplate(const PixelTemplate<qint32>& pixel);
            PixelTemplate(const PixelTemplate<double>& pixel);
            PixelTemplate(CrdType x, CrdType y);
            PixelTemplate(CrdType x, CrdType y, CrdType z);
            CrdType x() const;
            CrdType y() const;
            CrdType z() const;
            void setX(CrdType v);
            void setY(CrdType v);
            void setZ(CrdType v);
            bool is3D() const;

            PixelTemplate<CrdType>* operator *=(CrdType n);
            PixelTemplate<CrdType>* __itruediv__(CrdType n);
            bool operator==(const PixelTemplate<CrdType>& other);
            bool operator!=(const PixelTemplate<CrdType>& other);

            std::string __str__();
            bool __bool__() const;
        private:
            PixelTemplate(const Ilwis::Location<CrdType, false>& pixel);
            PixelTemplate(Ilwis::Location<CrdType, false>* pixel);
            Ilwis::Location<CrdType, false>& data() const;
            std::shared_ptr<Ilwis::Location<CrdType, false> > _data;
    };

    typedef PixelTemplate<qint32> Pixel;
    typedef PixelTemplate<double> PixelD;

//-----------------------------------------------------------------------------------------------------------------

    class Coordinate{
        template<class IlwisType, class PyType, typename T> friend class BoxTemplate;
        template<typename T> friend class SizeTemplate;
        friend class RasterCoverage;
        friend class GeoReference;
        friend class CoordinateSystem;
        friend class VertexIterator;
        public:
            Coordinate(double x, double y);
            Coordinate(double x, double y, double z);
            Coordinate(const Coordinate& crd);
            double x() const;
            double y() const;
            double z() const;
            void setX(double v);
            void setY(double v);
            void setZ(double v);
            bool is3D() const;

            Coordinate* operator*=(double n);
            Coordinate* __itruediv__(double n);
            bool operator==(const Coordinate& other);
            bool operator!=(const Coordinate& other);

            std::string __str__();
            bool __bool__() const;
        private:
            Coordinate(const Ilwis::Coordinate& coordinate);
            Coordinate(Ilwis::Coordinate* coordinate);
            Ilwis::Coordinate& data() const;
            std::shared_ptr<Ilwis::Coordinate> _data;
    };

//-----------------------------------------------------------------------------------------------------------------

    template<typename T> class SizeTemplate{
        template<class IlwisType, class PyType, typename U> friend class BoxTemplate;
        friend class RasterCoverage;
        friend class GeoReference;
        public:
            SizeTemplate(T xSizeT, T ySizeT, T zSizeT=1);
            SizeTemplate(const SizeTemplate<quint32>& size);
            SizeTemplate(const SizeTemplate<double>& size);

            T xsize() const;
            T ysize() const;
            T zsize() const;
            void setXsize(T x);
            void setYsize(T y);
            void setZsize(T z);
            quint64 linearSize() const;

            SizeTemplate<T>* operator+=(const SizeTemplate<T>& sz);
            SizeTemplate<T>* operator-=(const SizeTemplate<T>& sz);
            SizeTemplate<T>* operator*=(double f);
            bool operator==(const SizeTemplate<T>& sz);
            bool operator!=(const SizeTemplate<T>& sz);

            bool __contains__(const Coordinate& pix) const;
            bool __contains__(const PixelTemplate<qint32>& pix) const;
            bool __contains__(const PixelTemplate<double>& pix) const;

            std::string __str__();
            bool __bool__() const;
        private:
            SizeTemplate(const Ilwis::Size<T>& size);
            Ilwis::Size<T>& data() const;
            std::shared_ptr<Ilwis::Size<T> > _data;
    };
    typedef SizeTemplate<quint32> Size;
    typedef SizeTemplate<double> SizeD;

//-----------------------------------------------------------------------------------------------------------------

    template<class IlwisType, class PyType, typename SizeType> class BoxTemplate{
        friend class PixelIterator;
        friend class GeoReference;
        friend class CoordinateSystem;
        friend class Geometry;
        friend class RasterCoverage;
        friend class Coverage;
        public:
            BoxTemplate();
            BoxTemplate(const std::string &envelope);
            BoxTemplate(const PyType& min,const PyType& max);
            BoxTemplate(const SizeTemplate<SizeType>& size);

            PyType minCorner();
            PyType maxCorner();
            SizeTemplate<SizeType> size();

            bool is3D() const;
            bool __contains__(const PyType& point) const;
            bool __contains__(const BoxTemplate<IlwisType, PyType, SizeType>& box) const;
            bool operator==(const BoxTemplate<IlwisType, PyType, SizeType>& other);
            bool operator!=(const BoxTemplate<IlwisType, PyType, SizeType>& other);

            std::string __str__();
            bool __bool__() const;
        private:
            BoxTemplate(const Ilwis::Box<IlwisType>& box);
            BoxTemplate(const geos::geom::Envelope* envelope);
            Ilwis::Box<IlwisType>& data() const;
            std::shared_ptr<Ilwis::Box<IlwisType> > _data;
    };
    typedef BoxTemplate<Ilwis::Coordinate, Coordinate, double> Envelope;
    typedef BoxTemplate<Ilwis::Location<qint32, false>, Pixel, quint32 > Box;

//-----------------------------------------------------------------------------------------------------------------

    #ifdef SWIG
    %rename(PropertySets) Properties;
    #endif

    struct Properties{
        enum Values{pNONE = 0, pBASIC=1, pMIN=2, pMAX=4, pDISTANCE=8, pDELTA=16,pNETTOCOUNT=32, pCOUNT=64, pSUM=128,
                          pMEAN=256, pMEDIAN=512, pPREDOMINANT=1024, pSTDEV=2048, pHISTOGRAM=4096, pLAST=8192, pALL=4294967296};

    };

    typedef Properties::Values PropertySets;


    template<typename DataType> class ContainerStatistics{
    public:
        ContainerStatistics(): _data(new Ilwis::ContainerStatistics<DataType>()){
        }

        ContainerStatistics(const Ilwis::ContainerStatistics<DataType>& conStat)
            : _data(new Ilwis::ContainerStatistics<DataType>(conStat)){
        }

        quint16 significantDigits() const{
            return this->data().significantDigits();
        }

        void findSignificantDigits(double distance){
            //this->data().findSignificantDigits(distance);
        }

        void binCount(quint32 value){
            this->data().binCount(value);
        }

        bool __bool__() const{
            return this->data().isValid();
        }

        double stretchLinear(double input, int stretchRange) const{
            return this->data().stretchLinear(input, stretchRange);
        }

        PyObject* stretchLimits(double percent) const{
            PyObject* pyTup = newPyTuple(2);
            std::pair<double, double> cpair = this->data().stretchLimits(percent);
            setTupleItem(pyTup, 0, PyFloatFromDouble(std::get<0>(cpair)));
            setTupleItem(pyTup, 1, PyFloatFromDouble(std::get<1>(cpair)));
            return pyTup;
        }

        double __getitem__(PropertySets pyMethod){
            typename Ilwis::ContainerStatistics<DataType>::PropertySets ilwMethod;
            ilwMethod = static_cast<typename Ilwis::ContainerStatistics<DataType>::PropertySets>(pyMethod);
            return this->data().operator [](ilwMethod);
        }

        double prop(PropertySets pyMethod){
            return this->__getitem__(pyMethod);
        }

        PyObject* histogram(){
            std::vector<HistogramBin> hisVec = this->data().histogram();
            PyObject* pyList = newPyList(hisVec.size());
            for(int i = 0; i < hisVec.size(); i++){
                PyObject* histoTup = newPyTuple(2);
                HistogramBin histo = hisVec[i];

                setTupleItem(histoTup, 0, PyFloatFromDouble((double)histo._limit));
                setTupleItem(histoTup, 1, PyFloatFromDouble((double)histo._count));

                setListItem(pyList, i, histoTup);
            }
            return pyList;
        }

       //not yet implemented in ilwisobjects.i
       template<typename IterType> bool calculate(IterType& begin, IterType& end,
                                                   PropertySets mode = PropertySets::pBASIC){

            typename Ilwis::ContainerStatistics<DataType>::PropertySets ilwMethod;
            ilwMethod = static_cast<typename Ilwis::ContainerStatistics<DataType>::PropertySets>(mode);

            return this->data.calculate(begin, end, ilwMethod);
        }


    protected:
        Ilwis::ContainerStatistics<DataType>& data() const{
            return (*this->_data);
        }
        std::shared_ptr<Ilwis::ContainerStatistics<DataType> > _data;
        typedef typename Ilwis::ContainerStatistics<DataType>::HistogramBin HistogramBin;
    };

    typedef ContainerStatistics< double > NumericStatistics;

//-----------------------------------------------------------------------------------------------------------------
    class IOOptions{
        friend class IlwisObject;
        friend class Table;
    public:
        IOOptions();
        IOOptions(const std::string& key, PyObject* value);

        bool contains(const std::string& option);
        quint32 size();
        PyObject* __getitem__(const std::string& option);
        IOOptions& addOption(const std::string& key, PyObject* value);

    protected:
        std::shared_ptr<Ilwis::IOOptions> _data;
        Ilwis::IOOptions& ptr() const;
        IOOptions(Ilwis::IOOptions* ilwIOOp);
    };

//------------------------------------------------------------------------------------------------------------------
#ifdef SWIG
    %rename(ColorModel) ColorModelNS;
#endif

    struct ColorModelNS{
        enum Value{cmNONE, cmRGBA, cmHSLA, cmCYMKA, cmGREYSCALE};
    };

    typedef ColorModelNS::Value ColorModel;

    class Color{
    public:
        Color();
        Color(ColorModel type, PyObject* obj, const std::string& name = "");
        Color(const std::string& typeStr, PyObject* obj, const std::string& name = "");
        double getItem(std::string key) const;

        void setName(const std::string& name);
        std::string getName();

        ColorModel getColorModel() const;
        std::string toString() const;
        std::string __str__();
    private:
        void readColor(ColorModel type, PyObject* obj);
        ColorModel stringToModel(const std::string& type);
        ColorModel _type = ColorModel::cmRGBA;
        PyObject* _colorVal;
        std::string _name = "";
    };
} // namespace pythonapi

#endif // PYTHONAPI_UTIL_H
