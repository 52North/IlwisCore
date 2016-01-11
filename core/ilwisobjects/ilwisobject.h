#ifndef ILWISOBJECTIMPL_H
#define ILWISOBJECTIMPL_H

#include <QSharedData>
#include <QDateTime>
#include <QUrl>
#include <QDebug>
#include "kernel_global.h"
#include "locker.h"
#include "identity.h"
#include "resource.h"
#include "numericrange.h"
#include "juliantime.h"
#include "iooptions.h"


class QSqlRecord;

namespace Ilwis {

class IlwisObjectFactory;
class ConnectorInterface;
class Resource;
class IlwisObject;
struct IOOptions;

typedef IlwisTypes (*IlwisTypeFunction)(const QString& resource);

/*!
 *\brief A class that is the baseclass for all Geospatial data classes and their directly supporting classes
 *
 * The class should only be used for instances that must be singletons in the system. That is, there may exist only
 * one instance of it. The purpose of the class is to ensure automatic consistency of data in the system. Changes to one
 * reference to the object are automatically reflected in the others because they are the same object.
 *
 * A IlwisObject has a IlwisTypes, which can be any of the following, or a combination thereof:
 *
 * - itUNKNOWN : type not known
 * - itPOINT : a point (on a map)
 * - itLINE : a Line or a LineCoverage
 * - itPOLYGON : a polygon (on a map)
 * - itRASTER :a RasterCoverage
 * - itCOVERAGELIST : a list of Coverage s
 * - itOBJECTCOLLECTION : a list of IlwisObject s
 * - itNUMERICDOMAIN : a Domain with Numberic values (NumbericDomain
 * - itITEMDOMAIN : a ItemDomain
 * - itTIMEDOMAIN : -
 * - itCOORDDOMAIN : a Coordinate based domain (CoordinateDomain)
 * - itCOLORDOMAIN : a Color based domain
 * - itTEXTDOMAIN : a String based domain
 * - itCONVENTIONALCOORDSYSTEM : Projected coordinate system or lat-lon
 * - itORTHOCOORDSYSTEM : -
 * - itELLIPSOID : a geographical ellipsoid
 * - itPROJECTION : projection of coordinates
 * - itDATABASETABLE : a table from a database
 * - itFLATTABLE : a table from memory
 * - itREPRESENTATION : -
 * - itGEOREF : pixel to coordinate and the other way around
 * - itENVELOPE : Bounding Box
 * - itOPERATIONMETADATA : operation metadata
 * - itRASTERSIZE : the size in pixels of a raster
 * - itCATALOG : a collection of ilwisobjects
 * - itCONTAINER : external organisation of the ilwisobjects
 * - itGEODETICDATUM : geodetic datum
 * - itBOOL : a boolean value
 * - itINT8 : a 8 bit signed integer value
 * - itUINT8 : a 8 bit unsigned integer value
 * - itUINT16 : a 16 bit signed integer value
 * - itINT16 : a 16 bit unsigned integer value
 * - itUINT32 : a 32 bit unsigned integer value
 * - itINT32 : a 32 bit signed integer value
 * - itUINT64 : a 64 bit unsigned integer value
 * - itINT64 : a 64 bit signed integer value
 * - itFLOAT : a float value
 * - itDOUBLE : a double value
 * - itVALUERANGE : a range of numeric values
 * - itSTRING : a String value
 * - itTIME : a time (hh,mm,ss.ss) value
 * - itDATE : a date (yyyy,mm,dd) value
 * - itDATETIME : a datetime (yyyy,mm,dd,hh,mm,ss.ss) value
 * - itCOORD2D : a 2D (x,y) coordinate
 * - itCOORD3D : a 3D (x,y,z) coordinate
 * - itPIXEL : a pixel (x,y)
 * - itVOXEL : a voxel, a (x,y,z) pixel
 * - itBINARY : a binary value
 * - itFILE : a file
 * - itURL : an url value
 * - itTHEMATICITEM : an item on a thematic domain
 * - itNAMEDITEM : an item on a named domain
 * - itINDEXEDITEM : an item on a indexed domain
 * - itNUMERICITEM : an item on a NumericDomain
 * - itTIMEITEM : an item on a time domain
 * - itCOLOR : an item on a color domain
 * - itANY : the wildcard type
 */
class KERNELSHARED_EXPORT IlwisObject : public QObject, public Identity
{
    Q_OBJECT

