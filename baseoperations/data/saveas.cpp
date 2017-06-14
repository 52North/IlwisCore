#include "kernel.h"
#include "ilwisdata.h"
#include "symboltable.h"
#include "geometries.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "commandhandler.h"
#include "operation.h"
#include "tranquilizer.h"
#include "dataformat.h"
#include "catalog.h"
#include "ilwiscontext.h"
#include "saveas.h"

using namespace Ilwis;
using namespace BaseOperations;

REGISTER_OPERATION(SaveAs)

SaveAs::SaveAs()
{
}

SaveAs::SaveAs(quint64 metaid, const Ilwis::OperationExpression &expr) :
    OperationImplementation(metaid, expr)
{
}

bool SaveAs::execute(ExecutionContext *ctx, SymbolTable& symTable)
{
    if (_prepState == sNOTPREPARED)
        if((_prepState = prepare(ctx, symTable)) != sPREPARED)
            return false;
    _inputObject->connectTo(_outputUrl,_format,_provider,IlwisObject::cmOUTPUT);
    _inputObject->store();
    mastercatalog()->addContainer(_inputObject->resource(IlwisObject::cmOUTPUT).container() , true);
    logOperation( _expression);
    return true;
}

Ilwis::OperationImplementation *SaveAs::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
        return new SaveAs(metaid, expr);
}

Ilwis::OperationImplementation::State SaveAs::prepare(ExecutionContext *ctx, const SymbolTable &st)
{
    OperationImplementation::prepare(ctx,st);
    QString objectname = _expression.parm(0).value();
    objectname.remove("\"");
    if (!_inputObject.prepare(objectname,{"mustexist",true})) {
        ERROR2(ERR_COULD_NOT_LOAD_2,objectname,"");
        return sPREPAREFAILED;
    }
    QString  path = _expression.input<QString>(1);
    if ( path.indexOf("://") == -1){
        path = context()->workingCatalog()->resource().url().toString() + "/" + path;
    }
    _outputUrl = QUrl(path);
    _format = _expression.input<QString>(2);
    _provider = _expression.input<QString>(3);
    return sPREPARED;
}

quint64 SaveAs::createMetadata()
{

    OperationResource operation({"ilwis://operations/saveas"});
    operation.setSyntax("saveas(inputobject,url,outputformat, outputprovider)");
    operation.setDescription(TR("saves the object in a certain format"));
    operation.setInParameterCount({4});
    operation.addInParameter(0,itRASTER|itFEATURE|itTABLE|itWORKFLOW, TR("Input object"),TR("Raster, feature, table or workflow"));
    operation.addInParameter(1,itSTRING, TR("Output url"), TR("Destination url for output"));
    operation.addInParameter(2,itSTRING, TR("Format"), TR("format code as determined by the provider"));
    operation.addInParameter(3,itSTRING, TR("Provider"), TR("provider/library that will translate the object"));
    operation.setOutParameterCount({0});
    operation.setKeywords("export,format");

    mastercatalog()->addItems({operation});
    return operation.id();

}


