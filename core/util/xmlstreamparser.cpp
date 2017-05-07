#include <QTimer>
#include <QMap>
#include <QString>
#include <QIODevice>
#include <QEventLoop>
#include <QXmlStreamReader>
#include <initializer_list>

#include "kernel.h"
#include "xmlstreamparser.h"

using namespace Ilwis;

/*
 *
 * Open:
 * The function pointer takes only free function pointers, i.e. member
 * free functions (doNothing is static).
 *
 * see:
 * http://stackoverflow.com/questions/2374847/passing-member-function-pointer-to-member-object-in-c
 *
 */

XmlStreamParser::XmlStreamParser(QXmlStreamReader *reader): _reader(reader)
{
}

XmlStreamParser::XmlStreamParser(QIODevice *device): _device(device)
{
    _timer = new QTimer;
    _timer->setSingleShot(true);
    _reader = new QXmlStreamReader();
    _loop = new QEventLoop;

    connect(_timer, SIGNAL(timeout()), this, SLOT(abort()));
    connect(_device, SIGNAL(readChannelFinished()), this, SLOT(readChannelFinished()));
    connect(_device, SIGNAL(readyRead()), this, SLOT(readIncomingData()));

    readIncomingData();
}

XmlStreamParser::~XmlStreamParser()
{
    _loop->quit();
    _loop->deleteLater();
}

void XmlStreamParser::setXmlReader(QXmlStreamReader *reader)
{
    _reader = reader;
}

void XmlStreamParser::abort()
{
    _loop->quit();
    _reader->raiseError("No more data receiced ...");
}

void XmlStreamParser::readChannelFinished()
{
    _readingChannelFinished = true;
    readIncomingData();
}

void XmlStreamParser::readIncomingData()
{
    if (_device->size() > 0) {
        _reader->addData(_device->read(_device->size()));
    }
    _loop->quit();
}

QXmlStreamReader *XmlStreamParser::reader() const
{
    return _reader;
}

void XmlStreamParser::addNamespaceMapping(QString prefix, QString ns)
{
    _namespaces[prefix] = ns;
}

void XmlStreamParser::addTargetNamespaceMapping(QString ns)
{
    _namespaces["target"] = ns;
}

QString XmlStreamParser::targetNamespace() const
{
    return _namespaces["target"];
}

QString XmlStreamParser::getPrefixForNamespaceUri(QString namespaceUri)
{
    return _namespaces.key(namespaceUri);
}

bool XmlStreamParser::startParsing(QString qName)
{
    if ( !_reader->isStartDocument()) {
        readNext();
    }
    if (canProceedParsing()) {
        readNextStartElement();
        return isAtBeginningOf(qName);
    }
    return false;
}

bool XmlStreamParser::resolveFromPrematureEndOfDocument()
{
    if (_reader->error() != QXmlStreamReader::PrematureEndOfDocumentError ) {
        return false;
    }
    if ( !_readingChannelFinished) {
        _timer->start(10000); // ... limited to 10s
        _loop->exec(); // wait for readyRead() or timeout()

        qint64 line = _reader->lineNumber();
    }
    return true;
}

QString XmlStreamParser::errorString()
{
    return _reader->errorString();
}

bool XmlStreamParser::hasError() {
    return _reader->hasError() && !resolveFromPrematureEndOfDocument();
}

bool XmlStreamParser::canProceedParsing()
{
    if (_readingChannelFinished) {
        if (hasError()) {
            qWarning() << errorString();
            ERROR1("Error parsing stream: %1", errorString());
            return false;
        }
        return true;
    } else {
        if ( !hasError()) {
            return true;
        } else {
            if (resolveFromPrematureEndOfDocument()) {
               return true;
            } else {
                qWarning() << errorString();
                ERROR0(errorString());
                return false;
            }
        }
    }
}

QXmlStreamAttributes XmlStreamParser::attributes()
{
    canProceedParsing();
    return _reader->attributes();
}

bool XmlStreamParser::isStartElement()
{
    if (canProceedParsing()) {
        return _reader->isStartElement();
    }
    return false;
}

bool XmlStreamParser::isEndElement()
{
    if (canProceedParsing()) {
        return _reader->isEndElement();
    }
    return false;
}

