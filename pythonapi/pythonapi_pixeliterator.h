#ifndef PYTHONAPI_PIXELITERATOR_H
#define PYTHONAPI_PIXELITERATOR_H

typedef struct bufferinfo Py_buffer;

namespace Ilwis {
    class PixelIterator;
}

#include "pythonapi_qtGNUTypedefs.h"
#include "pythonapi_geometry.h"
#include "pythonapi_util.h"

namespace pythonapi {

#ifdef SWIG
%rename(Flow) FlowVal;
#endif

struct FlowVal{
    enum Value{ XYZ, YXZ, XZY, YZX, ZXY, ZYX};
};

typedef FlowVal::Value Flow;

class RasterCoverage;

class PixelIterator{

    friend class RasterCoverage;

    public:
        PixelIterator(const PixelIterator &pi);
        PixelIterator(RasterCoverage* rc, const Box &b = Box());
        PixelIterator(RasterCoverage *rc, const Geometry& geom);
        PixelIterator(Ilwis::PixelIterator* ilwIt);
        ~PixelIterator();
        /**
         * @brief next confusingly returns current value bevore iterating to the next item
         * brings together C++ style "std::iterator it != it.end()" with end() pointing the back (after last entry)
         * and Python style "for it in it:" using the __next__() also to retrieve first item.
         * @return
         */
        PixelIterator* __iter__();
        double __next__();
        bool __bool__() const;
        std::string __str__();
        /**
         * @brief __float__ returns value on current position
         * @return returns value on current position
         */
        double __float__();
        /**
         * @brief __int__ returns the current linear position of the iterator
         * @return returns the current linear position of the iterator
         */
        quint64 __int__();

        bool __contains__(const Pixel& vox);
        Box box();
        Pixel position();
        void setFlow(Flow flw);
        bool xChanged();
        bool yChanged();
        bool zChanged();

        PixelIterator* __set__(const PixelIterator& ohterIt);
        PixelIterator& __getitem__(const Pixel& vox);
        double __getitem__(quint32 linearPosition);
        void __setitem__(quint32 linearPosition, double value);
        PixelIterator operator+ (int n);
        PixelIterator __radd__(int n);
        PixelIterator* operator+= (int n);

        bool operator==(const PixelIterator& other);
        bool operator!=(const PixelIterator& other);
        bool operator<=(const PixelIterator& other);
        bool operator< (const PixelIterator& other);
        bool operator>=(const PixelIterator& other);
        bool operator> (const PixelIterator& other);

        PixelIterator end();
        Py_buffer* asBuffer();
        RasterCoverage* raster();

    protected:
        Ilwis::PixelIterator& ptr() const;
        RasterCoverage* _coverage;
        std::shared_ptr<Ilwis::PixelIterator> _ilwisPixelIterator;
        quint64 _endposition;
};

} // namespace pythonapi

#endif // PYTHONAPI_PIXELITERATOR_H
