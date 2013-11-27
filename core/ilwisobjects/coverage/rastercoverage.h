#ifndef RASTERLAYER_H
#define RASTERLAYER_H

#include "Kernel_global.h"

namespace Ilwis {

class Resource;
class Grid;
/*!
 * \brief The RasterCoverage class
 *
 * In Ilwis-Objects remote sensing imagery is accessed through raster-coverages. In this context a raster-coverage is a stack of 2D rasters (1..n)
 * in which all layers of the stack have the same raster size and pixel size. The z component is referred to as the ‘layer-index’ or ‘index’.  A layer
 * is always rectangular as are its pixels. As a raster coverage relation between its pixels and the real world is covered by the georeference class.
 * It translates pixel positions to real world coordinates and vice-versa.
 *
 * In a RasterCoverage we use 2 kinds of attribute tables: Index-tables and Coverage-tables.
 * A Coverage-table has object data in its initial position, the Index table has records at all other positions.
 *
 * In order to get a rastervalue you need to use either coord2value() or pix2value(), where coord2value simply translates the coordinates to pixels
 * and calls pix2value with those. The returned value is always a double or undefined. undefined values only happen when you query outside the raster,
 * or when the raster is not fully initialized.
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
     * The constructor for a RasterCoverage with a certain Resource.<br>
     * This resource must have a name and an Id.<br>
     * Code and description are optional.
     *
     * \sa IlwisObject
     * \sa Resource
     * \param resource The resource to use
     */
    RasterCoverage(const Resource& resource);

    ~RasterCoverage();

    //@override
    IlwisTypes ilwisType() const;

    /*!
     * Copies (clones) this rastercoverage to a new raster with a new unique id.<br>
     * Also copies all of the binary data.
     *
     * \return a copy of this rastercoverage
     */
    virtual RasterCoverage *clone() ;

    //@override
    const DataDefinition& datadef() const;

    //@override
    DataDefinition& datadef();

    /*!
     * Returns a reference to the IGeoReference of this RasterCoverage.<br>
     * Can be null if it is not set.
     *
     * \return the georeference
     */
    const Ilwis::IGeoReference &georeference() const;

    /*!
     * \brief Replaces the existing IGeoReference with a new one.
     *
     * Replaces the old IGeoReference with the new one, if the georeference is not null.<br>
     * It also resets the binary data because the new georeference can have a different size, making the original data invalid.<br>
     * If the georeference is valid it computes and sets the new coordinatesystem and the new size.
     *
     * \param grf the new georeference
     */
    void georeference(const IGeoReference& grf) ;

    /*!
     * Returns the size of this RasterCoverage<br>
     *
     * A size can, depending on the raster either be 2D or 3D.<br>
     * Size in row-col for 2D.<br>
     * And in row-col-heigth for 3D.
     *
     * \sa Size
     * \return the size of this RasterCoverage
     */
    Size size() const;

    /*!
     * Assigns a new size to this RasterCoverage.<br>
     * Size in row-col for 2D.<br>
     * And in row-col-heigth for 3D.<br>
     * The Size cannot be changed if this rastercoverage is readonly.<br>
     * Also changes the size of the georeference.
     *
     * \sa Size
     * \param sz the new size, must always be positive or undefined
     */
    void size(const Size& sz);

    /*!
     * \brief copyBinary Copies the binary data of this RasterCoverage
     *
     * The data will be coppied on the supplied raster, the index decides the starting layer of the copy,<br>
     * and the size of the raster decides the endpoint. only works when both rasters have the same geometrie and domain<br>
     *
     * \param raster The target of the binary copy
     * \param index The starting point of the copy
     */
    void copyBinary(const IlwisData<RasterCoverage> &raster, int index);

    /*!
     * transforms a certain coordinate to relevant the value<br>
     * The coordinate must fit in the raster, and can either be 2D or 3D<br>
     * This function will use the georeference to transform the coordinates in pixels<br>
     * and use those pixels to call the pix2value()
     *
     * \sa pix2value()
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
     * transforms the value of a certain pixel in the grid to the relevant value<br>
     * The pixel point, be it 2D or 3D must fit in envelope of this coverage for this function to return an actual value,<br>
     * if the pixel does not fit in the rastersize, it will return undefined.<br>
     * if the grid is null, it will also return undefined<br>
     * This function requires a valid georeference and a non null grid, if this is not the case undefined will be returned<br>
     *<br>
     * this function is rather inefficient, see PixelIterator if you require efficiency (recommended if you want to retrieve multiple pixels)
     * \sa PixelIterator
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

    //@override
    Resource source(int mode=cmINPUT) const;

    /*!
     * unload the grid, generally used to clear some memory
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
