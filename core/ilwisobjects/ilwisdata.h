#ifndef ILWISDATA_H
#define ILWISDATA_H

#include <QUrl>
#include <QStringList>
#include <typeinfo>
#include <mutex>
#include "ilwis.h"
#include "errorobject.h"
#include "ilwisobject.h"
#include "resource.h"
#include "mastercatalog.h"


namespace Ilwis {
typedef QExplicitlySharedDataPointer<IlwisObject> ESPIlwisObject;

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
    IlwisData(T *data) {
        set(data);
    }

    IlwisData(const ESPIlwisObject& impl) {
        _implementation = impl;
    }


    ~IlwisData() {
        if (_implementation.data()!= 0  ) // there is always one extra reference in the registerd objects, so 2 means last object will disappear
            if ( _implementation.data()->ref.load() == 2)
                mastercatalog()->unregister(_implementation.data()->id());
    }

    void set(T *data) {
        if ( data != nullptr) {
            if (!mastercatalog()->isRegistered(data->id())) {
                _implementation = data;
                mastercatalog()->registerObject(_implementation);
            }
            else
                _implementation = mastercatalog()->get(data->id());
        }
    }

    template<typename K> IlwisData<T>& operator=(const IlwisData<K>& obj) {
        set(dynamic_cast<T>(obj._implementation.data())) ;
        return *this;
    }

    T *operator->() {
        T* p =  static_cast<T *>(_implementation.data());
        if (!p)
            throw ErrorObject(TR("Using unitialized object"));
        return p;
    }

    T *operator->() const{
        T* p =  static_cast<T *>(_implementation.data());
        if (!p)
            throw ErrorObject(TR("Using unitialized object"));
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
    template<class C> IlwisData<C> get() const {
        QExplicitlySharedDataPointer<C> p(dynamic_cast<C *>(_implementation.data()));
        if (!p)
            throw ErrorObject(TR("Using unitialized object"));

        return IlwisData<C>(p);
    }
    /*!
     \brief create a new pristine IlwisObject. It has no connector (yet) and exists only in memory

      \return bool bool succes of the creation process. Any issues can be found in the issuelogger
    */
    bool prepare() {
        _implementation = QExplicitlySharedDataPointer<IlwisObject>(new T);
        if (_implementation.data() != 0) {
            _implementation.data()->prepare();
            _implementation.data()->setName(QString("internal_%1").arg(_implementation.data()->id()));
        }
        return _implementation.data() != 0;
    }

    /*!
     The method ensures that a proper IlwisResource is requested from the system. It then tries to create the resource and
     on success initializes it. After that it registers itself to the system to prevent the creation of other instances of this resource.
     Any warnings or errors during its creation are transferred to the created object and the issue stack is cleared.

     \param res the resource to be instanced as an IlwisObject
     \param connectorType the connector that should handle this resource. If none is given ("default"), the system will figure it out by it self
     \return bool bool succes of the creation process. Any issues can be found in the issuelogger
    */
    bool prepare(const QString& name){
        auto type = kernel()->demangle(typeid(T).name());
        auto tp = IlwisObject::name2Type(type);
        auto item = mastercatalog()->name2Resource(name,tp );
        if (item.isValid()) {
            if (!mastercatalog()->isRegistered(item.id())) {
                T *data = static_cast<T *>(IlwisObject::create(item));
                if ( data == 0) {
                    return ERROR1("Couldnt create ilwisobject %1",name);
                }
                data->prepare();
                _implementation = ESPIlwisObject(data);
                mastercatalog()->registerObject(_implementation);
            } else {
                _implementation = mastercatalog()->get(item.id());
            }
            return true;
        } else {
            ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1,name);
        }
        return false;

    }

    bool prepare(const Resource& item){
        if (item.isValid()) {
            if (!mastercatalog()->isRegistered(item.id())) {
                T *data = static_cast<T *>(IlwisObject::create(item));
                if ( data == 0) {
                    return ERROR1("Couldnt create ilwisobject %1",item.name());
                }
                data->prepare();
                _implementation = ESPIlwisObject(data);
                mastercatalog()->registerObject(_implementation);
            } else {
                _implementation = mastercatalog()->get(item.id());
            }
            return true;
        } else {
            ERROR1(ERR_COULDNT_CREATE_OBJECT_FOR_1,item.name());
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
    bool prepare(const quint64& iid){
        Resource item = mastercatalog()->id2Resource(iid);
        if (!mastercatalog()->isRegistered(iid)) {
            T *data = static_cast<T *>(IlwisObject::create(item));
            if ( data != 0)
                data->prepare();
            else {
                return ERROR1("Couldnt create ilwisobject %1",item.name());
            }
            _implementation = ESPIlwisObject(data);
        } else
            _implementation = mastercatalog()->get(iid);
        if ( _implementation.data() == 0) {
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
        return _implementation.data() != 0;
    }


private:
    ESPIlwisObject _implementation;
};

template<class T> bool operator==(const IlwisData<T>& d1, const IlwisData<T>& d2) {
    return d1->id() == d2->id();
}

template<class T> bool operator!=(const IlwisData<T>& d1, const IlwisData<T>& d2) {
    return d1->id() != d2->id();
}


}

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

