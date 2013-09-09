#ifndef ILWISOBJECTIMPL_H
#define ILWISOBJECTIMPL_H

#include <QSharedData>
#include <QDateTime>
#include <QUrl>
#include <QDebug>
#include "Kernel_global.h"
#include "locker.h"
#include "identity.h"
#include "resource.h"
#include "numericrange.h"
#include "juliantime.h"
#include "serializationoptions.h"


class QSqlRecord;

namespace Ilwis {

class IlwisObjectFactory;
class ConnectorInterface;
class Resource;

typedef IlwisTypes (*IlwisTypeFunction)(const QString& resource);

/*!
 \brief A class that is the baseclass for all Geospatial data classes and their directly supporting classes

 The class should only be used for instances that must be singletons in the system. That is, there may exist only
 one instance of it. The purpose of the class is to ensure automatic consistency of data in the system. Changes to one
 reference to the object are automatically reflected in the others because they are the same object.

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
    enum StoreMode{smMETADATA=1, smBINARYDATA=2};

    IlwisObject();
    IlwisObject(const Resource &resource);
    virtual ~IlwisObject();

    virtual bool prepare();
    void setName(const QString& nm);
    void setCode(const QString &cd);

   /*!
    \brief returns the moment when the object was last modified

    \return QDateTime
   */
   QDateTime modifiedTime() const;

   /*!
    \brief sets the last modified time of the object

    \param time
   */
   void setModifiedTime(const Time& time);
   Time createTime() const;
   void setCreateTime(const Time& time);

   /*!
    \brief translates the defintion of an object to a string representation

    \return QString
   */

   virtual QString toString();

   /*!
    \brief connects an ilwisobject to an input or output source

    IlwisObjects are agnostic to where their data comes from. The connector isolates them from that. An object might
    have two connectors : one for input, one for output. Mostely though there is only one that can be used for both
    input and output ( if the connector allows it).

    \param connector the connector to be used
    \param input defines the nature of the connector.
   */
   virtual void setConnector(ConnectorInterface *connector, int mode=cmINPUT | cmOUTPUT);

   /*!
    * \brief isEqual compares the properties of ilwisobjects to test for equality.
    *
    *It also will test the id's in which case the equality is guarenteed. The method is virtual so it will be re-implemented at each derivative.
    * \param obj an ilwisobject to be compared with
    * \return true when the objects are equal
    */
   virtual bool isEqual(const IlwisObject &obj) const;
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
   virtual bool hasChanged() const;
   virtual IlwisTypes ilwisType() const = 0;
   virtual bool prepare(const QString& def);

   bool fromInternal(const QSqlRecord& rec);
   bool isAnonymous() const;

   /*!
    * \brief source the location of the source that represents the physical read origin of this object
    * \return url that is sufficient to find the source
    */
   virtual Resource resource(int mode=cmINPUT) const;
   /*!
    * \brief target the location of the source that represents the physical write target of this object
    * \return url that is sufficient to find the target
    */
   virtual bool store(int mode=smMETADATA | smBINARYDATA);
   /*!
    connectTo allocates a connector of a certain format to the ilwisobject.
    * \param url the source/target were the connector connects to. The url maybe empty, internally an url will be created based on the format, name and working catalog
    * \param format the format identifying the source/target format. The name of the format is registered (plus namespace) by the connector plugin that realizes this format
    * \param fnamespace an additional identifier for selecting the correct connector. format names maybe identical from different connectors. The combination namespace-format is unique accross the system
    * \param cmode the mode of the connector to be created
    */
   void connectTo(const QUrl &url, const QString &format, const QString &fnamespace, ConnectorMode cmode);

   /*!
    \brief Factory method for creating IlwisObjects

    \param id of a resource a Url describing a unique Ilwis object.
    \param connectorType the connector that should handle this resource. If none is given ("default"), the system will figure it out by it self
    \return IlwisObject a created ilwisobject of the specified type
   */
   virtual IlwisObject *copy() ;

   static IlwisObject *create(const Resource& item);
   static IlwisTypes findType(const QString &resource);
   static QString type2Name(IlwisTypes t);
   static IlwisTypes name2Type(const QString &name);
   static void addTypeFunction(IlwisTypeFunction);

protected:
   QScopedPointer<ConnectorInterface>& connector(int mode=cmINPUT | cmOUTPUT);
   const QScopedPointer<ConnectorInterface> &connector(int mode=cmINPUT | cmOUTPUT) const;
   bool setValid(bool yesno);
   void copyTo(IlwisObject *obj);

   std::mutex _mutex;
private:

   bool _valid;
   bool _readOnly;
   bool _changed;
   Time _modifiedTime;
   Time _createTime;
   QScopedPointer<Ilwis::ConnectorInterface> _connector;
   QScopedPointer<Ilwis::ConnectorInterface> _outConnector;
   static QVector<IlwisTypeFunction> _typeFunctions;


};


}

bool operator==(const Ilwis::IlwisObject& obj1, const Ilwis::IlwisObject& obj2);
#define IVARIANT(o) qVariantFromValue(o)




#endif // ILWISOBJECTIMPL_H
