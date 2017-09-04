
#include <QMap>
#include <QString>
#include <QXmlStreamReader>
#include <initializer_list>

#include "xmlstreamparser.h"



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

XmlStreamParser::XmlStreamParser(QIODevice *device)
{
    _reader = new QXmlStreamReader(device);
}

void XmlStreamParser::setXmlReader(QXmlStreamReader *reader)
{
    _reader = reader;
}

QXmlStreamReader *XmlStreamParser::reader() const
{
    return _reader;
}

void XmlStreamParser::addNamespaceMapping(QString prefix, QString ns)
{
    _namespaces[prefix] = ns;
}

QString XmlStreamParser::getPrefixForNamespaceUri(QString namespaceUri) const
{
    return _namespaces.key(namespaceUri);
}

bool XmlStreamParser::startParsing(QString qName) const
{
    if ( !_reader->isStartDocument()) {
      _reader->readNext();
    }
    _reader->readNextStartElement();
    return isAtBeginningOf(qName);
}

bool XmlStreamParser::hasError() const
{
    return _reader->hasError();
}

QXmlStreamAttributes XmlStreamParser::attributes() const
{
    return _reader->attributes();
}

bool XmlStreamParser::atEnd() const
{
    return _reader->atEnd();
}

QString XmlStreamParser::readElementText() const
{
    return _reader->readElementText();
}

QString XmlStreamParser::name() const
{
    return _reader->name().toString();
}

QString XmlStreamParser::namespaceUri() const
{
    return _reader->namespaceUri().toString();
}

QString XmlStreamParser::qname() const
{
    return _reader->qualifiedName().toString();
}

bool XmlStreamParser::readNextStartElement() const
{
//    if (_reader->isEndElement()) {
//        _reader->readNext();
//    }
    return _reader->readNextStartElement();
}

void XmlStreamParser::skipCurrentElement() const
{
    _reader->skipCurrentElement();
}

void XmlStreamParser::readNext() const
{
    _reader->readNext();
}

bool XmlStreamParser::moveToEndOf(QString qName)
{
    bool found = false;
    while ( !(atEnd() || found)) {
        if (_reader->isEndElement()) {
            found = isAtEndOf(qName);
            if ( !found) {
                _reader->readNext();
            }
        } else {
            if (_reader->isStartElement()) {
                skipCurrentElement();
            }
            _reader->readNext();
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
    if (atEnd()) {
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

    while ( !(atEnd() || found)) {
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

bool XmlStreamParser::isAtBeginningOf(QString qName) const
{
    if (atEnd() || !_reader->isStartElement()) {
        return false;
    }
    return isAtElement(qName);
}

bool XmlStreamParser::isAtEndOf(QString qName) const
{
    if (atEnd() || !_reader->isEndElement()) {
        return false;
    }
    return isAtElement(qName);
}

bool XmlStreamParser::isAtElement(QString qName) const
{
    QString prefix;
    int splitIndex = qName.indexOf(":");
    if (splitIndex > 0) {
        prefix = qName.left(splitIndex);
    }
    QString name = qName.mid(splitIndex + 1);

    if (_reader->namespaceProcessing()) {
        QString currentNamespace = _reader->namespaceUri().toString();
        QString declaredNamespace = _namespaces[prefix];
        if (currentNamespace == declaredNamespace) {
            return name == this->name();
        } else {
            return false;
        }
    } else {
        if (_reader->qualifiedName().isEmpty()) {
            return _reader->name().toString() == qName;
        } else {
            return _reader->qualifiedName().toString() == qName;
        }
    }
}
