#ifndef XMLPARSER_H
#define XMLPARSER_H

#include <QObject>
#include <QMap>

#include "kernel_global.h"

class QTimer;
class QIODevice;
class QEventLoop;
class QXmlStreamReader;
class QXmlStreamAttributes;

namespace Ilwis {

class KERNELSHARED_EXPORT XmlStreamParser: QObject
{
    Q_OBJECT

public:

    XmlStreamParser(QXmlStreamReader *reader);
    XmlStreamParser(QIODevice *device);
    ~XmlStreamParser();

    QXmlStreamReader *reader() const;
    void setXmlReader(QXmlStreamReader *reader);
    void addNamespaceMapping(QString prefix, QString ns);
    void addTargetNamespaceMapping(QString ns);
    QString targetNamespace() const;

    QString getPrefixForNamespaceUri(QString namespaceUri);

    /**
     * Starts parsing the underlying xml by reading the next/first start element.
     *
     * @param qName the root element's name.
     * @return true if element was found, false if not or the document has ended.
     */
    bool startParsing(QString qName);

    bool isStartElement();
    bool isEndElement();
    bool atEnd();

    bool hasError();
    QString errorString();

    QXmlStreamAttributes attributes() ;

    QString readElementText() ;

    QString name();

    QString namespaceUri() ;

    QString qname();

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

    bool readNextStartElement() ;

    void skipCurrentElement();

    void readNext() ;

    bool isAtBeginningOf(QString qName) ;
    bool isAtEndOf(QString qName) ;

public slots:
    void abort();
    void readChannelFinished();
    void readIncomingData();


protected:
    QXmlStreamReader *_reader;

private:
    QTimer *_timer;
    QEventLoop *_loop;
    QIODevice *_device;
    QMap<QString,QString> _namespaces;
    bool _readingChannelFinished = false;

    bool isAtElement(QString qName);

    bool resolveFromPrematureEndOfDocument();
    bool canProceedParsing();

    static void doNothing() {}

    template<typename F>
    void nextElementDo(F f)
    {
        f();
    }
};

}
#endif // XMLPARSER_H
