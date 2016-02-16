#include "kernel.h"
#include "ilwisdata.h"
#include "domain.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "raster.h"
#include "createrastercoverage.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(CreateRasterCoverage)

CreateRasterCoverage::CreateRasterCoverage()
{

}

CreateRasterCoverage::CreateRasterCoverage(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{

}

bool CreateRasterCoverage::execute(ExecutionContext *ctx, SymbolTable &symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    IRasterCoverage raster;
    raster.prepare();
    raster->georeference(_grf);
    raster->size( Size<>(_grf->size().xsize(), _grf->size().ysize(), _bands));
    raster->setDescription(_desc);
    if ( _keywords != "")
            raster->resourceRef()["keywords"] = _keywords;
    std::vector<double> bands(_bands);
    for(int band = 0; band < _bands; ++band){
        bands[band] = band;
    }
    raster->setDataDefintions(_domain,bands);

    QVariant value;
    value.setValue<IRasterCoverage>(raster);
    ctx->setOutput(symTable,value,raster->name(),itRASTER,raster->resource());

    return true;
}

Ilwis::OperationImplementation *CreateRasterCoverage::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
    return new CreateRasterCoverage(metaid, expr);
}

Ilwis::OperationImplementation::State CreateRasterCoverage::prepare(ExecutionContext *ctx, const SymbolTable &)
{
    QString grf = _expression.input<QString>(0);
    _grf.prepare(grf);
    if ( !_grf.isValid()){
        kernel()->issues()->log(QString(TR("%1 is and invalid georeference")).arg(grf));
        return sPREPAREFAILED;
    }
    QString dom = _expression.input<QString>(1);
    _domain.prepare(dom);
    if ( !_domain.isValid()){
        kernel()->issues()->log(QString(TR("%1 is and invalid domain")).arg(dom));
        return sPREPAREFAILED;
    }
    _bands = _expression.input<int>(2);
    if ( _bands <= 0){
        kernel()->issues()->log(QString(TR("%1 is and value for number of bands")).arg(dom));
        return sPREPAREFAILED;
    }
    _desc = _expression.input<QString>(3);
    _desc = _desc.remove('\"');
    if ( _expression.parameterCount() == 5){
        _keywords = _expression.input<QString>(4);
        _keywords = _keywords.remove('\"');
    }
    return sPREPARED;
}

quint64 CreateRasterCoverage::createMetadata()
{
    OperationResource resource({"ilwis://operations/createrastercoverage"});
    resource.setLongName("Create Raster Coverage");
    resource.setSyntax("createrastercoverage(georeference, domain, bands,description[, keywords])");
    resource.setInParameterCount({4,5});
    resource.addInParameter(0, itGEOREF|itSTRING,TR("Georeference"), TR("Geometry of the new rastercoverage, coordinatesystem and pixel size"));
    resource.addInParameter(1, itDOMAIN|itSTRING,TR("Domain"), TR("Domain used by the raster coverage"));
    resource.addInParameter(2, itINTEGER,TR("Number of bands"), TR("Number of bands in the rastercoverage"));
    resource.addInParameter(3, itSTRING,TR("Description"), TR("Extra information about the new domain"));
    resource.addOptionalInParameter(4, itSTRING, TR("Keywords"), TR("optional list of comma seperated keywords"));
    resource.setOutParameterCount({1});
    resource.addOutParameter(0, itITEMDOMAIN, TR("raster coverage"), TR("The newly created raster"));
    resource.setKeywords("rastercoverage, create");

    mastercatalog()->addItems({resource});
    return resource.id();
}



