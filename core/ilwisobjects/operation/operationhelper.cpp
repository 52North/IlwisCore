#include <QString>
#include <functional>
#include <future>
#include <memory>
#include "kernel.h"
#include "coverage.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "polygon.h"
#include "geometry.h"
#include "feature.h"
#include "featurecoverage.h"
#include "symboltable.h"
#include "OperationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "operationhelper.h"

using namespace Ilwis;

OperationHelper::OperationHelper()
{
}

void OperationHelper::initialize(const IIlwisObject &inputObject, Ilwis::IIlwisObject &outputObject, IlwisTypes tp, quint64 what)
{
    if (inputObject->ilwisType() & itCOVERAGE) {
        ICoverage covInput = inputObject.get<Coverage>();
        ICoverage covOutput = outputObject.get<Coverage>();
        if ( what & itENVELOPE) {
            Box2D<double> bounds = covInput->envelope();
            covOutput->envelope(bounds);
        }
        if ( what & itCOORDSYSTEM) {
            covOutput->setCoordinateSystem(covInput->coordinateSystem());
        }
        if ( what & itDOMAIN) {
            covOutput->datadef() = covInput->datadef();
        }


        if ( what & itTABLE) {
            if ( covInput->attributeTable(tp).isValid())    {
                if ( covInput->datadef().domain() == covOutput->datadef().domain()) {
                    if ( covOutput.isValid())
                        covOutput->attributeTable(tp,covInput->attributeTable(tp));
                }
            }
        }
    }
}
