#include <QString>
#include <functional>
#include <future>
#include <memory>
#include "geos/geom/Geometry.h"
#include "kernel.h"
#include "coverage.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "operationhelper.h"
#include "operationhelperfeatures.h"
#include "geometryhelper.h"
#include "commandhandler.h"
#include "featureiterator.h"
#include "contains.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(Contains)

Contains::Contains()
{
}


Ilwis::BaseOperations::Contains::Contains(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool Ilwis::BaseOperations::Contains::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;

    IFeatureCoverage features = _coverage.as<FeatureCoverage>();

    std::vector<quint32> result;
    int index = 0;
    for(auto& iter : features){
        int geomcount = iter->trackSize();
        for(int gindex = 0; gindex < geomcount; ++gindex){
            if ( _containsWKT){
                if ((iter->geometry(gindex)->contains(_geometry.get()))){
                    result.push_back(index);
                }
            }else {
                if (_geometry->contains(iter->geometry(gindex).get())){
                    result.push_back(index);
                }
            }
        }
        ++index;
    }

    if ( ctx != 0) {
        QVariant value;
        value.setValue<std::vector<quint32>>(result);
        ctx->setOutput(symTable, value, sUNDEF, itINTEGER | itCOLLECTION, Resource());
    }

    return true;
}

Ilwis::OperationImplementation::State Contains::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString fc = _expression.parm(0).value();

    if (!_coverage.prepare(fc)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,fc,"");
        return sPREPAREFAILED;
    }

    QString order = _expression.parm(1).value().toLower();
    _containsWKT =  order == "yes" || order == "true";

    QString quotedGeom = _expression.parm(2).value();
    QString geom = quotedGeom.remove('\"');
    _geometry.reset(GeometryHelper::fromWKT(geom));
    if ( !_geometry){
        ERROR2(ERR_NO_INITIALIZED_2, TR("Geometry"), TR("Contains operation"));
        return sPREPAREFAILED;
    }

    return sPREPARED;

}

Ilwis::OperationImplementation *Ilwis::BaseOperations::Contains::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new Contains(metaid, expr);
}

quint64 Ilwis::BaseOperations::Contains::createMetadata()
{
    OperationResource operation({"ilwis://operations/contains"});
    operation.setSyntax("contains(coverage,secondInFirst,wkt-defintion)");
    operation.setDescription(TR("returns either a coverage or a feature-index of all features that satisfy the contains constraint"));
    operation.setInParameterCount({3});
    operation.addInParameter(0,itCOVERAGE, TR("coverage"),TR("coverage on which the containment operation is applicable"));
    operation.addInParameter(1,itBOOL, TR("contains Order"), TR("either the wkt geometry defintion is contained in the geometry of the coverage(true) or vice-versa"));
    operation.addInParameter(2,itSTRING, TR("Geometry"), TR("the geometry for which the contains musy held true"));

    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itCOLLECTION | itINTEGER, TR("result index collection"));
    operation.setKeywords("geometry, coordinate, coverage");

    mastercatalog()->addItems({operation});
    return operation.id();
}
