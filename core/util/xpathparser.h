#ifndef XPATHPARSER_H
#define XPATHPARSER_H

#include "kernel_global.h"

#include <memory>
#include <QXmlQuery>
#include <QXmlItem>
#include <QIODevice>
#include <QMapIterator>

namespace Ilwis {

typedef std::unique_ptr<QXmlQuery> UPXmlQuery;

class KERNELSHARED_EXPORT XPathParser
{
public:
    XPathParser(QIODevice *device);
    ~XPathParser();

    QString createXPathNamespaceDeclarations(QMap<QString, QString> &mappings)
    {
        QString declarationString;
        QMapIterator<QString,QString> iterator(mappings);
        while(iterator.hasNext()) {
            iterator.next();
            QString declaration = QString("declare namespace %1 = \"%2\"; \n ").arg(iterator.key(), iterator.value());
            declarationString.push_back(declaration);
        }
        return declarationString;
    }


    UPXmlQuery& queryFromRoot(QString query);
    UPXmlQuery& queryRelativeFrom(const QXmlItem &item, const QString &query);
    void addNamespaceMapping(QString prefix, QString ns);

private:
    UPXmlQuery _query;
    QIODevice *_iodevice;
    QMap<QString,QString> _namespaces;

};

typedef std::unique_ptr<XPathParser> UPXPathParser;

}

#endif // XPATHPARSER_H
