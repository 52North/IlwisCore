
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
    _loop = new QEventLoop();
    connect(_device, SIGNAL(readyRead()), this, SLOT(readIncomingData()));
    connect(_device, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
    _reader = new QXmlStreamReader();
}

XmlStreamParser::~XmlStreamParser()
{
    _loop->deleteLater();
}

void XmlStreamParser::setXmlReader(QXmlStreamReader *reader)
{
    _reader = reader;
}

void XmlStreamParser::bytesWritten(qint64 bytes) {
    qDebug() << "bytes written: " << bytes;
    _reader->addData(_device->read(bytes));
    _loop->quit();
    qDebug() << "... resume parsing.";
}

void XmlStreamParser::readIncomingData() {
    qDebug() << "readIncomingData()";
}


QXmlStreamReader *XmlStreamParser::reader() const
{
    return _reader;
}

void XmlStreamParser::addNamespaceMapping(QString prefix, QString ns)
{
    _namespaces[prefix] = ns;
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

bool XmlStreamParser::onError()
{
    if (_reader->error() == QXmlStreamReader::PrematureEndOfDocumentError) {
        qDebug() << "Wait for new data ...";
        _loop->exec();
        return false;
    } else {
        ERROR0(_reader->errorString());
        return true;
    }
}

bool XmlStreamParser::hasError() {
    return _reader->hasError();
}

bool XmlStreamParser::canProceedParsing()
{
    return !hasError() || ( hasError() && !onError() );
}

QXmlStreamAttributes XmlStreamParser::attributes()
{
    return _reader->attributes();
}

bool XmlStreamParser::atEnd()
{
    return _reader->atEnd();
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
        if (_reader->isEndElement()) {
            readNext();
        }
        return _reader->readNextStartElement();
    }
    return false;
}

void XmlStreamParser::skipCurrentElement()
{
    if (canProceedParsing()) {
        _reader->skipCurrentElement();
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
    while ( !(_reader->atEnd() || found)) {
        if (_reader->isEndElement()) {
            found = isAtEndOf(qName);
            if ( !found) {
                readNext();
            }
        } else {
            if (_reader->isStartElement()) {
                skipCurrentElement();
            }
            readNext();
        }
    }
    return found;
}

bool XmlStreamParser::moveToNext(QString qName, void (*callback)())
{
    if (_reader->atEnd()) {
        return false;
    }

    bool found = false;
    readNextStartElement();
    nextElementDo(callback);

    while ( !(_reader->atEnd() || found)) {
        if (_reader->isStartElement()) {
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
    if (_reader->atEnd()) {
        return false;
    }

    bool found = false;
    QString startElement = name();
    bool onStartElement = _reader->isStartElement();
    bool hasInnerStartElement = readNextStartElement();
    if ( !onStartElement && !hasInnerStartElement) {
        return false;
    }
    nextElementDo(callback);

    while ( !(_reader->atEnd() || found)) {
        if (_reader->isStartElement()) {
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
    if (_reader->atEnd() || !_reader->isStartElement()) {
        return false;
    }
    return isAtElement(qName);
}

bool XmlStreamParser::isAtEndOf(QString qName)
{
    if (_reader->atEnd() || !_reader->isEndElement()) {
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
        if (_reader->qualifiedName().isEmpty()) {
            return this->name() == qName;
        } else {
            return this->qname() == qName;
        }
    }
}
