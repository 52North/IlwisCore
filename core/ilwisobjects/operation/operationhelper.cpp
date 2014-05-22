#include <QString>
#include <functional>
#include <future>
#include <memory>
#include "kernel.h"
#include "raster.h"
#include "columndefinition.h"
#include "table.h"
#include "attributerecord.h"
#include "feature.h"
#include "featurecoverage.h"
#include "symboltable.h"
#include "operationExpression.h"
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
            rasCoverageOut->datadef() = rasCoverageIn->datadef();
        }

        if ( hasType(what,itTABLE)) {
            if ( covInput->attributeTable().isValid())    {
                if ( covOutput.isValid()) {
                    if ( hasType(tp, itRASTER)) {
                        IRasterCoverage rasCoverageIn = inputObject.as<RasterCoverage>();
                        IRasterCoverage rasCoverageOut = outputObject.as<RasterCoverage>();
                        if ( !rasCoverageIn.isValid() || !rasCoverageOut.isValid())
                            return;
                        if(rasCoverageIn->datadef().domain<>() != rasCoverageOut->datadef().domain<>())
                            return;
                    }
                    covOutput->attributeTable(covInput->attributeTable());
                }
            }

        }
    }
}
