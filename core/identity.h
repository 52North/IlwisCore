#ifndef IDENTITY_H
#define IDENTITY_H

#include "kernel_global.h"
#include "ilwis.h"
#include "identityinterface.h"

#define ANONYMOUS_PREFIX "_ANONYMOUS_"
#define NAME_ALIAS "_ILWISOBJECT_"
#define SZ_ANONYMOUS_PREFIX (strlen(ANONYMOUS_PREFIX))
#define SZ_NAME_ALIAS (strlen(NAME_ALIAS))

namespace Ilwis {

struct SerializationOptions;

/*!
 \brief general class to describe the identity of a class

 The identity is used as baseclass for a number of classes that need a unique identification
 The code and id are unique. The code has an attached meaning usualy related to the type of the instance
 Other members might be unique (e.g. name) but that depends on the type of
 class. Codes are not mandatory.

*/
class KERNELSHARED_EXPORT Identity : public IdentityInterface {
public:
    friend class Resource;

    Identity();

    /*!
     \brief  sets the code for this instance

     \param code unique code from a code set that is unique for this class
    */
    virtual void code(const QString& code);
    /*!
     * \brief code returns the code of the identity. If no code is attached it will return sUNDEF ("?").
     *
     *A code is unique for a certain type of objects. The difference between code and id (see below) is that code may have some meaning and that they are only unique
     *for a certain type of objects (e.g. epsg number for coordinate systems), while id are all uniquem irrespective of the type.
     *
     * \return a string identifying the object
     */
    QString code() const;
    /*!
     * \brief id returns a unsigned 64-bit number unique for the objects using the identity
     * \return 64-bits unsigned integer
     */
    quint64 id() const;
    /*!
     * \brief newId generates a new id for the identity. usefull after a copy constructor which should copy, not clone
     */
    void newId(quint64 base=1000000);
    /*!
     * \brief name returns the name of an object. All objects have names though names need not to be unique
     * \return
     */

    virtual QString name() const;
    /*!
     * \brief setName sets the name of an object. All objects have names though names need not to be unique
     * \param n the name
     */
    void name(const QString& n);
    virtual QString description() const;
    void setDescription(const QString& desc);
    bool store(QDataStream& stream) const;
    bool load(QDataStream &stream);

    static QString newAnonymousName() ;
    static quint64 newAnonymousId() ;
    static void setBaseId(quint64 base);


protected:
    Identity(const QString &name, qint64 id=i64UNDEF, const QString &cde=sUNDEF, const QString &descr=sUNDEF);

    virtual void prepare(quint64 base=1000000);
    void setId(quint64 newid);

private:
    quint64 _id;
    QString _name;
    QString _description;
    QString _code;
    static qint64 _baseId;

};

const quint64 idNOID = -1;

}

#endif // IDENTITY_H
