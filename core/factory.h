#ifndef FACTORY_H
#define FACTORY_H

class QUrl;

//TODO: merge with abstractfactory as the split is not useful anymore

namespace Ilwis {

class Resource;

/*!
 * \brief The FactoryInterface class general placeholder interface for all factory classes in Ilwis
 *
 *The masterfactory stores pointers with this interface. It is a very generic interface so it must be cased toa specific type to become useful
 */
class FactoryInterface {
public:
    /*!
     * \brief description
     * \return a brief description whata certain factory is supposed to do. It is optional
     */
    virtual QString description() const = 0;
    /*!
     * \brief type the main type of the factory.
     *
     *This member, together with the subtype is used to select factories from the masterfactory.
     *
     * \return the name of the type
     */
    virtual QString type() const = 0;
    /*!
     * \brief subtype further specializes the search key for factories
     *
     *The subtype usualy limits the factory to a module were this factory is implemented. It is used when they are nameclashes about the type of the factory.
     * \return the name of the subtype;
     */
    virtual QString subtype() const = 0;
    /*!
     \brief string combinations of type subtype

     Convenience method for creating a full type description for a factory

     \return QString conbined name
    */
    virtual QString key() const = 0;

    /*!
     \brief test if a resource can be used by this factory to create something from the resource

     Not all factories can create all the objects of course. This method checks if a resource is usefull
     for a factory

     \param resource the resource to be tested
     \return bool true if the object type indicated by the resource can be created by the factory
    */
    virtual bool canUse(const Resource& resource) const = 0;
    /*!
     * \brief prepare initializes the factory
     * \return
     */
    virtual bool prepare() = 0;
};

}



#endif // FACTORY_H