    friend class IlwisObjectFactory; //  needs to set validity of object
    friend void addFactory(IlwisObjectFactory *);

public:
    /*!
     * \brief The ConnectorMode enum Describes the state of the instance of the connector
     *
     *connector maybe for input/output or both. This enum describes how a connector can be used.
     */
    enum ConnectorMode{cmINPUT=1, cmOUTPUT=2, cmEXTENDED=4};

    /**
     * this enum describes how certain data should be stored, as metadata or as binary data
     */
    enum StoreMode{smMETADATA=1, smBINARYDATA=2};

    /**
     * Standard constructor for an ilwisobject
     */
    IlwisObject();

    /**
     * Constructor for an ilwis object using a Resource
     * the resource will be used to fill the identity field of this object
     *
     *\sa Resource
     * @param source the resource to be used
     */
    IlwisObject(const Resource &source);

    virtual ~IlwisObject();

    /**
     * tries to Prepares this ilwisobject for use
     * @return true when the prepare succeeded
     */
    virtual bool prepare(const IOOptions& options=IOOptions());

    /**
     * Changes the name of this object, new name cannot be null.
     * neither can you change the name if the IlwisObject is readonly
     *
     * @param nm the new name
     */
    void name(const QString& nm);
    QString name() const { return Identity::name(); }

    /**
     * Changes the code of this IlwisObject, can only happen when this object is not readonly,
     * and the identity is not null
     *
     * @param cd the new code
     */
    void code(const QString &cd);
    QString code() const { return Identity::code();}

    /*!
     *\brief returns the moment when the object was last modified
     *
     *\return QDateTime
     */
    Time modifiedTime() const;


    /*!
     *\brief sets the last modified time of the object
     *
     *\param time
     */
    void modifiedTime(const Time& time);

    /**
     * returns the create time of this object
     *
     * @return Time
     */
    Time createTime() const;

    /**
     * Changes the create time of this object
     *
     * @param time
     */
    void createTime(const Time& time);

    /*!
     * \brief translates the defintion of an object to a string representation
     *
     *\return QString
     */
    virtual QString toString();

    /*!
     * \brief connects an ilwisobject to an input or output source
     *
     * IlwisObjects are agnostic to where their data comes from. The connector isolates them from that. An object might
     * have two connectors : one for input, one for output. Mostely though there is only one that can be used for both
     * input and output ( if the connector allows it).
     *
     *\param connector the connector to be used
     *\param input defines the nature of the connector.
     */
    virtual bool setConnector(ConnectorInterface *connector, int mode=cmINPUT | cmOUTPUT, const IOOptions &options=IOOptions());

    /*!
     * \brief isEqual compares the properties of ilwisobjects to test for equality.
     *
     *It also will test the id's in which case the equality is guarenteed. The method is virtual so it will be re-implemented at each derivative.
     * \param obj an ilwisobject to be compared with
     * \return true when the objects are equal
     */
    virtual bool isEqual(const Ilwis::IlwisObject *obj) const;

    /*!
     * \brief isValid Tests the validity of an ilwisobject.
     *
     *Ilwisobjects are valid when all their properties are valid. Usualy this is determined during the 'loadXXX' methods that fill the properties of an object
     * \return valid or not
     */
    virtual bool isValid() const;

    /*!
     * \brief isReadOnly tells if the source/target of the ilwisobject maybe modifiable.
     *
     * This method returns the combination of the connectors readonly state and the objects readonly state. The connectors readonly state points usualy to a physical or other lower level readonly state. while the objects readonly state points to a logical state.
     * \return readonly or not
     */
    virtual bool isReadOnly() const;
    virtual bool outputConnectionReadonly() const;

    /**
     * Changes the readonly state of this object. in order to do this it will both change the connectors readonly state and the objects. If the connectors state can't be set to read/write only the state of the object will be set
     *
     * @param yesno the new readonly state
     */
    virtual void readOnly(bool yesno);

    /**
     * Checks if this object has been changed, requires the changed flag to be set during the actual change, so might not always be fully acurate
     * @return true when the object has changed
     */
    virtual bool hasChanged() const;

    /**
     * Sets the changed flag, needs to be set when something within this object gets changed
     *
     * @param yesno the new value of the changed flag
     */
    virtual void changed(bool yesno) ;

    /**
     * Gives the IlwisTypes of this object
     * \sa IlwisObject
     * @return the IlwisTypes of this IlwisObject
     */
    virtual IlwisTypes ilwisType() const = 0;

    /**
     * ??
     * @brief prepare
     * @param def
     * @return
     */
    virtual bool prepare(const QString& def);

    /**
     * fills the name description code and connector field from a database record
     * also sets the changed flag.
     * will not succeed if this object is readonly
     * @param rec the databaserecord (QSqlRecord)
     * @return true when succeeded
     */
    bool fromInternal(const QSqlRecord& rec);

