#ifndef RASTERLAYER_H
#define RASTERLAYER_H

#include "Kernel_global.h"

namespace Ilwis {

class Resource;
class Grid;
/*!
 * \brief The RasterCoverage class The RasterCoverage class
 *
 * In Ilwis-Objects remote sensing imagery is accessed through raster-coverages. In this context a raster-coverage is a stack of 2D rasters (1..n)
 * in which all layers of the stack have the same raster size and pixel size. The z component is referred to as the ‘layer-index’ or ‘index’.  A layer
 * is always rectangular as are its pixels. As a raster coverage relation between its pixels and the real world is covered by the georeference class.
 * It translates pixel positions to real world coordinates and vice-versa.
 *
 * In a RasterCoverage we use 2 kinds of attribute tables: Index-tables and Coverage-tables.
 * A Coverage-table has object data in its initial position, the Index table has records at all other positions.
 */
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
     * The constructor for a RasterCoverage with a certain Resource
     *
     * \sa IlwisObject
     * \param resource The resource to use
     */
    RasterCoverage(const Resource& resource);

    ~RasterCoverage();

    /*!
     * Returns the IlwisType of this object
     *
     * \return the IlwisType of this object
     */
    IlwisTypes ilwisType() const;

    /*!
     * copies (clones) this rastercoverage
     * \return a copy of this rastercoverage
     */
    virtual RasterCoverage *copy() ;

    /*!
     * \brief Returns the DataDefinition of this rastercoverage
     *
     * \return the datadefinition of this rastercoverage
     */
    const DataDefinition& datadef() const;


    /*!
     * \brief Returns the DataDefinition of this rastercoverage
     *
     * \return the datadefinition of this rastercoverage
     */
    DataDefinition& datadef();

    /*!
     * Returns the IGeoReference of this RasterCoverage
     *
     * \return the georeference
     */
    const Ilwis::IGeoReference &georeference() const;

    /*!
     * \brief Replaces the existing IGeoReference with a new one
     *
     * replaces the old IGeoReference with the new one, if the georeference is not null
     * it also resets the binary data.
     * if the georeference is valid it computes and sets the new coordinatesystem and the new size
     *
     * \param grf the new georeference
     */
    void georeference(const IGeoReference& grf) ;

    /*!
     * Returns the size of this RasterCoverage
     *
     * \return the size of this RasterCoverage
     */
    Size size() const;

    /*!
     * assigns a new size to this RasterCoverage
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
     * Gives the value of a certain pixel in the grid
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
     * Gives the Resource used for this coverage
     *
     * \return the resource used for this coverage
     */
    Resource source(int mode=cmINPUT) const;

    /*!
     * unload the grid generally used to clear some memory
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
