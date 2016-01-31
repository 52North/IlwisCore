#ifndef WORKFLOW_H
#define WORKFLOW_H

#include <QPoint>
#include <QMap>
#include <QUrl>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include "kernel_global.h"
#include "ilwistypes.h"
#include "operationmetadata.h"
#include "resource.h"
#include "ilwisobject.h"

#include "iooptions.h"

namespace Ilwis {

/*!
 * \brief Struct for data for an input assignment
 */
struct AssignedInputData {
    AssignedInputData() {}
    QString inputName;
    QString value;
};

/*!
 * \brief Struct for data for an output assignment
 */
struct AssignedOutputData {
    AssignedOutputData() {}
    quint16 assignedParameterIndex;
    QString outputName;
};

typedef std::shared_ptr<AssignedInputData> SPAssignedInputData;
typedef std::shared_ptr<AssignedOutputData> SPAssignedOutputData;

/*!
 * \brief This struct is used for storing data in nodes within the graph.
 */
struct NodeProperties {
    NodeProperties(){}
    NodeProperties(const Resource& res) : _operationid(res.id()){
        _syntax = res["syntax"].toString();
        _resourceProvider = res["namespace"].toString();
    }
    NodeProperties(const QString& syntax, QString provider, quint16 x, quint16 y) {
        _syntax = syntax;
        _resourceProvider = provider;
        _x = x;
        _y = y;
        std::vector<Resource> items = mastercatalog()->select("catalogitemproperties.propertyname='syntax' and catalogitemproperties.propertyvalue='" + syntax + "'");
        if ( items.size() == 1){
            _operationid = items[0].id();
        }
        qDebug() << _operationid;
    }
    quint64 _operationid = i64UNDEF;
    QString _syntax;
    QString _resourceProvider;
    quint16 _x;
    quint16 _y;
};

/*!
 * \brief This struct is used for storing data in edges within the graph.
 */
struct EdgeProperties {
    EdgeProperties(int outParm, int inParm, int outRect, int inRect) :
        _outputParameterIndex(outParm),
        _inputParameterIndex(inParm),
        _outputRectangleIndex(outRect),
        _inputRectangleIndex(inRect){}
    EdgeProperties(int outParm, int inParm, int outRect, int inRect, int outOper) :
        _outputParameterIndex(outParm),
        _inputParameterIndex(inParm),
        _outputRectangleIndex(outRect),
        _inputRectangleIndex(inRect),
        _outputOperationIndex(outOper){}

    QString outputName;
    bool temporary = true;
    int _outputParameterIndex;
    int _inputParameterIndex;
    int _outputRectangleIndex;
    int _inputRectangleIndex;
    int _outputOperationIndex;
};

struct Condition {
    Condition(){}
    Condition(const Resource res) {
        _operation.prepare(res);
        int i = 0;
        for (auto parameter : _operation->getInputParameters()) {
            _inputAssignments.insert(i, AssignedInputData());
            ++i;
        }
    }

    IOperationMetaData _operation;
    QMap<int, AssignedInputData> _inputAssignments;
    QList<EdgeProperties> _edges;
};

struct ConditionContainer {
    ConditionContainer() {}
    QList<quint16> operationVertexes;
    QList<Condition> conditions;
};

typedef boost::property<boost::vertex_index1_t, NodeProperties> NodeProperty;
typedef boost::property<boost::edge_index_t, EdgeProperties> FlowProperty;
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                              NodeProperty, FlowProperty> WorkflowGraph;

typedef boost::property_map<WorkflowGraph, boost::vertex_index1_t>::type NodePropertyMap;
typedef boost::property_map<WorkflowGraph, boost::edge_index_t>::type EdgePropertyMap;

typedef boost::graph_traits<WorkflowGraph>::vertex_descriptor OVertex;
typedef boost::graph_traits<WorkflowGraph>::edge_descriptor OEdge;

typedef boost::graph_traits<WorkflowGraph>::in_edge_iterator InEdgeIterator;
typedef boost::graph_traits<WorkflowGraph>::out_edge_iterator OutEdgeIterator;

typedef boost::graph_traits<WorkflowGraph>::vertex_iterator WorkflowVertexIterator;

typedef std::pair<OVertex, int> InputAssignment;

/*!
 * \brief The Workflow class
 *
 * This class is used for generating and editing workflows within Ilwis Objects.
 * Note: an assignedInput is the value of the combination between a vertex and a parameter.
 * Note: an inputAssignment points to that value and it constist of the vertex and the parameter
 */
class KERNELSHARED_EXPORT Workflow: public OperationMetaData
{

