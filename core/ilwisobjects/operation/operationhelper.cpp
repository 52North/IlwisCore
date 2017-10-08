#include <QString>
#include <functional>
#include <future>
#include <memory>
#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "attributedefinition.h"
#include "raster.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "symboltable.h"
#include "catalog.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "proj4parameters.h"
#include "basetable.h"
#include "flattable.h"
#include "conventionalcoordinatesystem.h"
#include "numericdomain.h"
#include "itemdomain.h"
#include "ilwiscontext.h"
#include "operationhelper.h"

using namespace Ilwis;

OperationHelper::OperationHelper()
{
}

void OperationHelper::initialize(const IIlwisObject &inputObject, Ilwis::IIlwisObject &outputObject, IlwisTypes tp, quint64 what)
{
    if (inputObject->ilwisType() & itCOVERAGE) {
        ICoverage covInput = inputObject.as<Coverage>();
        ICoverage covOutput = outputObject.as<Coverage>();
        if ( what & itENVELOPE) {
            Envelope bounds = covInput->envelope();
            covOutput->envelope(bounds);
        }
        if ( what & itCOORDSYSTEM) {
            covOutput->coordinateSystem(covInput->coordinateSystem());
        }
        if ( hasType(what,itDOMAIN) && hasType(tp, itRASTER) ) {
            IRasterCoverage rasCoverageIn = inputObject.as<RasterCoverage>();
            IRasterCoverage rasCoverageOut = outputObject.as<RasterCoverage>();
            rasCoverageOut->datadefRef() = rasCoverageIn->datadef();
            rasCoverageOut->datadefRef(0) = rasCoverageIn->datadef();
            IDomain dom("integer");
            std::vector<double> stack{0.0};
            rasCoverageOut->stackDefinitionRef().setSubDefinition(dom, stack);

        }

        if ( hasType(what,itGEOREF) && hasType(tp, itRASTER) ) {
            IRasterCoverage rasCoverageIn = inputObject.as<RasterCoverage>();
            IRasterCoverage rasCoverageOut = outputObject.as<RasterCoverage>();
            rasCoverageOut->georeference(rasCoverageIn->georeference());
        }

        if ( hasType(what,itRASTERSIZE) && hasType(tp, itRASTER) ) {
            IRasterCoverage rasCoverageIn = inputObject.as<RasterCoverage>();
            IRasterCoverage rasCoverageOut = outputObject.as<RasterCoverage>();
            rasCoverageOut->size(rasCoverageIn->size());
        }

        if ( hasType(what,itTABLE)) {
            if ( hasType(tp, itRASTER)) {
                IRasterCoverage rasCoverageIn = inputObject.as<RasterCoverage>();
                IRasterCoverage rasCoverageOut = outputObject.as<RasterCoverage>();
                if ( !rasCoverageIn.isValid() || !rasCoverageOut.isValid())
                    return;
                if(rasCoverageIn->datadef().domain<>() != rasCoverageOut->datadef().domain<>())
                    return;
                rasCoverageOut->setAttributes(rasCoverageIn->attributeTable());
            }else { // we can only copy the structure of the table as the actual values are stored in the feature itself
                IFeatureCoverage outputFC = covOutput.as<FeatureCoverage>();
                IFeatureCoverage inputFC = covInput.as<FeatureCoverage>();
                for(int c = 0; c < inputFC->attributeDefinitions().columnCount(); ++c){
                    outputFC->attributeDefinitionsRef().addColumn(inputFC->attributeDefinitions().columndefinition(c));
                }
            }
        }
    }

}

