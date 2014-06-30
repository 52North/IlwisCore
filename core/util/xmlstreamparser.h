#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "kernel_global.h"

class QXmlStreamReader;
class QXmlStreamAttributes;

class KERNELSHARED_EXPORT XmlStreamParser
{
public:

    XmlStreamParser(QXmlStreamReader *reader);
    XmlStreamParser(QIODevice *device);

    QXmlStreamReader *reader() const;
    void setXmlReader(QXmlStreamReader *reader);
    void addNamespaceMapping(QString prefix, QString ns);

    QString getPrefixForNamespaceUri(QString namespaceUri) const;

    /**
     * Starts parsing the underlying xml by reading the next/first start element.
     *
     * @param qName the root element's name.
     * @return true if element was found, false if not or the document has ended.
     */
    bool startParsing(QString qName) const;
    bool hasError() const;
    bool atEnd() const;

    QXmlStreamAttributes attributes() const;

    QString readElementText() const;

    QString name() const;

    QString namespaceUri() const;

    QString qname() const;

    /**
     * Moves to next end element named by qName.
     *
     * @param qName the name of the end element to move to.
     * @return true if element was found, false otherwise.
     */
    bool moveToEndOf(QString qName);

    /**
     * Moves to next element named by qName which resides one level deeper. Optionally a callback
     * can be passed in which will be called on each element node the reader passes.
     *
     * @param qName the name of the element to move to.
     * @return true if element was found, false otherwise.
     */
    bool moveToNext(QString qName, void (*callback)()=doNothing);

    /**
     * Finds the next inner element named by one of the qNames in the elementList. Optionally
     * a callback can be passed in which will be called on each element node the reader
     * passes.
     *
     * @param elementList a list of element names to find. First qNames have precendence.
     * @return true if an element was found, false otherwise.
     */
    bool findNextOf(std::initializer_list<QString> elementList, void (*callback)()=doNothing);

    bool readNextStartElement() const;

    void skipCurrentElement() const;

    void readNext() const;

    bool isAtBeginningOf(QString qName) const;
    bool isAtEndOf(QString qName) const;


protected:
    QXmlStreamReader *_reader;

private:
    QMap<QString,QString> _namespaces;

    bool isAtElement(QString qName) const;

    static void doNothing() {}

    template<typename F>
    void nextElementDo(F f)
    {
        f();
    }
};

#endif // XMLPARSER_H
