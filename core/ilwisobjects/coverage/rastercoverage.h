#ifndef RASTERLAYER_H
#define RASTERLAYER_H

#include "Kernel_global.h"

namespace Ilwis {

class Resource;
class Grid;

class KERNELSHARED_EXPORT RasterCoverage : public Coverage
{
public:

    friend class PixelIterator;
    friend class GridBlock;
    friend class Grid;
    friend class RasterInterpolator;

    /*!
     * The constructor for an empty RasterCoverage
     */
    RasterCoverage();

    /*!
     * The constructor for a RasterCoverage with a certain resource
     *
     * \sa IlwisObject(const Resource &source)
     * \param resource The resource to use
     */
    RasterCoverage(const Resource& resource);

    ~RasterCoverage();

    /*!
     * \brief ilwisType Returns the IlwisType of this object
     *
     * \return the IlwisType of this object
     */
    IlwisTypes ilwisType() const;

    /*!
     * \brief copy copies (clones) this rastercoverage
     * \return a copy of this rastercoverage
     */
    virtual RasterCoverage *copy() ;

    /*!
     * \brief datadef Returns the datadef of this rastercoverage
     *
     * The datadef when is a range on the domain
     * \sa DataDefenition
     * \return the datadefinition of this rastercoverage
     */
    const DataDefinition& datadef() const;


    /*!
     * \brief datadef Returns the datadef of this rastercoverage
     *
     * The datadef when is a range on the domain
     * \sa DataDefenition
     * \return the datadefinition of this rastercoverage
     */
    DataDefinition& datadef();

    /*!
     * \brief georeference returns the georeference of this RasterCoverage
     * \return the georeference
     */
    const Ilwis::IGeoReference &georeference() const;

    /*!
     * \brief georeference Replaces the existing georeference with a new one
     *
     * replaces the old georeference with the new one, if the georeference is not null
     * it also resets the binary data.
     * if the georeference is valid it computes and sets the new coordinatesystem and the new size
     *
     * \param grf the new georeference
     */
    void georeference(const IGeoReference& grf) ;

    /*!
     * Returns the size of this rastercoverage, if the size is unknown of invalid it looks up the correct size and returns this instead
     * size in row-col
     * \return the size of this rastercoverage
     */
    Size size() const;

    /*!
     * assigns a new size to this rastercoverage
     * size in row-col
     * \param sz the new size, must always be positive or undefined
     */
    void size(const Size& sz);

    /*!
     * \brief copyBinary Copies the binary data of this RasterCoverage
     *
     * The data will be coppied on the supplied raster, the index decides the starting point of the copy,
     * and the size of the raster decides the endpoint
     *
     * \param raster The target of the binary copy
     * \param index The starting point of the copy
     */
    void copyBinary(const IlwisData<RasterCoverage> &raster, int index);

    /*!
     * Gives the value at a certain coordinate
     * \param c the coordinate
     * \return the value at the coordinates or undefined
     */
    double coord2value(const Coordinate &c){
        if ( _georef->isValid() && c.isValid()) {
            Point2D<double> pix = _georef->coord2Pixel(c);
            return pix2value(pix);
        }
        return rUNDEF;
    }

    /*!
     * Gives the value of a certain pixel
     * \param pix the pixel
     * \return the value at the pixel or undefined
     */
    double pix2value(const Point3D<double>& pix){
        if ( _georef->isValid() && !connector().isNull()) {
            if ( _grid.isNull()) {
                _grid.reset(connector()->loadGridData(this));
                if (_grid.isNull())
                    return rUNDEF;
            }

            double v = _grid->value(pix);
            return datadef().range()->ensure(v);
        }
        return rUNDEF;
    }

    /*!
     * Gives the resource for this coverage
     *
     * \return the resource for this coverage
     */
    Resource source(int mode=cmINPUT) const;

    /*!
     * unload the grid
     */
    void unloadBinary();

protected:
    Grid *grid();
    QScopedPointer<Grid> _grid;
    void copyTo(IlwisObject *obj);

private:
    DataDefinition _datadefCoverage;
    IGeoReference _georef;
    Size _size;
    std::mutex _mutex;
};

typedef IlwisData<RasterCoverage> IRasterCoverage;
}

Q_DECLARE_METATYPE(Ilwis::IRasterCoverage)


#endif // RASTERLAYER_H
