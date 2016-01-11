#ifndef ILWISDATA_H
#define ILWISDATA_H

#include <QUrl>
#include <QStringList>
#include <typeinfo>
#include <mutex>
//#include "ilwis.h"
#include "errorobject.h"
#include "iooptions.h"
#include "ilwisobject.h"
#include "resource.h"
#include "mastercatalog.h"


namespace Ilwis {
typedef std::shared_ptr<IlwisObject> ESPIlwisObject;

/*!
 \brief container for an instance of an IlwisObject

 The IlwisData object ensures that every IlwisObject is a singleton in the system. The QExplicitlySharedDataPointer of the class
 holds an unique pointer to the instance. The class also holds a (static) hash table to instances of all ilwisobjects that functions
 as a lookup table to quickly create new references to a particular IlwisObject. If a to instanced object is not in the hash table it will
 be created and initialized from the container were the data of the object resides ( disk, database, ...). If the object already exists
 it will be fetched from the hash table and returned to the caller. Its reference count will go up by one (automaticly).

*/
template<class T> class IlwisData {
public:
    template<class C> friend class IlwisData;

    IlwisData() {}
    IlwisData(const QString& name, IlwisTypes tp=itANY, const IOOptions& options=IOOptions()){
        prepare(name, tp, options);
    }
    IlwisData(const Resource& resource1,const IOOptions& options=IOOptions()){
        prepare(resource1, options);
    }


    IlwisData(T *data) {
        set(data);
    }

    IlwisData(const ESPIlwisObject& impl) {
        _implementation = impl;
    }

    template<typename K> IlwisData(const IlwisData<K>& obj) {
       _implementation = std::dynamic_pointer_cast<T>(obj._implementation);
    }

    IlwisData(const IlwisData& obj) {
       _implementation = obj._implementation;
    }



    ~IlwisData() {
        if (_implementation.get()!= 0  ) // there is always one extra reference in the registerd objects, so 2 means last object will disappear
            if ( _implementation.use_count() == 2)
                mastercatalog()->unregister(_implementation.get()->id());
    }

    void set(T *data) {
        removeCurrent();
        if ( data != nullptr) {
            if (!mastercatalog()->isRegistered(data->id())) {
                _implementation.reset(data);
                mastercatalog()->registerObject(_implementation);
            }
            else {
                _implementation = mastercatalog()->get(data->id());
            }
        }else {
            _implementation.reset();
        }
    }

    template<typename K> IlwisData<T>& assign(const IlwisData<K>& obj) {
        if ( _implementation && obj->ilwisType() == _implementation->ilwisType())
            set(static_cast<T *>(obj._implementation.get())) ;
        else
            set(dynamic_cast<T *>(obj._implementation.get())) ;
        return *this;
    }

    template<typename K> IlwisData<T>& operator=(const IlwisData<K>& obj) {
         return assign(obj);
    }

    IlwisData& operator=(const IlwisData& obj) {
         return assign(obj);
    }

    T *operator->() {
        T* p =  static_cast<T *>(_implementation.get());
        if (!p)
            throw ErrorObject(TR("Using unitialized ilwis object"));
        return p;
    }

    T *operator->() const{
        T* p =  static_cast<T *>(_implementation.get());
        if (!p)
            throw ErrorObject(TR("Using unitialized ilwis object"));
        return p;
    }

    T *ptr() const {
        return operator->();
    }

    /*!
     \brief casts an object to another object based on the template parameter.

     If the cast is not to a child derivative of the class, the class will fail and a exception will be thrown.
     This one of the few places in the system were an exception will be thrown. The catch for these exceptions
     must be placed on a place were the system can resume normal operations
     This operation is usually used the gain access to methods of the child derivative.

     \return Returns a new "upgraded" reference to the class.
    */
    template<class C=IlwisObject> IlwisData<C> as() const {
        if (_implementation.get() == 0)
            return IlwisData<C>();
            //throw ErrorObject(TR("Using unitialized object"));
        if ( hasType(_implementation->ilwisType(),itILWISOBJECT)) {
            IlwisData<C> obj;
            obj._implementation = std::static_pointer_cast<C>(_implementation);
            return  obj;
        }
        return IlwisData<C>();
    }
    /*!
     \brief create a new pristine IlwisObject. It has no connector (yet) and exists only in memory

      \return bool bool succes of the creation process. Any issues can be found in the issuelogger
    */
    bool prepare() {
        removeCurrent();
        auto type = kernel()->demangle(typeid(T).name());

        IlwisTypes tp = IlwisObject::name2Type(type);
        Resource res;
        res.prepare();
        res.setIlwisType(tp);
        tp = IlwisObject::name2ExtendedType(type);
        if ( tp != itUNKNOWN)
            res.setExtendedType(tp);
        QString name = QString("%1%2").arg(ANONYMOUS_PREFIX).arg(res.id());
        QUrl url(QString("ilwis://internalcatalog/%1").arg(name));
        res.name(name);
        res.setUrl(url);
        return prepare(res);

    }

    bool prepare(const QString& name,const IOOptions& options=IOOptions()){
        auto type = kernel()->demangle(typeid(T).name());
        IlwisTypes tp = IlwisObject::name2Type(type);
        return prepare(name, tp, options);
    }

    /*!
     The method ensures that a proper IlwisResource is requested from the system. It then tries to create the resource and
     on success initializes it. After that it registers itself to the system to prevent the creation of other instances of this resource.
     Any warnings or errors during its creation are transferred to the created object and the issue stack is cleared.

     \param resource the resource to be instanced as an IlwisObject
     \param connectorType the connector that should handle this resource. If none is given ("default"), the system will figure it out by it self
     \return bool bool succes of the creation process. Any issues can be found in the issuelogger
    */
    bool prepare(const QString& nme, IlwisTypes tp,const IOOptions& options=IOOptions()){
        QString name = Resource::quoted2string(nme);
        // see if it is an internal object (ANONYMOUS_, or ILWISOBJECT_)
        quint64 id = IlwisObject::internalname2id(name);
        if ( id != i64UNDEF) { // internal objects are not in the catalog
                ESPIlwisObject data = mastercatalog()->get(id);
                if ( data.get() != 0) {
                    removeCurrent();
                    _implementation = data;
                    return true;
                }

        }
        auto type = kernel()->demangle(typeid(T).name());
        IlwisTypes objecttp = IlwisObject::name2Type(type);
        if ( tp == itANY) {
            tp = objecttp;
        }else {
            if (!hasType(tp,objecttp)){
                QString message = QString("Could not create object. type %1 is not compatible with %2").arg(IlwisObject::type2Name(tp)).arg(IlwisObject::type2Name(objecttp));
                kernel()->issues()->log(message);
                return false;
            }
        }
        bool mustExist = false;
        if ( options.contains("mustexist"))
            mustExist = options["mustexist"].toBool();
        auto resource = mastercatalog()->name2Resource(name,tp );
        if (resource.isValid()) {
            if (!mastercatalog()->isRegistered(resource.id())) {
                T *data = static_cast<T *>(IlwisObject::create(resource, options));
                if ( data == 0) {
                    _implementation.reset((T*)0);
                    removeCurrent();
                    return ERROR1("Couldnt create ilwisobject %1",name);
                }
                bool ok = data->prepare();
                if ( !ok){
                    delete data;
                    return false;
                }

                data->changed(false);
                removeCurrent();
                _implementation = ESPIlwisObject(data);
                mastercatalog()->registerObject(_implementation);
            } else {
                _implementation = mastercatalog()->get(resource.id());
            }
            return true;
        } else {
            if ( mustExist)
                return false;

            if(tp != itUNKNOWN && prepare(Resource(name, tp), options))
                return true;
        }
        return ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1,name);

    }