IIlwisObject OperationHelper::variant2ilwisobject(const QVariant& variant, IlwisTypes tp){
    switch(tp)    {
        case itRASTER:
            return variant.value<IRasterCoverage>();
    case itFEATURE:
        return variant.value<IFeatureCoverage>();
    case itTABLE:
        return variant.value<ITable>();
    case itFLATTABLE:
        {
            IIlwisObject obj = variant.value<IFlatTable>();
            if (!obj.isValid()){
                // a bit a hack but the rules for these kind of cast from qvariant are very strict and it sometimnes difficult to predict which exact kind of table is hiding in there
                // the tp and pointer are correct but due to some copying the qvariant is not completely convinced
                obj = variant.value<ITable>();
            }
            return obj;
        }
    case itCOORDSYSTEM:
        return variant.value<ICoordinateSystem>();
    case itCONVENTIONALCOORDSYSTEM:
        return variant.value<IConventionalCoordinateSystem>();
    case itDOMAIN:
        return variant.value<IDomain>();
    case itNUMERICDOMAIN:
        return variant.value<INumericDomain>();
    case itITEMDOMAIN:
        return variant.value<IDomain>();
    case itGEOREF:
        return variant.value<IGeoReference>();
    default:
        {
            IIlwisObject obj;
            std::vector<IlwisTypes> aggregate = {itTABLE , itRASTER , itFEATURE , itGEOREF, itFLATTABLE , itCOORDSYSTEM , itCONVENTIONALCOORDSYSTEM ,
                                                 itDOMAIN , itITEMDOMAIN , itNUMERICDOMAIN};
            for(IlwisTypes specificType : aggregate){
                obj = OperationHelper::variant2ilwisobject(variant,specificType);
                if (obj.isValid())
                    return obj;
            }

        }
    }
    return IIlwisObject();
}

QString OperationHelper::unquote(const QString& name){
    if ( name.size() == 0)
        return name;

    if (name[0] == '\'' && name[name.size()-1] == '\''){
        return name.mid(1,name.size() - 2);
    }
    if (name[0] == '\"' && name[name.size()-1] == '\"'){
        return name.mid(1,name.size() - 2);
    }
    return name;
}

QString OperationHelper::variant2string(const QVariant& v, IlwisTypes tp){
    QMetaType::Type metatype = static_cast<QMetaType::Type>(v.type());
    bool inttype = metatype == QMetaType::Int ||  metatype == QMetaType::UInt ||
            metatype == QMetaType::LongLong ||  metatype == QMetaType::ULongLong ||
            metatype == QMetaType::Long ||  metatype == QMetaType::ULong ||
            metatype == QMetaType::Short || metatype == QMetaType::UShort ||
            metatype == QMetaType::Char || metatype == QMetaType::UChar;
    if ( inttype)
        return QString::number(v.toLongLong());

    if ( tp == itBOOL){
        QString b =  v.toString();
        return b;
    }
    if ( metatype == QMetaType::Double || metatype == QMetaType::Float)
        return QString::number(v.toDouble());
    if ( metatype == QMetaType::QString)
        return v.toString();

    if ( hasType(tp, itILWISOBJECT)){
        IIlwisObject obj = OperationHelper::variant2ilwisobject(v,tp);
        if ( obj.isValid()){
            return obj->resource().url()    .toString();
        }
    }
    if ( hasType(tp, itCOLUMN)){
        IIlwisObject obj = OperationHelper::variant2ilwisobject(v,itTABLE);
        if ( obj.isValid()){
            return obj->resource().url()    .toString();
        }
    }
    return "";
}

IlwisTypes OperationHelper::determineType(const QString &value)
{
    if ( value == "\"?\"")
        return itANY;

    IlwisTypes tp = IlwisObject::findType(value);

    if ( tp != itUNKNOWN)
        return tp;

    QString s = context()->workingCatalog()->resolve(value);
    IlwisTypes type = IlwisObject::findType(s) ;
    if ( type != itUNKNOWN)
        return type;

    quint64 id = IlwisObject::internalname2id(value);
    if ( id != i64UNDEF){
        ESPIlwisObject obj =  mastercatalog()->get(id);
        if ( obj.get() != 0)
            return obj->ilwisType();
    }
    tp = TypeHelper::variant2type(value);

    return tp == itUNKNOWN ? itSTRING : tp;
}
