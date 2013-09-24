#include <QString>
#include <functional>
#include <future>
#include <memory>
#include "kernel.h"
#include "raster.h"
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
        if ( hasType(what,itDOMAIN) && hasType(tp, itRASTER) ) {
            IRasterCoverage rasCoverageIn = inputObject.get<RasterCoverage>();
            IRasterCoverage rasCoverageOut = outputObject.get<RasterCoverage>();
            rasCoverageOut->datadef() = rasCoverageIn->datadef();
        }

        if ( hasType(what,itTABLE)) {
            if ( covInput->attributeTable(tp).isValid())    {
                if ( covOutput.isValid()) {
                    if ( hasType(tp, itRASTER)) {
                        IRasterCoverage rasCoverageIn = inputObject.get<RasterCoverage>();
                        IRasterCoverage rasCoverageOut = outputObject.get<RasterCoverage>();
                        if ( !rasCoverageIn.isValid() || !rasCoverageOut.isValid())
                            return;
                        if(rasCoverageIn->datadef().domain() != rasCoverageOut->datadef().domain())
                            return;
                    }
                    covOutput->attributeTable(tp,covInput->attributeTable(tp));
                }
            }

        }
    }
}
