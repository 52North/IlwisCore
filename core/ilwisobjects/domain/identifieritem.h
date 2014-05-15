#ifndef IDENTIFIERITEM_H
#define IDENTIFIERITEM_H

#include "kernel_global.h"
#include "domainitem.h"

namespace Ilwis{

class NamedIdentifierRange;
/**
 * The IndexedIdentifier class
 */
class KERNELSHARED_EXPORT IndexedIdentifier : public DomainItem
{
public:

    friend class IndexedIdentifierRange;

    /**
     * Constructor for an empty IndexedIdentifier
     */
    IndexedIdentifier();

    /**
     * Creates an IndexedIdentifier from a given label and index, and optionally a counter
     *
     * @param label the label of this indentifier, will be used as a prefix
     * @param ind index of this identifier, standard 0
     * @param cnt counter of this identifier, standard iUNDEF
     */
    IndexedIdentifier(const QString& label, quint32 ind=0, qint32 cnt =iUNDEF);

    /**
     * Query for the name of this IndexedIdentifier<br>
     * normally the name equalls prefix+index(raw)
     *
     * @return the name of this object
     */
    QString name() const;

    /**
     * Query to the index of this IndexedIdentifier without prefix
     *
     * @return the index of this object
     */
    quint32 raw() const;

    //void setIndex(quint32 ind);

    /**
     * Query to the prefix of this IndexedIdentifier
     *
     * @return the prefix of this object
     */
    QString prefix() const;

    /**
     * Changes the prefix of this IndexedIdentifier to the specified value
     *
     * @param pf the new prefix
     */
    void setPrefix(const QString& pf);

    /**
     * override of operator==<br>
     *
     * an IndexedIdentifier is the equal to another if both the prefix and the raw are the same
     *
     * @param item item that should be compared
     * @return true when the identifiers are equal
     */
    bool operator==(const IndexedIdentifier& item) const;

    /**
     * Creates a new IndexedIdentifier with the same prefix raw and count
     *
     * @return a clone of this object
     */
    DomainItem *clone() const;

    //@override
    IlwisTypes valueType() const;

    /**
     * Checks if this object is valid
     *
     * @return returns true when valid
     */
    bool isValid() const;

    /**
     * Creates an itemrange according to this object
     * @return a new IndexedIdentifierRange
     */
    static ItemRange *createRange();

    //@override
    static IlwisTypes valueTypeS();

private:
    QString _prefix;
    quint32 _count;

};

class KERNELSHARED_EXPORT NamedIdentifier : public DomainItem {
public:

    friend class NamedIdentifierRange;

    /**
     * Constructs an empty NamedIdentifier
     */
    NamedIdentifier();

    /**
     * Constructs a NamedIdentifier with a given name
     *
     * @param name the name
     */
    NamedIdentifier(const QString& name, quint32 rawvalue=iUNDEF);

    /**
     * Query to check if this object is valid
     *
     * @return true when valid
     */
    bool isValid() const;

    /**
     * Query for the name of this NamedIdentifier
     *
     * @return The name of this object
     */
    QString name() const;

    /**
     * Changes the name of this object into the one specified
     *
     * @param n the new name
     */
    void setName(const QString& n);

    /**
     * override of operator==
     *
     * an NamedIdentifier is the equal to another if both have the same name
     *
     * @param item the NamedIdentifier which this one should be compared to
     * @return true when they are equal
     */
    bool operator==(const NamedIdentifier& item) const;

    /**
     * Query to the ilwistype of this object
     *
     * @return itNAMEDITEM
     */
    IlwisTypes valueType() const;

    /**
     * Creates a new NamedIdentifier with the same raw and name
     *
     * @return a clone of this object
     */
    DomainItem *clone() const;

    /**
     * Creates an itemrange according to this object
     * @return a new NamedIdentifierRange
     */
    static Ilwis::NamedIdentifierRange *createRange();

    /**
     * Query to the ilwistype of this object
     *
     * @return itNAMEDITEM
     */
    static IlwisTypes valueTypeS();

private:
    QString _name;
};

typedef QSharedPointer<NamedIdentifier> SPNamedIdentifier;
typedef QSharedPointer<IndexedIdentifier> SPIndexedIdentifier;
}

#endif // IDENTIFIERITEM_H
