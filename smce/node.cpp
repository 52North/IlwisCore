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
{

}

Node::Node(Node * parent, QObject *qparent)
: QObject(qparent)
, _parent(parent)
, _weight(0)
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
    return QQmlListProperty<Node>(this, _subNodes) ;
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
