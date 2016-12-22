#include "kernel.h"
#include "ilwisdata.h"
#include "modeller/modellerfactory.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"
#include "smce.h"

using namespace Ilwis;
using namespace Smce;

REGISTER_ANALYSISPATTERN(SMCE)

SMCE::SMCE() : Ilwis::AnalysisPattern()
{

}

bool SMCE::execute(const QVariantMap &inputParameters, QVariantMap &outputParameters)
{
    return false;
}

IWorkflow SMCE::workflow(const IOOptions &opt)
{
    return IWorkflow();
}


Node * SMCE::getNode (Node *node, quint16 id) {

    if (node->_id == id)
        return node;
    else {
        if (node->_subNodes.length() >0  ) {
            for (Node subNode: node->_subNodes) {
                Node *found = getNode(&subNode, id);

                if (found != nullptr)
                    return found;
            }
        }
    }
    return NULL;
}

// not tested
void SMCE::loadData(QDataStream &stream)
{
    Node node;
    quint8 nodeTypeInt;

    while (!stream.atEnd())
    {
        stream >> node._id >> node._parentId >> node._text >> nodeTypeInt >> node._weight >> node._input;
        node._type = static_cast<Node::NodeType>(nodeTypeInt);

        if (node._type == Node::NodeType::Goal )
            _tree = node;
        else {
            Node *parent = getNode(&_tree, node._parentId);
            if (parent != NULL)
                parent->_subNodes.append(node);
        }
    }
}

// not tested
void SMCE::storeNode(QDataStream &stream, Node node) {
    stream << node._id << node._parentId <<node._text << node._type << node._weight << node._input;
    if (node._subNodes.length() > 0) {
        for (Node subNode: node._subNodes) {
            storeNode(stream, subNode);
        }
    }
}

void SMCE::store(QDataStream &stream)
{
    stream << type();
    AnalysisPattern::store(stream);

    storeNode(stream, _tree);
}

void SMCE::loadMetadata(QDataStream &stream)
{
    AnalysisPattern::load(stream);
    //stream >> _tree;
}

QString SMCE::type() const
{
    return "smce";
}

SMCE::SMCE(const QString &name, const QString &description) : AnalysisPattern(name, description){

}

SMCE::AnalysisPattern *SMCE::create(const QString &name, const QString &description, const IOOptions &options)
{
    SMCE *smce =  new SMCE(name, description);
    return smce;
}


