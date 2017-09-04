#ifndef WFSFEATUREDESCRIPTIONPARSER_H
#define WFSFEATUREDESCRIPTIONPARSER_H

#include "wfsconnector_global.h"

class QUrl;
class QString;
class XmlStreamParser;

namespace Ilwis {
namespace Wfs {

class WfsResponse;

class WFSCONNECTORSHARED_EXPORT WfsFeatureDescriptionParser
{
public:
    WfsFeatureDescriptionParser();
    WfsFeatureDescriptionParser(SPWfsResponse response);
    ~WfsFeatureDescriptionParser();

    bool parseMetadata(FeatureCoverage *fcoverage, WfsParsingContext &context);

private:
    XmlStreamParser *_parser;

    void parseNamespaces(WfsParsingContext &context);
    void parseFeatureProperties(FeatureCoverage *fcoverage, WfsParsingContext &context);

    /**
     * Initiates the domain according to the given xml schema type.
     *
     * @param type the schema type.
     * @param domain the (empty) domain to initiate.
     * @return true if domain could be initiated, false otherwise.
     */
    bool initDomainViaType(QString &type, IDomain &domain);

};

}
}
#endif // WFSFEATUREDESCRIPTIONPARSER_H
