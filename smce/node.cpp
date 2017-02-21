#include "node.h"
#include <QDebug>
#include <QUrl>

Node::Node()
{

}

Node::Node(QObject *qparent)
: QObject(qparent)
, _parent(0)
, _weight(0)
, _weights(0)
{

}

Node::Node(Node * parent, QObject *qparent)
: QObject(qparent)
, _parent(parent)
, _weight(0)
, _weights(0)
{

}

int Node::type() const
{
    return _type;
}

void Node::setType(NodeType nt)
{
    _type = nt;
    emit typeChanged();
}

const QString Node::name() const
{
    return _name;
}

void Node::setName(QString name)
{
    _name = name;
    emit nameChanged();
}

const QString Node::unit() const
{
    return _unit;
}

void Node::setUnit(QString unit)
{
    _unit = unit;
    emit unitChanged();
}

double Node::weight() const
{
    return _weight;
}

void Node::setWeight(double weight)
{
    _weight = weight;
    emit weightChanged();
}

Weights * Node::weights()
{
    if (_type == NodeType::Group && _subNodes.length() > 1) {
        if (!_weights)
            _weights = Weights::create(this);
        return _weights;
    } else
        return 0;
}

const Node * Node::parent() const
{
    return _parent;
}

QList<Node*> Node::subNodes()
{
    return _subNodes;
}

QQmlListProperty<Node> Node::subNodesQml()
{
    return QQmlListProperty<Node>(this, _subNodes);
}

void Node::addNode(Node * node)
{
    node->_parent = this;
    _subNodes.append(node);
    emit subNodesChanged();
}

const QString Node::fileName() const
{
    return _fileName;
}

void Node::setFileName(QString fileName)
{
    if (fileName.indexOf("://") < 0)
        _fileName = fileName;
    else {
        QUrl url (fileName);
        _fileName = url.fileName();
    }

    emit fileNameChanged();
}

int Node::level() const
{
    if (_parent != 0)
        return 1 + _parent->level();
    else
        return 0;
}

void Node::setGoal(QString name)
{
    if (_parent != 0)
        _parent->setGoal(name);
    else {
        setType(Group);
        setName(name);
        setWeight(-1);
    }
}

void Node::addMask(QString name)
{
    Node * node = new Node(this);
    node->setType(MaskArea);
    node->setName(name);
    node->setWeight(-1);
    addNode(node);
}

Node * Node::addGroup(QString name)
{
    Node * node = new Node(this);
    node->setType(Group);
    node->setName(name);
    addNode(node);
    recalcWeights();
    return node;
}

void Node::addFactor(QString name)
{
    Node * node = new Node(this);
    node->setType(Factor);
    node->setName(name);
    addNode(node);
    recalcWeights();
}

void Node::addConstraint(QString name)
{
    Node * node = new Node(this);
    node->setType(Constraint);
    node->setName(name);
    node->setWeight(-1);
    addNode(node);
}

void Node::recalcWeights()
{
    if (_subNodes.size() > 1) {
        for (Node* node : _subNodes) {
            node->setWeight(0);
        }
    } else if (_subNodes.size() == 1) {
        _subNodes[0]->setWeight(1);
    }
}

void Node::deleteNode()
{
    if (_parent) {// otherwise we're the root that can't be deleted
        emit nodeDeleted();
        _parent->deleteChild(this);
    }
}

void Node::deleteChild(Node *node)
{
    _subNodes.removeAll(node);
    emit subNodesChanged();
    node->deleteLater();
    recalcWeights();
}


/* ******************************************************* */

Weights::Weights()
{

}

Weights::Weights(Node *node)
: QObject(node)
, _node(node)
{

}

int Weights::method()
{
    return WeightMethod::None;
}

Weights * Weights::create(Node * node)
{
    return new DirectWeights(node);
}

DirectWeights * Weights::pDirectWeights()
{
    return 0;
}

void Weights::Recalculate()
{

}

void Weights::apply()
{

}

/* ******************************************************* */

DirectWeightItem::DirectWeightItem()
{

}

DirectWeightItem::DirectWeightItem(Node * node, DirectWeights *weights)
: QObject(node)
, _node(node)
, _weights(weights)
, _directWeight(0)
, _normalizedWeight(0)
{

}

QString DirectWeightItem::name()
{
    return _node->name();
}

double DirectWeightItem::directWeight()
{
    return _directWeight;
}

void DirectWeightItem::setDirectWeight(double weight)
{
    _directWeight = weight;
    emit directWeightChanged();
    _weights->Recalculate();
}

double DirectWeightItem::normalizedWeight()
{
    return _normalizedWeight;
}

void DirectWeightItem::setNormalizedWeight(double normalizedWeight)
{
    _normalizedWeight = normalizedWeight;
    emit normalizedWeightChanged();
}

/* ******************************************************* */

DirectWeights::DirectWeights()
: Weights()
, _weightType(SumToOne)
{
}

DirectWeights::DirectWeights(Node *node)
: Weights(node)
, _weightType(SumToOne)
{
    for (Node * child : _node->subNodes()) {
        DirectWeightItem * dwi = new DirectWeightItem(child, this);
        _directWeights.append(dwi);
    }
}

int DirectWeights::method()
{
    return WeightMethod::Direct;
}

DirectWeights * DirectWeights::pDirectWeights()
{
    return this;
}

DirectWeightItem * DirectWeights::getItem(Node * node)
{
    for (DirectWeightItem * dwi : _directWeights) {
        if (dwi->parent() == node)
            return dwi;
    }
    return 0;
}

void DirectWeights::Recalculate()
{
    Refresh(); // in case there was some resizing since last time
    if (SumToOne == _weightType)
    {
        double rSum = 0;

        for (Node * child : _node->subNodes())
            rSum += getItem(child)->directWeight();

        if (rSum != 0) {
            for (Node * child : _node->subNodes())
                getItem(child)->setNormalizedWeight(getItem(child)->directWeight() / rSum);
        } else {
            // sum = 0 .. make sure it becomes 1 by setting the first item to 1!!
            if (_node->subNodes().length() > 0)
                getItem(_node->subNodes().at(0))->setNormalizedWeight(1);
        }
    }
    else if (MaxIsOne == _weightType)
    {
        double rMax = 0;

        for (Node * child : _node->subNodes())
            rMax = fmax(rMax, getItem(child)->directWeight());

        if (rMax != 0) {
            for (Node * child : _node->subNodes())
                getItem(child)->setNormalizedWeight(getItem(child)->directWeight() / rMax);
        } else {
            // max = 0 .. make sure it becomes 1 by setting the first item to 1!!
            if (_node->subNodes().length() > 0)
                getItem(_node->subNodes().at(0))->setNormalizedWeight(1);
        }
    }
}

void DirectWeights::Refresh()
{
    QList <DirectWeightItem*> listTemp;
    for (Node * child : _node->subNodes()) {
        if (getItem(child)) // existing item; take over
            listTemp.append(getItem(child));
        else { // new item; create a corresponding DirectWeightItem
            DirectWeightItem * dwi = new DirectWeightItem(child, this);
            listTemp.append(dwi);
        }
    }
    // this will cleanup
    _directWeights = listTemp;
}

QQmlListProperty<DirectWeightItem> DirectWeights::directWeights()
{
    return QQmlListProperty<DirectWeightItem>(this, _directWeights);
}

void DirectWeights::apply()
{
    for (Node * child : _node->subNodes())
        if (getItem(child))
            child->setWeight(getItem(child)->normalizedWeight());
}
