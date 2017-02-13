#include "node.h"
#include <QDebug>
#include <QUrl>

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

void Node::setGoal(QString name, QString fileName)
{
    if (_parent != 0)
        _parent->setGoal(name, fileName);
    else {
        setType(Group);
        setName(name);
        setWeight(-1);
        setFileName(fileName);
    }
}

void Node::addMask(QString name, QString fileName)
{
    Node * node = new Node(this);
    node->setType(MaskArea);
    node->setName(name);
    node->setWeight(-1);
    node->setFileName(fileName);
    addNode(node);
}

Node * Node::addGroup(QString name, double weight, QString fileName)
{
    Node * node = new Node(this);
    node->setType(Group);
    node->setName(name);
    node->setWeight(weight);
    node->setFileName(fileName);
    addNode(node);
    return node;
}

void Node::addFactor(QString name, double weight, QString fileName)
{
    Node * node = new Node(this);
    node->setType(Factor);
    node->setName(name);
    node->setWeight(weight);
    node->setFileName(fileName);
    addNode(node);
}

void Node::addConstraint(QString name, QString fileName)
{
    Node * node = new Node(this);
    node->setType(Constraint);
    node->setName(name);
    node->setWeight(-1);
    node->setFileName(fileName);
    addNode(node);
}
