#include "kernel_global.h"
#include <QXmlQuery>

#include "xpathparser.h"

using namespace Ilwis;

XPathParser::XPathParser(QIODevice *device): _iodevice(device)
{
}

XPathParser::~XPathParser()
{
}

UPXmlQuery &XPathParser::queryFromRoot(QString query)
{
    if ( !_query)
        _query.reset(new QXmlQuery);
    QString xPath(createXPathNamespaceDeclarations(_namespaces));
    xPath.append("doc($xml)").append(query);
    _query->bindVariable("xml", _iodevice);
    _query->setQuery(xPath);
    return _query;
}

UPXmlQuery &XPathParser::queryRelativeFrom(const QXmlItem &item, const QString &query)
{
    if ( !_query)
        _query.reset(new QXmlQuery);
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
