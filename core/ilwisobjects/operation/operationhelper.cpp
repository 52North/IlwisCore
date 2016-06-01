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
