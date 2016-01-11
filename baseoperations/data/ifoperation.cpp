#include <QString>
#include <functional>
#include <future>
#include "kernel.h"
#include "raster.h"
#include "symboltable.h"
#include "pixeliterator.h"
#include "ilwisoperation.h"
#include "ifoperation.h"

using namespace Ilwis;
using namespace BaseOperations;


IfOperation::IfOperation()
{
}

IfOperation::IfOperation(quint64 metaid, const Ilwis::OperationExpression &expr) : OperationImplementation(metaid, expr)
{
}

DataDefinition IfOperation::findOutputDataDef(const OperationExpression &expr ) {
    IDomain dm;
    QString domName = expr.parm(0,false).domain();
    if ( domName != sUNDEF && domName != "") {
        if( dm.prepare(domName))
            return DataDefinition(dm);
    }
    DataDefinition def1 = findParameterDataDef(expr,1);
    DataDefinition def2 = findParameterDataDef(expr,2);

    return DataDefinition::merge(def1,def2);
}

DataDefinition IfOperation::findParameterDataDef(const OperationExpression &expr, int index)  {
    const Parameter& parm = expr.parm(index);
    DataDefinition def;
    QString parmvalue = parm.value().toLower();

    quint64 gcid = mastercatalog()->name2id(parmvalue, itRASTER);
    if ( gcid != i64UNDEF) {
        IRasterCoverage cov;
        if(cov.prepare(gcid)) {
            def = cov->datadef();
            _coverages[index - 1] = cov.as<Coverage>();
        }
    } else {
        bool ok;
        _number[index - 1] =parmvalue.toDouble(&ok);
        if ( ok){
            IDomain dom("code=domain:value");
            def.domain(dom);
        } else {
            std::vector<QString> bools = {"true","false","yes","no","?"};
            auto iter = std::find(bools.begin(), bools.end(), parmvalue.toLower());
            if ( iter != bools.end()) {
                int v = 0;
                if ( parmvalue == "?" )
                    v= 2;
                else if ( parmvalue == "yes" || parmvalue == "true")
                    v = 1;
                _number[index - 1] = v;
                IDomain dm;
                dm.prepare("boolean");
                def.domain(dm);
            }
        }
    }
    return def;
}
