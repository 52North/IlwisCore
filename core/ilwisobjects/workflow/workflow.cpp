#include "kernel.h"
#include "ilwisdata.h"

#include "workflow.h"

using namespace Ilwis;

Workflow::Workflow(): OperationMetaData()
{

}

Workflow::Workflow(const Resource &resource): OperationMetaData(resource)
{
}

Workflow::~Workflow() {

}

OVertex Workflow::addOperation(OperationProperties properties)
{
    return boost::add_vertex(properties, _wfGraph);
}

void Workflow::removeOperation(OVertex vertex)
{
    boost::clear_vertex(vertex, _wfGraph);
    boost::remove_vertex(vertex, _wfGraph);
}

OEdge Workflow::addOperationFlow(OVertex v1, OVertex v2, FlowProperties properties)
{
    return (boost::add_edge(v1, v2, properties, _wfGraph)).first;
}

IlwisTypes Workflow::ilwisType() const
{
    return itOPERATIONMETADATA | itWORKFLOW;
}

quint64 Workflow::createMetadata()
{
    QString workflowName = name();


    /*
    if (_operationNodes.size() == 1) {
        OperationMetaData *metadata = _operationNodes.at(0);
        QString syntax = metadata->source()["syntax"];
        QString keywords = metadata->source()["keywords"];
        QString inparameters = metadata->source()["inparameters"];
        QString outparameters = metadata->source()["outparameters"];

        return mastercatalog()->addItems({operation});
    }

    // TODO perform actions on graph, i.e. topological sort, etc.

    QList<OperationMetaData*> operationNodes;
    for (auto it : operationNodes) {
        OperationMetaData *metadata = it;
        //metadata->
    }
    */

    /*
    OperationResource operation({"ilwis://operations/aggregateraster"});
    operation.setLongName("Spatial Aggregation of Raster coverage");

    operation.setSyntax("aggregateraster(inputgridcoverage,!Avg|Max|Med|Min|Prd|Std|Sum, groupsize,changegeometry)");


    operation.setDescription(TR("generates a rastercoverage according to a aggregation method. The aggregation method determines how pixel values are used in the aggregation"));
    operation.setInParameterCount({4});
    operation.addInParameter(0,itRASTER , TR("input rastercoverage"),TR("input rastercoverage with any domain"));
    operation.addInParameter(1,itSTRING , TR("Aggregation Method"),TR("the method how pixels inside a group will be accumulated"));
    operation.addInParameter(2,itINTEGER | itSTRING , TR("Groupsize"),TR("The size of the block used to aggregate. In the case of integer it is a square 2D block; in the case of string it is of the list format (2 or 3 dimensions). eg {3 4}"));
    operation.addInParameter(3,itBOOL , TR("change geometry"),TR("The aggregation can either create a map with a reduced size proportional to de block size or use the same geometry size but fill all pixels in the block with the aggregate"));
    operation.setOutParameterCount({1});
    operation.addOutParameter(0,itRASTER, TR("Aggregated raster"), TR("output rastercoverage with the domain of the input map"));
    operation.setKeywords("aggregate,raster,geometry");
     */
}
