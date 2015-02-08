#include "coverage.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "colorlookup.h"
#include "representation.h"
#include "attributevisualproperties.h"
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

AttributeVisualProperties SpatialDataDrawer::visualAttribute(const QString &attrName) const
{
    auto iter = _visualProperties.find(attrName)    ;
    if ( iter != _visualProperties.end())
        return iter->second;
    return AttributeVisualProperties();
}

void SpatialDataDrawer::visualAttribute(const QString &attrName, const AttributeVisualProperties &properties)
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

bool SpatialDataDrawer::prepare(DrawerInterface::PreparationType prepType, const IOOptions &options, QOpenGLContext *openglContext)
{
    if(!ComplexDrawer::prepare(prepType, options, openglContext))
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

}




