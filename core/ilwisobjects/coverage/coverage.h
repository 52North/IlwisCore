#ifndef BASELAYER_H
#define BASELAYER_H

#include "kernel_global.h"
#include "geos/geom/Geometry.h"
#include "geometries.h"
#include "ilwisobject.h"
#include "ilwisdata.h"
#include "domain.h"
#include "coordinatesystem.h"
#include "datadefinition.h"
#include "indexdefinition.h"
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
     * An AttributeTable can have 2 types depending on the object, it can be a coveragetable,<br>
     * in which the features are defined at only 1 index, or it can be a indextable,<br>
     * in which the features are defined at all known indexes.<br>
     * the indexes can for example describe different measurements at different times
     */
    enum AttributeType{atCOVERAGE, atINDEX};

    /*!
     * The constructor for an empty coverage
     */
    Coverage();

    /*!
     * The constructor for a RasterCoverage with a a Resource that is registered in the mastercatalog.<br>
     * This resource must have a name and an Id.<br>
     * Code and description are optional.
     *
     * \sa IlwisObject
     * \sa Resource
     * \param source Resource that has to be used
     */
    Coverage(const Resource& source);

    virtual ~Coverage();

    /*!
     * Query for the coordinate system of this coverage
     *
     * \sa ICoordinateSystem
     * \return the coordinate system if it has one or else null
     */
    ICoordinateSystem coordinateSystem() const;

    /*!
     * Changes the coordinate system of this coverage into the one specified<br>
     * does not work if this coverage is readonly<br>
     * sets the changed flag
     *
     * \sa ICoordinateSystem
     * \param the new coordinate system
     */
    void coordinateSystem(const ICoordinateSystem& csy);

    /*!
     * Query for the envelope of this coverage, must fit in the coordinate system<br>
     * a envelope decides what domain within a coordinate system is valid, or in this case,<br>
     * which part of the coordinate system is covered by this coverage<br>
     * do note that the envelope should not be bigger than the coordinateSystem
     *
     * \return the envelope of this coverage if it has one or else null
     */
    Envelope envelope() const;

    /*!
     * Changes the envelope of this coverage to the one specified<br>
     * a envelope decides what domain within a coordinate system is valid, or in this case,<br>
     * which part of the coordinate system is covered by this coverage<br>
     * do note that the envelope should not be bigger than the coordinateSystem<br>
     * sets the changed flag
     *
     * \param the new envelope
     */
    void envelope(const Envelope &bnds);

    /*!
     *
     * Gives the Coveragetable when called without parameter, and gives the indextable if you use atINDEX as parameter<br>
     * atCOVERAGE as parameter also results in the attributeable
     *
     * \param attType the type of table required (atCOVERAGE or atINDEX)
     * \return an AttributeTable
     */
    AttributeTable attributeTable(AttributeType attType=atCOVERAGE) const ;

    /*!
     * Gives the Coveragetable when called without parameter, and gives the indextable if you use atINDEX as parameter<br>
     * atCOVERAGE as parameter also results in the attributeable
     *
     * \param attType the type of table required (atCOVERAGE or atINDEX)
     * \return an AttributeTable
     */
    AttributeTable &attributeTable(AttributeType attType=atCOVERAGE);

    /**
     * Checks for a valid attributetable.<br>
     * the parameter decides between coverage and index table. anyhting different than atCOVERAGE or atINDEX is not allowed
     *
     * @param attType must be atCOVERAGE or atINDEX
     * @return true if this coverage has a valid table of the chosen type
     */
    bool hasAttributes(AttributeType attType=atCOVERAGE) const;

    /*!
     * \brief Sets a new Coveragetable or Indextable
     *
     * When called without parameter, or with atCOVERAGE as parameter you will replace the Coveragetable<br>
     * When called with atINDEX as parameter it will replace the Indextable
     *
     * \param tbl the new table
     * \param attType the type of the new table
     */

    void attributeTable(const ITable& tbl, AttributeType attType=atCOVERAGE );

    /*!
     * Query for the NumbericStatistics of this coverage
     *
     *
     * \return the statistics of this coverage
     */
    virtual NumericStatistics& statistics(int mode=0);

    /*!
     * Returns the DataDefinition of this rastercoverage<br>
     * can be null if it is not set
     *
     * \sa DataDefinition
     * \return the datadefinition of this rastercoverage
     */
    const IndexDefinition& indexDefinition() const;
    IndexDefinition& indexDefinition();
    void indexDomain(const IDomain &dom);
    virtual std::vector<QVariant> indexValues() const;



    /*!
     * \brief Returns a value in the Coverage- or Index-table
     *
     * Using the colname and the itemid this function will return the correspondending value from the Coveragetable<br>
     * if you add an index, the value will be retrieved from the Indextable instead.
     *
     * \param colName name of the required collumn, must be a valid name
     * \param itemid id of the item in the collumn, must be a valid id
     * \param layerIndex the index of the Indextable, default =-1 = Coveragetable any negative index other than -1, will cause errors
     * \return the requested value, when all parameters are valid
     */
    QVariant value(const QString& colName, quint32 itemid, qint32 layerIndex = -1);

    //@override
    Resource source(int mode=cmINPUT) const;

    /**
     * Changes the name of this coverage<br>
     * only works if this coverage is not readonly<br>
     * will also adjust the name of the attribute and or indextable<br>
     * to respectively "name"_attributes and "name"_indexattributes
     *
     * @param nam the new name of this coverage
     */
    void name(const QString &nam);
    QString name() const { return IlwisObject::name(); }

    bool prepare();
protected:
    void copyTo(IlwisObject *obj) ;
private:

    ICoordinateSystem _coordinateSystem;
    Envelope _envelope;
    AttributeTable _attTable;
    AttributeTable _attTableIndex;
    NumericStatistics _statistics;
    IndexDefinition _indexdefinition;


};

typedef IlwisData<Coverage> ICoverage;


}





#endif // BASELAYER_H