    bool prepare(const Resource& resource1,const IOOptions& options=IOOptions()){
        if (resource1.isValid()) {
            quint64 id = iUNDEF;
            //if ( mastercatalog()->usesContainers(resource1.url())) // containers dont make sense for services
            //    mastercatalog()->addContainer(resource1.container());
            id = mastercatalog()->url2id(resource1.url(), resource1.ilwisType());
            Resource resource = mastercatalog()->id2Resource(id);
            if (!resource.isValid())
                resource = resource1;
            auto type = kernel()->demangle(typeid(T).name());
            IlwisTypes objecttp = IlwisObject::name2Type(type);
            if ( objecttp == itANY || !hasType(objecttp, resource.ilwisType())){
                kernel()->issues()->log(TR("Requested object type doesnt match object type found in the master catalog; Is the requested resource correct?"));
                return false;
            }
            if (!mastercatalog()->isRegistered(resource.id())) {
                T *data = static_cast<T *>(IlwisObject::create(resource, options));
                if ( data == 0) {
                    _implementation.reset((T*)0);
                    removeCurrent();
                    return ERROR1("Couldnt create ilwisobject %1",resource.name());
                }
                bool ok = data->prepare(options);
                if ( !ok){
                    delete data;
                    return false;
                }
                data->changed(false);
                removeCurrent();
                _implementation = ESPIlwisObject(data);
                mastercatalog()->registerObject(_implementation);
            } else {
                _implementation = mastercatalog()->get(resource.id());
            }
            return true;
        } else {
            ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1,resource1.name());
        }
        return false;

    }

    /*!
     \brief constructs an IlwisObject and registers its existens.

     The method ensures that a proper IlwisResource is requested from the system. It then tries to create the resource and
     on success initializes it. After that it registers itself to the system to prevent the creation of other instances of this resource.
     Any warnings or errors during its creation are transferred to the created object and the issue stack is cleared.

     \param name a string representation of a resource. This might be an Url/IlwisResource but other representations can also be used
     \return bool succes of the creation process. Any issues can be found in the issuelogger
    */
    bool prepare(const quint64& iid,const IOOptions& options=IOOptions()){
        Resource resource = mastercatalog()->id2Resource(iid);
        if (!mastercatalog()->isRegistered(iid)) {
            T *data = static_cast<T *>(IlwisObject::create(resource, options));
            if ( data != 0)
                data->prepare();
            else {
                _implementation.reset((T*)0);
                removeCurrent();
                return ERROR1("Couldnt create ilwisobject %1",resource.name());
            }
            removeCurrent();
            _implementation = ESPIlwisObject(data);
        } else
            _implementation = mastercatalog()->get(iid);
        if ( _implementation.get() == 0) {
            return ERROR0("Corrupted object registration");
        }
        mastercatalog()->registerObject(_implementation);
        return true;

    }


    /*!
     \brief An instance is valid if an object has been allocated

     \return bool true of an object is allocated
    */
    bool isValid() const {
        return _implementation.get() != 0;
    }