bool XmlStreamParser::atEnd()
{
    if (canProceedParsing()) {
        return _reader->atEnd();
    }
    return true;
}

QString XmlStreamParser::readElementText()
{
    if (canProceedParsing()) {
        return _reader->readElementText();
    }
    return "";
}

QString XmlStreamParser::name()
{
    if (canProceedParsing()) {
        return _reader->name().toString();
    }
    return "";
}

QString XmlStreamParser::namespaceUri()
{
    if (canProceedParsing()) {
        return _reader->namespaceUri().toString();
    }
    return "";
}

QString XmlStreamParser::qname()
{
    if (canProceedParsing()) {
        return _reader->qualifiedName().toString();
    }
    return "";
}

bool XmlStreamParser::readNextStartElement()
{
    if (canProceedParsing()) {
        return _reader->readNextStartElement();
    }
    return false;
}

void XmlStreamParser::skipCurrentElement()
{
    if (canProceedParsing()) {
        _reader->skipCurrentElement();
        //moveToEndOf(qname());
    }
}

void XmlStreamParser::readNext()
{
    if (canProceedParsing()) {
        _reader->readNext();
    }
}

bool XmlStreamParser::moveToEndOf(QString qName)
{
    bool found = false;
    while ( !(atEnd() || found)) {
        if (isEndElement()) {
            found = isAtEndOf(qName);
            if ( !found) {
                readNext();
            }
        } else {
            if (isStartElement()) {
                skipCurrentElement();
            }
            readNext();
        }
    }
    return found;
}

bool XmlStreamParser::moveToNext(QString qName, void (*callback)())
{
    if (atEnd()) {
        return false;
    }

    bool found = false;
    readNextStartElement();
    nextElementDo(callback);

    while ( !(atEnd() || found)) {
        if (isStartElement()) {
            found = isAtBeginningOf(qName);
            if ( !found) {
                skipCurrentElement();
                readNext();
            }
        } else {
            if ( !readNextStartElement()) {
                break;
            }
            nextElementDo(callback);
        }
    }
    return found;
}


bool XmlStreamParser::findNextOf(std::initializer_list<QString> elementList, void (*callback)())
{
    if (atEnd()) {
        return false;
    }

    bool found = false;
    QString startElement = name();
    bool onStartElement = isStartElement();
    bool hasInnerStartElement = readNextStartElement();
    if ( !onStartElement && !hasInnerStartElement) {
        return false;
    }
    nextElementDo(callback);

    while ( !(atEnd() || found)) {
        if (isStartElement()) {
            for (QString qName : elementList) {
                found = isAtBeginningOf(qName);
                if (found) {
                    break;
                }
            }
            if ( !found) {
                bool hasNextNestedElement = readNextStartElement();
                bool atEndOfStartElement = name() == startElement;
                if ( !hasNextNestedElement) {
                    if (atEndOfStartElement) {
                        break; // not found;
                    }
                    nextElementDo(callback);
                }
            }
        } else {
            bool hasNextNestedElement = readNextStartElement();
            bool atEndOfStartElement = name() == startElement;
            if ( !hasNextNestedElement) {
                if (atEndOfStartElement) {
                    break; // not found;
                }
                nextElementDo(callback);
            }
        }
    }
    return found;
}

bool XmlStreamParser::isAtBeginningOf(QString qName)
{
    if (atEnd() || !isStartElement()) {
        return false;
    }
    return isAtElement(qName);
}

bool XmlStreamParser::isAtEndOf(QString qName)
{
    if (atEnd() || !isEndElement()) {
        return false;
    }
    return isAtElement(qName);
}

bool XmlStreamParser::isAtElement(QString qName)
{
    QString prefix;
    int splitIndex = qName.indexOf(":");
    if (splitIndex > 0) {
        prefix = qName.left(splitIndex);
    }
    QString name = qName.mid(splitIndex + 1);

    if (_reader->namespaceProcessing()) {
        QString currentNamespace = namespaceUri();
        QString declaredNamespace = _namespaces[prefix];
        if (currentNamespace == declaredNamespace) {
            return this->name() == name;
        } else {
            return false;
        }
    } else {
        if (canProceedParsing() && _reader->qualifiedName().isEmpty()) {
            return this->name() == qName;
        } else {
            return this->qname() == qName;
        }
    }
}