    friend class WorkflowOperationImplementation;

public:
    /*!
     * \brief Constructor of Workflow
     *
     * This constructor must be called when creating a new workflow
     */
    Workflow();
    /*!
     * \brief Constructor of Workflow
     *
     * Call this constructor when the workflow already exists and needs to be reloaded.
     *
     * \param resource The resource of the existing workflow
     */
    Workflow(const Resource &resource);
    ~Workflow();

    /*!
     * \brief Checks whether vertex has input assignement
     *
     * Loops through all input assignments and looks whether one of them is his.
     *
     * \param v position of the vertex to look for
     * \return true if vertex has input assignments
     */
    bool hasInputAssignments(const OVertex &v) const;
    /*!
     * \brief Checks whether vertex has input assignment on index.
     *
     * Checks whether vertex and parameterIndex combination exists within the input assignment list
     *
     * \param v position of the vertex to look for
     * \param index position of parameter to look for
     * \return true if vertex has input assignment on index
     */
    bool hasInputAssignment(const OVertex &v, int index) const;
    /*!
     * \brief Get all assigned input data of all parameters of the vertex
     * \param v The vertex to get the input data from
     * \return a list of inputData
     */
    QList<SPAssignedInputData> getAssignedInputData(const OVertex &v) const;
    /*!
     * \brief Gets the assigned input data of the assignment
     * \param assignment Combination of vertex and parameterIndex
     * \return the input data of this assignment (inputName and value)
     */
    SPAssignedInputData getAssignedInputData(const InputAssignment &assignment) const;
    /*!
     * \brief Assigns empty input data to combination of vertex and index
     * \param v The vertex to asign input data to
     * \param index The parameter to asign input data to
     * \return The assigned input data
     */
    SPAssignedInputData assignInputData(const OVertex &v, quint16 index);
    /*!
     * \brief Assigns the properties to the assignment
     * \param assignment The combination of vertex and parameter index to assign the data to.
     * \param properties The properties (data) that need to be assigned to a parameter.
     */
    void assignInputData(const InputAssignment &assignment, const SPAssignedInputData &properties);
    /*!
     * \brief Assignes the output data properties to the vertex (These are still empty and are for later use).
     * \param v the vertex to assign data to
     * \return The assigned data
     */
    SPAssignedOutputData addOutputDataProperties(const OVertex &v);
    /*!
     * \brief Get all assigned output data of the vertex
     * \param v The vertex to get the output data from
     * \return a list of outputData
     */
    QList<SPAssignedOutputData> getOutputDataProperties(const OVertex &v) const;
    /*!
     * \brief Removes the assignment of the vertex and parameter combination. Usefull when connecting edges
     * \param v The vertex of the combination
     * \param index The parameter of the combination
     */
    void removeInputAssignment(const OVertex &v, quint16 index);
    /*!
     * \brief Cleans all input assignments of the vertex. Usefull when deleting operations.
     * \param v The vertex
     */
    void removeAllInputAssignments(const OVertex &v);
    /*!
     * \brief Removes the outputdata of the vertex and parameter combination.
     * \param v The vertex of the combination
     * \param index The parameter of the combination
     */
    void removeOutputDataProperties(const OVertex &v, quint16 index);
    /*!
     * \brief Adds an vertex to the graph and adds the properties to the vertex
     * \param properties The properties to add to our new vertex
     * \return The index of our new vertex.
     */
    OVertex addOperation(const NodeProperties &properties);
    /*!
     * \brief Adds an connection to the graph between two vertexes
     * \param from The vertex where the connection must come from
     * \param to The vertex where the connection should go to
     * \param properties The properties of the new edge
     * \return The index of our new edge
     */
    OEdge addOperationFlow(const OVertex &from, const OVertex &to, const EdgeProperties &properties);

    /*!
     * \brief adds an operation to a container
     * \param id of the container the condition gets added to
     * \param vertex of the operation that has to be added
     */
    void addOperationToContainer(quint16 containerId, OVertex operationVertex);