    /**
     * checks the name of this object for the ANONYMOUS_PREFIX, this prefix must be at position 0 to generate a positive result
     *
     * @return true when this object is ANONYMOUS
     */
    bool isAnonymous() const;
    /*!
     * \brief source the location of the source that represents the physical read origin of this object
     * \return url that is sufficient to find the source
     */
    virtual Resource source(int mode=cmINPUT) const;

    /*!
     * \brief target the location of the source that represents the physical write target of this object
     * \return url that is sufficient to find the target
     */
    virtual bool store(const IOOptions& options = IOOptions());
    virtual bool loadData(const IOOptions &options = IOOptions());

    /*!
     connectTo allocates a connector of a certain format to the ilwisobject.
     * \param url the source/target were the connector connects to. The url maybe empty, internally an url will be created based on the format, name and working catalog
     * \param format the format identifying the source/target format. The name of the format is registered (plus namespace) by the connector plugin that realizes this format
     * \param fnamespace an additional identifier for selecting the correct connector. format names maybe identical from different connectors. The combination namespace-format is unique accross the system
     * \param cmode the mode of the connector to be created
     */
    void connectTo(const QUrl &url, const QString &format, const QString &fnamespace, ConnectorMode cmode, const IOOptions &options = IOOptions());


    //TODO. Puzzeld, this method is not allowed as duplicate copies are illegal
    virtual IlwisObject *clone() ;


    /**
     * Tries to merge 2 IlwisObjects together, template function does not work for standard IlwisObject, see the correspondending subclass for details
     *
     * @param obj the object this should merge with
     * @param options merging options
     * @return true when succesful
     */
    virtual bool merge(const IlwisObject* obj, int options=0);
    bool isSystemObject() const;
    virtual bool isInternalObject() const;
    bool isRemote() const;
    void remove();
    virtual bool canUse(const IlwisObject *obj, bool strict=false) const;
    virtual bool isCompatibleWith( const IlwisObject* obj, bool strict=false) const;

    template<typename T> static T create(const QString& def){
        T object;
        object.prepare(def);
        return object;
    }

    template<typename T> static T create(quint64 id){
        T object;
        object.prepare(id);
        return object;
    }

    /**
     * Creates a new IlwisObject from the given Resource
     * will fail if the resource is somehow invalid
     *
     * @param source the Resource
     * @return a new ilwisObject when succesful, or 0 when it fails
     */
    static IlwisObject *create(const Resource& source,const IOOptions& options=IOOptions());

    /**
      ??
     * @brief findType
     * @param source
     * @return
     */
    static IlwisTypes findType(const QString &source);

    /**
     * creates a String with the name of the given IlwisTypes
     * will return undefined if the given IlwisTypes is invalid
     * \sa IlwisObject
     * @param the IlwisTypes in need of naming
     * @return A string with the name of the IlwisTypes
     */
    static QString type2Name(IlwisTypes t);

    /**
     * converts a string with a name into a IlwisTypes
     * if the name is invalid it will return itUNKOWN
     *
     * @param name a string with the name of an IlwisTypes
     * @return the correspondending IlwisTypes
     */
    static IlwisTypes name2Type(const QString &name);
    static quint64 internalname2id(const QString& name, bool resolve = true);

    /**
      ??
     * @brief addTypeFunction
     */
    static void addTypeFunction(IlwisTypeFunction);

    static IlwisTypes name2ExtendedType(const QString &dname);

    QString formatCode(bool input=true) const;
    QString provider(bool input=true) const;
    double pseudoUndef() const;
    virtual void setPseudoUndef(double v);
protected:
   QScopedPointer<ConnectorInterface>& connector(int mode=cmINPUT | cmOUTPUT);
   const QScopedPointer<ConnectorInterface> &connector(int mode=cmINPUT | cmOUTPUT) const;
   bool setValid(bool yesno);
   void copyTo(IlwisObject *obj);

   std::recursive_mutex _mutex;
   std::mutex _loadforstore;
private:

   bool _valid;
   bool _readOnly;
   bool _changed;

   double _pseudoUndef=rUNDEF;
   QScopedPointer<Ilwis::ConnectorInterface> _connector;
   QScopedPointer<Ilwis::ConnectorInterface> _outConnector;
   static QVector<IlwisTypeFunction> _typeFunctions;


};


}

bool operator==(const Ilwis::IlwisObject& obj1, const Ilwis::IlwisObject& obj2);
#define IVARIANT(o) qVariantFromValue(o)




#endif // ILWISOBJECTIMPL_H
