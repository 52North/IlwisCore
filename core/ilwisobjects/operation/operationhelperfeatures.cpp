#include <QString>
#include <QThread>
#include <functional>
#include <future>
#include <memory>
#include "kernel.h"
#include "ilwisdata.h"
#include "datadefinition.h"
#include "columndefinition.h"
#include "attributedefinition.h"
#include "coverage.h"
#include "columndefinition.h"
#include "table.h"
#include "featurecoverage.h"
#include "feature.h"
#include "symboltable.h"
#include "operationExpression.h"
#include "operationmetadata.h"
#include "operation.h"
#include "commandhandler.h"
#include "operationhelper.h"
#include "operationhelperfeatures.h"

using namespace Ilwis;

OperationHelperFeatures::OperationHelperFeatures()
{
}

IIlwisObject OperationHelperFeatures::initialize(const IIlwisObject &inputObject, IlwisTypes tp, quint64 what)
{
    Resource resource(tp);
    resource.prepare();
    IIlwisObject obj;
    obj.prepare(resource);
    if (inputObject->ilwisType() & itCOVERAGE) {
        OperationHelper::initialize(inputObject, obj, tp, what);
    }
    return obj;
}

int OperationHelperFeatures::subdivideTasks(ExecutionContext *ctx, const IFeatureCoverage &fcov, std::vector<std::vector<quint32>> &subsets)
{
    int nofFeatures = fcov->featureCount();
    int cores = std::min(QThread::idealThreadCount(),nofFeatures);
    if (nofFeatures < 1000 || ctx->_threaded == false)
        cores = 1;

    int block = 0;
    subsets.resize(cores);
    int blockBoundary = nofFeatures / cores;
    for(int i=0; i<nofFeatures ; ++i) {
        subsets[block].push_back(i);
        if ( (i!= 0 && i % blockBoundary == 0) && block != cores - 1) { // last block maybe indefinitly big, catches rounding errors
            ++block;
        }
    }

    return cores;
}
