#include "coverage.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "raster.h"
#include "range.h"
#include "itemrange.h"
#include "identifieritem.h"
#include "identifierrange.h"
#include "itemdomain.h"
#include "colorlookup.h"
#include "representation.h"
#include "drawers/attributevisualproperties.h"
#include "rootdrawer.h"
#include "spatialdatadrawer.h"

using namespace Ilwis;
using namespace Geodrawer;

SpatialDataDrawer::SpatialDataDrawer(const QString &name, DrawerInterface *parentDrawer, RootDrawer *rootdrawer, const IOOptions &options) : ComplexDrawer(name, parentDrawer, rootdrawer, options)
{
}

std::vector<double> SpatialDataDrawer::numericAttributeValues(const QString &attribute) const
{
    if (_coverage.isValid()){
        if ( hasType(_coverage->ilwisType(), itFEATURE)){
            IFeatureCoverage features = _coverage.as<FeatureCoverage>();
            auto columnValues = features->attributeTable()->column(attribute);
            std::vector<double> attribValues(columnValues.size());
            int count = 0;
            for(const auto& value : columnValues){
                bool ok;
                const double v = value.toDouble(&ok);
                attribValues[count++] = ok ? v : rUNDEF;
            }
            return attribValues;
        }
        return std::vector<double>();
    }
    return std::vector<double>();
}

ICoverage SpatialDataDrawer::coverage() const
{
    return _coverage;
}

void SpatialDataDrawer::coverage(const ICoverage &cov)
{
    _coverage = cov;
    _envelope = cov->envelope();
}

Envelope SpatialDataDrawer::envelope() const
{
    return _envelope;
}

void SpatialDataDrawer::envelope(const Envelope &env)
{
    _envelope = env;
}

VisualAttribute SpatialDataDrawer::vattributeFeatureCoverage(const QString &attrName) const
{
    VisualAttribute attr;
    IFeatureCoverage features = coverage().as<FeatureCoverage>();
    if ( !features.isValid()){
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"FeatureCoverage", TR("Visualization"));
    }else {
        int columnIndex = features->attributeDefinitions().columnIndex(attrName);
        if ( columnIndex == iUNDEF){ // test a number of fallbacks to be able to show at least something
            columnIndex = features->attributeDefinitions().columnIndex(COVERAGEKEYCOLUMN);
            if ( columnIndex == iUNDEF){
                columnIndex = features->attributeDefinitions().columnIndex(FEATUREVALUECOLUMN);
                if ( columnIndex == iUNDEF){ // default to a indexeditemdomain
                    IIndexedIdDomain itemdom;
                    itemdom.prepare();
                    IndexedIdentifierRange *rng = new IndexedIdentifierRange("feature", features->featureCount());
                    itemdom->range(rng);
                    attr = VisualAttribute(itemdom);
                }else {
                    attr =  visualAttribute(FEATUREVALUECOLUMN);
                }
            }else {
                attr =  visualAttribute(COVERAGEKEYCOLUMN);
            }
        }
        attr.setColumnIndex(columnIndex);
    }
    return attr;
}

VisualAttribute SpatialDataDrawer::vattributeRasterCoverage(const QString &attrName) const
{
    //TODO later when displaying rasters with attributes
    VisualAttribute attr;
    IRasterCoverage features = coverage().as<RasterCoverage>();
    if ( !features.isValid()){
        ERROR2(ERR_COULDNT_CREATE_OBJECT_FOR_2,"FeatureCoverage", TR("Visualization"));
    }else {

    }
    return attr;
}

VisualAttribute SpatialDataDrawer::visualAttribute(const QString &attrName) const
{
    auto iter = _visualProperties.find(attrName)    ;
    if ( iter != _visualProperties.end())
        return iter->second;
    if ( hasType(coverage()->ilwisType(), itFEATURE)){
        return vattributeFeatureCoverage(attrName);
    } else if ( hasType(coverage()->ilwisType(), itRASTER)){
        return vattributeRasterCoverage(attrName);
    }
    return VisualAttribute();
}

void SpatialDataDrawer::visualAttribute(const QString &attrName, const VisualAttribute &properties)
{
    _visualProperties[attrName] = properties;
}

std::vector<QVariant> SpatialDataDrawer::attributes(const QString &keys) const
{
    std::vector<QVariant> results = ComplexDrawer::attributes(keys);

    QStringList parts = keys.split("|");
    for(QString& part : parts)
        part.toLower();


    for(const QString& part : parts){
        if ( part == "visualattributenames"){
            for(auto pair : _visualProperties){
                results.push_back(pair.first);
            }
        }else {
            results.push_back(attribute(part));
        }
    }
    return results;
}

bool SpatialDataDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options)
{
    if(!ComplexDrawer::prepare(prepType, options))
        return false;

    if ( hasType(prepType, DrawerInterface::ptMVP) && !isPrepared(DrawerInterface::ptMVP) ){

        if (!_shaders.bind())
            return false;
        _shaders.setUniformValue("mvp",rootDrawer()->mvpMatrix());
        _shaders.enableAttributeArray("mvp");

        _prepared |= DrawerInterface::ptMVP;
        _shaders.release();
    }
    return true;
}

bool SpatialDataDrawer::isVisualAttribute(const QString &attName) const
{
    if ( coverage()->ilwisType() == itRASTER){
        if ( attName == PIXELVALUE)
            return true;
    }
    for(auto pair : _visualProperties){
        if ( pair.first == attName)
            return true;
    }

    return false;
}

QVariant SpatialDataDrawer::attribute(const QString &key) const
{
    QVariant var = ComplexDrawer::attribute(key);
    if ( var.isValid())
        return var;

    if ( key.indexOf("visualattribute") == 0){
        QStringList parts = key.split("|");
        if ( parts.size() == 3){
            if ( parts[1] == "representation")
                var.setValue<IRepresentation>(visualAttribute(parts[2]).representation());
            else if ( parts[1] == "stretchrange"){
                var.setValue<NumericRange>(visualAttribute(parts[2]).stretchRange());
            } else if ( parts[1] == "domain"){
                var.setValue<IDomain>(visualAttribute(parts[2]).domain());
            }
        }
    }
    else if ( key == "coverage"){
        var.setValue<ICoverage>(coverage());
    }
    else if ( key == "envelope"){
        var.setValue<Envelope>(envelope());
    }
    return var;
}

void SpatialDataDrawer::setAttribute(const QString &attrName, const QVariant &attrib)
{
    ComplexDrawer::setAttribute(attrName, attrib);
}




