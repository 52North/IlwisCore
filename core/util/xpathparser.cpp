#include "kernel_global.h"
#include <QXmlQuery>

#include "xpathparser.h"

XPathParser::XPathParser(QIODevice *device): _iodevice(device)
{
    _query = new QXmlQuery;
}

XPathParser::~XPathParser()
{
    delete _query;
}

QXmlQuery *XPathParser::queryFromRoot(QString query)
{
    QString xPath(createXPathNamespaceDeclarations(_namespaces));
    xPath.append("doc($xml)").append(query);
    _query->bindVariable("xml", _iodevice);
    _query->setQuery(xPath);
    return _query;
}

QXmlQuery *XPathParser::queryRelativeFrom(QXmlItem &item, QString query)
{
    if (!_query) {
        _query = new QXmlQuery;
    }
    _query->setFocus(item);
    QString xPath(createXPathNamespaceDeclarations(_namespaces));
    xPath.append(query);
    _query->setQuery(xPath);
    return _query;
}

void XPathParser::addNamespaceMapping(QString prefix, QString ns)
{
    _namespaces[prefix] = ns;
}
