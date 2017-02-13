#include "kernel.h"
#include "ilwisdata.h"
#include "modeller/modellerfactory.h"
#include "symboltable.h"
#include "commandhandler.h"
#include "operationmetadata.h"
#include "geos/geom/Coordinate.h"
#include "location.h"
#include "ilwiscoordinate.h"
#include "box.h"
#include "workflownode.h"
#include "modeller/workflow.h"
#include "modeller/analysispattern.h"
#include "smce.h"

using namespace Ilwis;
using namespace Smce;

REGISTER_ANALYSISPATTERN(SMCE)

SMCE::SMCE() : Ilwis::AnalysisPattern()
, _tree(0)
{
    Init();
}

void SMCE::Init()
{
    _tree = new Node();
    _tree->setGoal("New Goal", "");
}

bool SMCE::execute(const QVariantMap &inputParameters, QVariantMap &outputParameters)
{
    return false;
}

IWorkflow SMCE::workflow(const IOOptions &opt)
{
    return IWorkflow();
}

Node * SMCE::root() const
{
    return _tree;
}

Node * SMCE::loadNode(QDataStream &stream)
{
    quint16 nrSubNodes;
    quint8 nodeTypeInt;
    QString name;
    QString unit;
    double weight;
    QString fileName;
    stream >> nodeTypeInt >> name >> unit >> weight >> fileName >> nrSubNodes;
    Node * node = new Node();
    node->setName(name);
    node->setUnit(unit);
    node->setWeight(weight);
    node->setType(static_cast<Node::NodeType>(nodeTypeInt));
    node->setFileName(fileName);
    for (int i = 0; i < nrSubNodes; ++i) {
        node->addNode(loadNode(stream));
    }
    return node;
}

void SMCE::loadData(QDataStream &stream)
{
    _tree = loadNode(stream);
}

// not tested
void SMCE::storeNode(QDataStream &stream, Node * node) const {
    stream << (quint8)(node->type()) << node->name() << node->unit() << node->weight() << node->fileName() << node->subNodes().length();
    if (node->subNodes().length() > 0) {
        for (Node * subNode: node->subNodes()) {
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
}

QString SMCE::type() const
{
    return "smce";
}

SMCE::SMCE(const QString &name, const QString &description) : AnalysisPattern(name, description)
, _tree(0)
{
    Init();
}

SMCE::AnalysisPattern *SMCE::create(const QString &name, const QString &description, const IOOptions &options)
{
    SMCE *smce =  new SMCE(name, description);
    return smce;
}