    /*!
     * \brief removes an operation from the ConditionContainer
     * \param id of the container the condition is currently in
     * \param vertex of the operation that has to be removed
     */
    void removeOperationFromContainer(quint16 containerId, OVertex operationVertex);
    /*!
     * \brief Removes the operation on the given index
     * \param vertex The index of the vertex thats need to be deleted
     */
    void removeOperation(OVertex vertex);
    /*!
     * \brief Removes the connection between to vertexes
     * \param edge The edge that should be deleted.
     */
    void removeOperationFlow(OEdge edge);

    /*!
     * \brief adds an empty conditioncontainer to the workflow.
     */
    void addConditionContainer();

    /*!
     * \brief getOperationCount returns the amout of vertexes
     * \return the amount of vertexes
     */
    int getOperationCount();
    /*!
     * \brief Gets the indexes from within the workflow input set that belong to the given vertex
     *
     * Searches for the given vertex through all inputs of this workflow. Returns all indexes of these inputs that belong to that vertex.
     *
     * \param v The vertex to look for.
     * \return  A list of parameterIndexes
     */
    QList<int>* getWorkflowParameterIndex(const OVertex &v) const;
    /*!
     * \brief Gets the indexes from within the workflow input set that belong to the given vertex and parameter
     *
     * Searches for the given vertex and parameter through all inputs of this workflow. Returns the index of taht input that belong to that vertex and parameter and -1 if not found.
     *
     * \param v The vertex to look for.
     * \param index The index of the parameter to look for
     * \return The index of the parameter within the inputs of the workflow. -1 if not found.
     */
    int getWorkflowParameterIndex(const OVertex &v, int index) const;
    /*!
     * \brief Returns all vertexes that have an input that do not have a value yet.
     * \return A List of vertex indexes
     */
    QList<OVertex> getNodesWithExternalInput();
    /*!
     * \brief Returns all vertexes that have an output that do not have an outgoing connection
     * \return A List of vertex indexes
     */
    QList<OVertex> getNodesWithExternalOutputs();

    /*!
     * \brief Returns the properties of the VertexMutableGraphConcept.
     * \param v The index of the vertex
     * \return The properties of this vertex
     */
    NodeProperties nodeProperties(const OVertex &v);
    /*!
     * \brief Returns the properties of the edge.
     * \param v The index of the edge
     * \return The properties of this edge
     */
    EdgeProperties edgeProperties(const OEdge &e);

    /*!
     * \brief Updates the properties of the vertex
     * \param v The index of the vertex
     * \param properties The new properties for this vertex
     */
    void updateNodeProperties(OVertex v, const NodeProperties &properties);
    /*!
     * \brief Updates the properties of the edge
     * \param v The index of the edge
     * \param properties The new properties for this edge
     */
    void updateEdgeProperties(OEdge e, const EdgeProperties &properties);

    /*!
     * \brief Returns the vertex where this edge comes from.
     * \param e The edge
     * \return The vertex where this edge comes from
     */
    OVertex getSourceOperationNode(const OEdge &e);
    /*!
     * \brief Returns the vertex where this edge goes to.
     * \param e The edge
     * \return The vertex where this edge goes to
     */
    OVertex getTargetOperationNode(const OEdge &e);

    /*!
     * \brief searches all ingoing edges of the vertex
     * \param v The vertex
     * \return A pair of iterators. the first is begin() and the seccond is end()
     */
    std::pair<InEdgeIterator, InEdgeIterator> getInEdges(const OVertex &v);
    /*!
     * \brief searches all outgoing edges of the vertex
     * \param v The vertex
     * \return A pair of iterators. the first is begin() and the seccond is end()
     */
    std::pair<OutEdgeIterator, OutEdgeIterator> getOutEdges(const OVertex &v);
    /*!
     * \brief returns all input assignments that belong to this vertex
     * \param v The vertex
     * \return A list of input assignments [<vertex, parameterIndex>]
     */
    QList<InputAssignment> getInputAssignments(const OVertex &v) const;

    /*!
     * \brief Looks whether vertex and parameter combination have a value defined.
     *
     * True when assignedInputdata of inputAssignment has a value
     * True when a line points to the parameter
     * Fals if none of above
     *
     * \param operationVertex The vertex
     * \param parameterIndex The index of the parameter
     * \return true when combination has value defined
     */
    bool hasValueDefined(const OVertex& operationVertex, int parameterIndex);
    /*!
     * \brief Returns the inputs of an operation which have already been defined (if they for example have a flow drawn to them)
     * \param operationVertex the operations
     * \return  A list of indexes
     */
    QStringList implicitIndexes(const OVertex &operationVertex);

