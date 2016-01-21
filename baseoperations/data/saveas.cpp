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

    return true;
}

Ilwis::OperationImplementation *SaveAs::create(quint64 metaid, const Ilwis::OperationExpression &expr)
{
        return new SaveAs(metaid, expr);
}

Ilwis::OperationImplementation::State SaveAs::prepare(ExecutionContext *, const SymbolTable&)
{

    QString objectname = _expression.parm(0).value();
    if (!_inputObject.prepare(objectname)) {
        ERROR2(ERR_COULD_NOT_LOAD_2,objectname,"");
        return sPREPAREFAILED;
    }
    _outputUrl = QUrl(_expression.input<QString>(1));
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
    operation.addInParameter(0,itRASTER|itFEATURE|itTABLE , TR("input object"),TR("Raster, feature or table"));
    operation.addInParameter(1,itSTRING, "Output url");
    operation.addInParameter(2,itSTRING, TR("format"), TR("format code as determined by the provider"));
    operation.addInParameter(3,itSTRING, TR("provider"), TR("provider/library that will translate the object"));
    operation.setOutParameterCount({0});
    operation.setKeywords("export");

    mastercatalog()->addItems({operation});
    return operation.id();

}


