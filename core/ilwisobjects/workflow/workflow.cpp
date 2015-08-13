#include <QUrl>

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

SPDataProperties Workflow::addInputDataProperties(const OVertex v)
{
    if ( !_inputProperties.contains(v)) {
        QList<SPDataProperties> list;
        _inputProperties.insert(v, list);
    }
    SPDataProperties properties = SPDataProperties(new DataProperties());
    _inputProperties[v].push_back(properties);
    return SPDataProperties(properties);
}


SPDataProperties Workflow::addOutputDataProperties(const OVertex &v)
{
    if ( !_outputProperties.contains(v)) {
        QList<SPDataProperties> list;
        _outputProperties.insert(v, list);
    }
    SPDataProperties properties = SPDataProperties(new DataProperties());
    _outputProperties[v].push_back(properties);
    return SPDataProperties(properties);
}

void Workflow::removeInputDataProperties(const OVertex &v, quint16 index)
{
    if (_inputProperties.contains(v) && _inputProperties[v].size() > index) {
        _inputProperties[v].removeAt(index);
    }
}

void Workflow::removeOutputDataProperties(const OVertex &v, quint16 index)
{
    if (_outputProperties.contains(v) && _outputProperties[v].size() > index) {
        _outputProperties[v].removeAt(index);
    }
}

OVertex Workflow::addOperation(const NodeProperties &properties)
{
    return boost::add_vertex(properties, _wfGraph);
}

void Workflow::removeOperation(OVertex vertex)
{
    boost::clear_vertex(vertex, _wfGraph);
    boost::remove_vertex(vertex, _wfGraph);
}

NodeProperties Workflow::nodeProperties(const OVertex &v)
{
    return nodeIndex()[v];
}

QList<OVertex> Workflow::getRoots()
{
    QList<OVertex> roots;
    qDebug() << "inputs(" << name() << ")=";
    boost::graph_traits<WorkflowGraph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi,vi_end) = boost::vertices(_wfGraph); vi != vi_end; ++vi) {
        OVertex v = *vi;
        if (boost::in_edges(v, _wfGraph).first == boost::in_edges(v, _wfGraph).second) {
            qDebug() << "in[" << nodeProperties(v).id << "] ";
            roots.push_back(v);
        }
    }
    return roots;
}

QList<OVertex> Workflow::getLeafs()
{
    QList<OVertex> leafs;
    qDebug() << "outputs(" << name() << ")=";
    boost::graph_traits<WorkflowGraph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi,vi_end) = boost::vertices(_wfGraph); vi != vi_end; ++vi) {
        OVertex v = *vi;
        if (boost::out_edges(v, _wfGraph).first == boost::out_edges(v, _wfGraph).second) {
            qDebug() << "out[" << nodeProperties(v).id << "] ";
            leafs.push_back(v);
        }
    }
    return leafs;
}

void Workflow::updateNodeProperties(OVertex v, const NodeProperties &properties)
{
    boost::put(nodeIndex(), v, properties);
}

void Workflow::updateEdgeProperties(OEdge e, const EdgeProperties &properties)
{
    boost::put(edgeIndex(), e, properties);
}

OEdge Workflow::addOperationFlow(const OVertex &v1, const OVertex &v2, const EdgeProperties &properties)
{
    return (boost::add_edge(v1, v2, properties, _wfGraph)).first;
}

IlwisTypes Workflow::ilwisType() const
{
    return itWORKFLOW;
}

quint64 Workflow::createMetadata()
{
    //mastercatalog()->removeItems({ source() });

    parseInputParameters();
    parseOutputParameters();

    //mastercatalog()->addItems( { source() });
    return source().id();

}

NodePropertyMap Workflow::nodeIndex()
{
    return get(boost::vertex_index1, _wfGraph);
}

EdgePropertyMap Workflow::edgeIndex()
{
    return get(boost::edge_index, _wfGraph);
}

void Workflow::parseInputParameters()
{
    qDebug() << "parse input parameters for workflow";
    clearInputs();
    int inCount = 0;
    for (OVertex root : getRoots()) {
        NodeProperties properties = nodeProperties(root);
        IOperationMetaData rootMeta;
        rootMeta.prepare(mastercatalog()->id2Resource(properties.id));
        if ( !rootMeta.isValid()) {
            qDebug() << properties.id << "is not known IlwisObject!";
        }

        /*
            // TODO parse optional parameters
            // TODO let the user edit parameter names and optionalities
            Resource opResource = rootMeta->source();
            QString inParameters = opResource["inparameters"];
        */
        // add all inparameters for now
        inCount += rootMeta->getInputParameters().size();

        for (SPOperationParameter input : rootMeta->getInputParameters()) {
            SPOperationParameter parameter = newParameter(input);
            parameter->addToResource(source());
        }
    }
    // TODO replace max number of args with parsed inParameters
    setProperty("inparameters", inCount);

}

void Workflow::parseOutputParameters()
{
    qDebug() << "parse output parameters for workflow";
    clearOutputs();
    int outCount = 0;
    for (OVertex leaf : getLeafs()) {
        NodeProperties properties = nodeProperties(leaf);
        IOperationMetaData leafMeta;
        leafMeta.prepare(mastercatalog()->id2Resource(properties.id));
        if ( !leafMeta.isValid()) {
            qDebug() << properties.id << "is not known IlwisObject!";
        }
        /*
            // TODO parse optional parameters
            // TODO let the user edit parameter names and optionalities
            Resource opResource = rootMeta->source();
            QString outParameters = opResource["outparameters"];
        */
        // add all outparameters for now
        outCount += leafMeta->getOutputParameters().size();

        for (SPOperationParameter output : leafMeta->getOutputParameters()) {
            SPOperationParameter parameter = newParameter(output);
            parameter->addToResource(source());
        }
    }
    // TODO replace max number of args with parsed outParameters
    setProperty("outparameters", outCount);
}


void Workflow::debugPrintGraph()
{
    debugPrintVertices();
    debugPrintEdges();
}

void Workflow::debugPrintVertices()
{
    qDebug() << "vertices(g) = ";
    boost::graph_traits<WorkflowGraph>::vertex_iterator vi, vi_end;
    for (boost::tie(vi, vi_end) = boost::vertices(_wfGraph); vi != vi_end; ++vi) {
        OVertex v = *vi;
        NodeProperties p = nodeIndex()[v];
        qDebug() << p.id;// /*<< " ";//*/ << " (id=" << v << ") ";
    }
}

void Workflow::debugPrintEdges()
{
    qDebug() << "edges(g) = ";
    boost::graph_traits<WorkflowGraph>::edge_iterator ei, ei_end;
    for (boost::tie(ei, ei_end) = boost::edges(_wfGraph); ei != ei_end; ++ei)
        qDebug() << "(" << nodeIndex()[boost::source(*ei, _wfGraph)].id
                  << "->" << nodeIndex()[boost::target(*ei, _wfGraph)].id
                  //<< "[foo='" << edgeIndex()[*ei].foo << "']"
                  << ") ";
}
