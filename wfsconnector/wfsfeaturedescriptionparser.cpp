#include <QUrl>
#include <QXmlStreamAttributes>
#include <initializer_list>

#include "kernel.h"
#include "coverage.h"
#include "ilwis.h"
#include "ilwisdata.h"
#include "domain.h"
#include "numericdomain.h"
#include "numericrange.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "attributedefinition.h"
#include "table.h"
#include "domainitem.h"
#include "itemdomain.h"
#include "textdomain.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "coverage.h"
#include "featurecoverage.h"
#include "feature.h"
#include "featureiterator.h"

#include "wfsresponse.h"
#include "xmlstreamparser.h"
#include "wfsparsingcontext.h"
#include "wfsfeaturedescriptionparser.h"
#include "wfsutils.h"

using namespace Ilwis;
using namespace Wfs;

WfsFeatureDescriptionParser::WfsFeatureDescriptionParser()
{
}


WfsFeatureDescriptionParser::WfsFeatureDescriptionParser(SPWfsResponse response)
{
    _parser = new XmlStreamParser(response->device());
    _parser->addNamespaceMapping("xsd", "http://www.w3.org/2001/XMLSchema");
}

WfsFeatureDescriptionParser::~WfsFeatureDescriptionParser()
{
}

bool WfsFeatureDescriptionParser::parseMetadata(FeatureCoverage *fcoverage, WfsParsingContext &context)
{
    qDebug() << "WfsFeatureDescriptionParser::parseMetadata()";

    WfsUtils::addSpatialMetadata(fcoverage, context.resource());

    if (_parser->startParsing("xsd:schema")) {
        parseNamespaces(context);
        while ( !_parser->atEnd()) {
            if (_parser->readNextStartElement()) {
                if (_parser->isAtBeginningOf("xsd:complexType")) {
                    parseFeatureProperties(fcoverage, context);
                } else if (_parser->isAtBeginningOf("xsd:element")) {
                    QStringRef typeName = _parser->attributes().value("name");
                    context.setFeatureType(typeName.toString());
                }
            }
        }
    }

    return true;
}

void WfsFeatureDescriptionParser::parseNamespaces(WfsParsingContext &context)
{
    qDebug() << "WfsFeatureDescriptionParser::parseNamespaces()";

    for (QXmlStreamAttribute attribute : _parser->attributes()) {
        QString name = attribute.name().toString();
        QString value = attribute.value().toString();
        if (name == "targetNamespace") {
            context.addNamespaceMapping("", value);
            break; // TODO: consider how parse further namespaces
        } /*else {
            QString prefix = attribute.prefix().toString();
            QString parsedPrefix = name.left(name.indexOf(":"));
            wfsSchemaInfo.addNamespaceMapping(prefix, value); // does not work
        }*/
    }
}

void WfsFeatureDescriptionParser::parseFeatureProperties(FeatureCoverage *fcoverage, WfsParsingContext &context)
{
    FeatureAttributeDefinition &fad = fcoverage->attributeDefinitionsRef();
    if (_parser->findNextOf( { "xsd:complexContent" } )) {
        if (_parser->findNextOf( { "xsd:extension" } )) {
            if (_parser->findNextOf( { "xsd:sequence" } )) {
                if (_parser->findNextOf( { "xsd:element" } )) {

                    /*
                     * Parse schema elements to table colummns to obtain feature
                     * specific knowledge, e.g. namespaces
                     */

                    do {
                        QXmlStreamAttributes attributes = _parser->attributes();
                        QString name = attributes.value("name").toString();
                        QString type = attributes.value("type").toString();

                        if (type.startsWith("gml")) {
                            fcoverage->featureTypes(WfsUtils::getGeometryType(type));
                            context.setGeometryAtttributeName(name);
                        } else {
                            IDomain domain;
                            if (initDomainViaType(type, domain)) {
                                fad.addColumn(name, domain);
                            }
                        }
                        _parser->skipCurrentElement(); // move to end
                    } while (_parser->findNextOf( { "xsd:element"} ));
                }
            }
        }
    }
}

bool WfsFeatureDescriptionParser::initDomainViaType(QString &type, IDomain &domain)
{
    type = type.mid(type.indexOf(":") + 1);
    if (type == "double" || type == "float" || type == "decimal") {
        INumericDomain ndomain;
        ndomain.prepare("value");
        domain = ndomain;
        return true;
    }
    if (type == "int" || type == "unsignedInt" || type.contains("Integer") || type == "unsignedShort") {
        INumericDomain ndomain;
        ndomain.prepare("integer");
        domain = ndomain;
        return domain.isValid();
    }
    if (type == "long" || type == "unsignedLong") {
        INumericDomain ndomain;
        ndomain.prepare("integer");
        domain = ndomain;
        return domain.isValid();
    }
    if (type == "string" || type == "token" || type == "normalizedString") {
        domain.prepare("code=domain:text", itTEXTDOMAIN);
        return domain.isValid();
    }
    if (type == "boolean") {
        domain.prepare("boolean", itBOOL);
        return domain.isValid();
    }
    if (type == "date") {
        ITimeDomain tdomain;
        tdomain.prepare();
        tdomain->range(new TimeInterval(itDATE));
        domain = tdomain;
        return domain.isValid();
    }
    if (type == "dateTime") {
        ITimeDomain tdomain;
        tdomain.prepare();
        tdomain->range(new TimeInterval(itDATETIME));
        domain = tdomain;
        return domain.isValid();
    }

    // -------------------------------------------------

    /*
     * NOTE: there are some more xsd:* types possible.
     * Some make sense, some not ...
     *
     * see:
     * http://infohost.nmt.edu/tcc/help/pubs/rnc/xsd.html
     *
     * Types which makes sense to be addded here
     */

    // anyURI
    // base64Binary => decode to itBINARY
    // duration => decode to timeinterval with valid bounds
    // gDay, gMonth, gMonthDay, gYear, gYearMonth
    // ID
    // hexBinary


    /*
     * types which makes less sense to be addded here
     */

    // byte
    // IDREF, IDREFS
    // Name, NCName
    // NMTOKEN, NMTOKENS
    // QName
    // unsignedByte

    // -------------------------------------------------

    ERROR1(TR("Could not create domain for schema type: %1"), type);
    return false;
}