private:
    void removeCurrent() {
        if ( _implementation && _implementation->id() != i64UNDEF) {
            ESPIlwisObject obj= mastercatalog()->get( _implementation->id() );
            if(obj.use_count() <= 3) { // current _impl + one in the lookup + the one just created
                 mastercatalog()->unregister(_implementation->id());
            }
        }
    }

    ESPIlwisObject _implementation;
};

template<class T> bool operator==(const IlwisData<T>& d1, const IlwisData<T>& d2) {
    return d1->id() == d2->id();
}

template<class T> bool operator!=(const IlwisData<T>& d1, const IlwisData<T>& d2) {
    return d1->id() != d2->id();
}

typedef Ilwis::IlwisData<Ilwis::IlwisObject> IIlwisObject;

}

Q_DECLARE_METATYPE(Ilwis::IIlwisObject)

template<class C> QDataStream & operator << ( QDataStream & s, const Ilwis::IlwisData<C> & obj ){
    if ( obj.isValid() == false)
        return s;

    obj->store(s, obj->serializationOptions());

    return s;
}

template<class C> QDataStream & operator >> ( QDataStream & s, Ilwis::IlwisData<C> & obj ){
    if ( obj.isValid() == false)
        obj.prepare();
    obj->load(s);

    return s;
}


#endif // ILWISDATA_H

