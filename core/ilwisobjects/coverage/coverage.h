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
 */
class KERNELSHARED_EXPORT Coverage : public IlwisObject
{

public:

    enum AttributeType{atCOVERAGE, atINDEX};

    /*!
     * The constructor for an empty coverage
     */
    Coverage();
    /*!
     * The constructor for an coverage based on a resource
     * \sa IlwisObject(const Resource &source)
     * \param source
     */

    Coverage(const Resource& source);

    ~Coverage();

    /*!
     * Query for the coordinate system of this coverage
     *
     * \return the coordinate system if it has one or else null
     */
    ICoordinateSystem coordinateSystem() const;

    /*!
     * Changes the coordinate system of this coverage into the one specified
     * \param the new coordinate system
     */
    void setCoordinateSystem(const ICoordinateSystem& csy);

    /*!
     * Query for the envelope of this coverage
     *
     * \return the envelope of this coverage if it has one or else null
     */
    Box3D<double> envelope() const;

    /*!
     * Changes the envelope of this coverage to the one specified
     *
     * \param the new envelope
     */
    void envelope(const Box3D<double> &bnds);

    /*!
     * \brief Gives either the Coveragetable or the Indextable
     *
     * Gives the Coveragetable when called without parameter, and gives the indextable if you use atINDEX as parameter
     * atCOVERAGE as parameter also gives the attributeable
     *
     * \param attType
     * \return an AttributeTable
     */
    AttributeTable attributeTable(AttributeType attType=atCOVERAGE) const ;

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
     * Query for the statistics of this coverage
     * \return the statistics of this coverage
     */
    NumericStatistics& statistics();

    /*!
     * Query for the datadefinition
     *
     * \sa DataDefinition
     * \return de datadefinition van deze coverage
     */
    const DataDefinition& datadefIndex() const;

    /*!
     * Query for the datadefinition
     *
     * \sa DataDefinition
     * \return de datadefinition van deze coverage
     */
    DataDefinition& datadefIndex();

    /*!
     * \brief value returns a value in the Coverage- or Indextable
     *
     * using the the colname and the itemid this function will return the correspondending value from the Coveragetable
     * if you ad a index, the value will be retrieved from the Indextable instead
     *
     * \param colName name of the required collumn
     * \param itemid id of the item in the collumn
     * \param layerIndex the index of the Indextable, default =-1 = Coveragetable
     * \return the requested value
     */
    QVariant value(const QString& colName, quint32 itemid, qint32 layerIndex = -1);

    /*!
     * Gives the Resource of this coverage
     *
     * \return the Resourece of this coverage
     */
    Resource source(int mode=cmINPUT) const;

    /*!
     * \brief layerIndex translates a value to a index from the Indextable
     * note : the returned index is not necessarily retrievable, as it is possible to get doubles back
     * this happens for interpollation purposes and such
     * \param value the value that must be translated to a index
     * \return the index of the value
     */
    double layerIndex(const QString& value);

    /*!
     * \brief setLayerIndexes sets the supplied items at the indexes
     *
     * uses the order specified in the itemrange itself
     *
     * \param items the items that have to be added
     */
    void setLayerIndexes(const ItemRange &items);

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
