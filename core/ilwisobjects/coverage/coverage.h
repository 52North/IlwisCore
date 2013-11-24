#ifndef BASELAYER_H
#define BASELAYER_H

#include "Kernel_global.h"
#include "geometries.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "coordinatesystem.h"
#include "datadefinition.h"
#include "connectorinterface.h"
#include "containerstatistics.h"
#include "commandhandler.h"

namespace Ilwis {

class Table;
class ItemRange;

typedef IlwisData<Table> ITable;
typedef ITable AttributeTable;
/*!
 * \brief The Coverage class
 *
 * In Ilwis we have two kinds of coverages. Raster coverages for imagery and feature coverages for vector data.
 * A coverage controls the validity and meaning of data. A resource can be mapped to a coverage, and by this way be accessed
 */
class KERNELSHARED_EXPORT Coverage : public IlwisObject
{

public:
    /*!
     * An AttributeTable can have 2 types depending on the object, it can be a coveragetable,
     * in which the features are defined at only 1 index, or it can be a indextable,
     * in which the features are defined at all known indexes.
     * the indexes can for example describe different measurements at different times
     */
    enum AttributeType{atCOVERAGE, atINDEX};

    /*!
     * The constructor for an empty coverage
     */
    Coverage();

    /*!
     * The constructor for an coverage based on a Resource
     *
     * \sa IlwisObject
     * \param source Resource that has to be used
     */
    Coverage(const Resource& source);

    ~Coverage();

    /*!
     * Query for the coordinate system of this coverage
     *
     * \sa ICoordinateSystem
     * \return the coordinate system if it has one or else null
     */
    ICoordinateSystem coordinateSystem() const;

    /*!
     * Changes the coordinate system of this coverage into the one specified
     *
     * \sa ICoordinateSystem
     * \param the new coordinate system
     */
    void setCoordinateSystem(const ICoordinateSystem& csy);

    /*!
     * Query for the envelope of this coverage, must fit in the coordinate system
     * a envelope decides what domain within a coordinate system is valid, or in this case,
     * which part of the coordinate system is covered by this coverage
     *
     * \return the envelope of this coverage if it has one or else null
     */
    Box3D<double> envelope() const;

    /*!
     * Changes the envelope of this coverage to the one specified
     * a envelope decides what domain within a coordinate system is valid, or in this case,
     * which part of the coordinate system is covered by this coverage
     *
     * \param the new envelope
     */
    void envelope(const Box3D<double> &bnds);

    /*!
     *
     * Gives the Coveragetable when called without parameter, and gives the indextable if you use atINDEX as parameter
     * atCOVERAGE as parameter also gives the attributeable
     *
     * \param attType the type of table required (atCOVERAGE or atINDEX)
     * \return an AttributeTable
     */
    AttributeTable attributeTable(AttributeType attType=atCOVERAGE) const ;
    AttributeTable &attributeTable(AttributeType attType=atCOVERAGE);
    bool hasAttributes(AttributeType attType=atCOVERAGE) const;

    /*!
     * \brief Sets a new Coveragetable or Indextable
     *
     * When called without parameter, or with atCOVERAGE as parameter you will replace the Coveragetable
     * When called with atINDEX as parameter it will replace the Indextable
     *
     * \param tbl the new table
     * \param attType the type of the new table
     */

    void attributeTable(const ITable& tbl, AttributeType attType=atCOVERAGE );

    /*!
     * Query for the NumbericStatistics of this coverage
     * \return the statistics of this coverage
     */
    NumericStatistics& statistics();

    /*!
     * Returns the DataDefinition of this rastercoverage
     * can be null if it is not set
     *
     * \return the datadefinition of this rastercoverage
     */
    const DataDefinition& datadefIndex() const;

    /*!
     * Returns the DataDefinition of this rastercoverage
     * can be null if it is not set
     *
     * \return the datadefinition of this rastercoverage
     */
    DataDefinition& datadefIndex();

    /*!
     * \brief Returns a value in the Coverage- or Index-table
     *
     * Using the the colname and the itemid this function will return the correspondending value from the Coveragetable
     * if you ad a index, the value will be retrieved from the Indextable instead.
     *
     * \param colName name of the required collumn
     * \param itemid id of the item in the collumn
     * \param layerIndex the index of the Indextable, default =-1 = Coveragetable
     * \return the requested value
     */
    QVariant value(const QString& colName, quint32 itemid, qint32 layerIndex = -1);

    //@override
    Resource source(int mode=cmINPUT) const;

    /*!
     * Translates a value to a index from the Indextable
     *
     * note : the returned index is not necessarily an entry in the index table, as it is possible to get doubles or indices not in the table back
     * this happens for interpollation purposes and such
     *
     * \param value the value that must be translated to a index
     * \return the index of the value
     */
    double layerIndex(const QString& value);

    /*!
     * Sets the supplied items at the indexes
     *
     * Uses the order specified in the itemrange itself
     * \sa ItemRange
     * \param items the items that have to be added
     */
    void setLayerIndexes(const ItemRange &items);

    /**
     * Changes the name of this coverage
     *
     * @param nam the new name of this coverage
     */
    void setName(const QString &nam);

protected:
    void copyTo(IlwisObject *obj) ;
private:

    ICoordinateSystem _coordinateSystem;
    Box3D<double> _envelope;
    AttributeTable _attTable;
    AttributeTable _attTableIndex;
    NumericStatistics _statistics;
    DataDefinition _indexdefinition;
    std::vector<quint32> _indexValues;

};

typedef IlwisData<Coverage> ICoverage;


}





#endif // BASELAYER_H