    /*!
     * \brief Gets the metadata of the operation within this vertex
     * \param v The vertex
     * \return The metadata of the operation
     */
    IOperationMetaData getOperationMetadata(const OVertex &v);
    /*!
     * \brief Returns itWORKFLOW
     * \return itWORKFLOW
     */
    IlwisTypes ilwisType() const;
    /*!
     * \brief Creates the input and output metadata from the graph
     * \return The ID of this Ilwis object
     */
    quint64 createMetadata();

    /*!
     * \brief Returns the inputAssignments variable.
     * \return inputAssignments
     */
    QMap<InputAssignment, SPAssignedInputData> getAllInputAssignments() { return _inputAssignments; }
    /*!
     * \brief getNodeIterators
     * \return node iterators
     */
    std::pair<WorkflowVertexIterator, WorkflowVertexIterator> getNodeIterators() { return boost::vertices(_wfGraph); }
    /*!
     * \brief adds a condition with the given type to the the given container
     * \param containerId the id of the container
     * \param type the type of the condition
     * \return the operation
      */
    QVariantMap addCondition(int containerId, int operationId);

    /*!
     * \brief Returns container with the given id
     * \param containerId the id of the container
     * \return the container
     */
    ConditionContainer getContainer(const int containerId) { return _conditionContainers[containerId]; }

    /*!
     * \brief Returns condition of container with the given ids
     * \param containerId the id of the container
     * \param conditionId the id of the condition
     * \return the condition
     */
    Condition getCondition(const int containerId, const int conditionId) {
        return _conditionContainers[containerId].conditions[conditionId];
    }
    /*!
     * \brief Returns condition of container with the given ids
     * \param containerId the id of the container
     * \param conditionId the id of the condition
     * \return the condition
     */
    void assignConditionInputData(const int containerId, const int conditionId, const QStringList inputData);
    /*!
     * \brief Returns all containers that contain this vertex.
     * \return List of containers
     */
    QList<ConditionContainer> getContainersByVertex(const int v);

    /*!
     * \brief Call this to print the graph
     */
    void debugPrintGraph();
    /*!
     * \brief Call this to print the vertices
     */
    void debugPrintVertices();
    /*!
     * \brief Call this to print the edges
     */
    void debugPrintEdges();
    /*!
     * \brief Call this to print the metadata of the workflow
     */
    void debugWorkflowMetadata() const;
    void debugOperationParameter(const SPOperationParameter parameter) const;

    virtual bool isInternalObject() const;

    QStringList getInputTerms(const OVertex &v);
    QStringList getOutputTerms(const OVertex &v);

    std::vector<quint16> getAssignedPouts(const OVertex &v);

private:
    // The graph representation of the workflow
    WorkflowGraph _wfGraph;
    // Is only used in loading the graph (DONT TOUCH)
    QList<OVertex> _inputNodes;
    // Is only used in loading the graph (DONT TOUCH)
    QList<OVertex> _outputNodes;
    QList<ConditionContainer> _conditionContainers;

    // The inputassignments looks as following: [<vetex, parameter>, {inputName, value}]
    QMap<InputAssignment, SPAssignedInputData> _inputAssignments;
    // The outputProperties looks as following: [vetex, {outputParameterName, index}]
    QMap<OVertex, QList<SPAssignedOutputData>> _outputProperties;

    /*!
     * \brief returns the node property map of the graph
     * \return the node property map of the graph
     */
    NodePropertyMap nodeIndex();
    /*!
     * \brief returns the edge property map of the graph
     * \return the edge property map of the graph
     */
    EdgePropertyMap edgeIndex();

    // Create metadata shit
    void parseInputParameters();
    void parseOutputParameters();
    QStringList createSyntaxTerms(const OVertex &v, const std::vector<SPOperationParameter> &parameters, const QString &inoutPart);
    QString createParametersCountString(const QStringList &mandatory, const QStringList &optionals) const;

    QList<InputAssignment> getOpenInputAssignments(const OVertex &v) const;
    QList<InputAssignment> getConstantInputAssignments(const OVertex &v) const;
    QList<InputAssignment> getImplicitInputAssignments(const OVertex &v);
};

typedef IlwisData<Workflow> IWorkflow;

}

#endif // WORKFLOW_H
