#include "core/ilwisobjects/coverage/rastercoverage.h"

#include "node.h"
#include <QUrl>

using namespace Ilwis;

Node::Node()
{

}

Node::Node(QObject *qparent)
: QObject(qparent)
, _parent(0)
, _weight(0)
, _weights(0)
, _weightsEdit(0)
, _standardization(0)
, _standardizationEdit(0)
, _smceMode(Mode::EditTree)
{

}

Node::Node(Node * parent, QObject *qparent)
: QObject(qparent)
, _parent(parent)
, _weight(0)
, _weights(0)
, _weightsEdit(0)
, _standardization(0)
, _standardizationEdit(0)
, _smceMode(Mode::EditTree)
{

}

Node * Node::load(QDataStream &stream, QObject *qparent)
{
    quint8 nodeTypeInt;
    QString name;
    QString unit;
    double weight;
    QString fileName;
    quint16 nrSubNodes;
    stream >> nodeTypeInt >> name >> unit >> weight >> fileName >> nrSubNodes;
    Node * node = new Node(qparent);
    node->setName(name);
    node->setUnit(unit);
    node->setWeight(weight);
    node->setType(static_cast<Node::NodeType>(nodeTypeInt));
    node->setFileName(fileName);
    for (int i = 0; i < nrSubNodes; ++i) {
        node->addNode(load(stream, node));
    }
    Weights::load(stream, node);
    Standardization::load(stream, node);
    return node;
}

void Node::store(QDataStream &stream)
{
    stream << (quint8)(_type) << _name << _unit << _weight << _fileName << (quint16)_subNodes.length();
    if (_subNodes.length() > 0) {
        for (Node * subNode: _subNodes) {
            subNode->store(stream);
        }
    }
    Weights::store(stream, _weights);
    Standardization::store(stream, _standardization);
}

int Node::type() const
{
    return _type;
}

void Node::setType(NodeType nt)
{
    _type = nt;
    emit typeChanged();
    emit doneChanged();
    if (parent())
        root()->emitDoneChanged();
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

QString Node::uniqueName(QString name)
{ // produce a unique name that can be safely added as a child to the current node
    QString unique = name;
    QList <Node*> siblings = subNodes();
    bool found = false;
    int seq = 0;
    while(!found) {
        found = true;
        for (Node* node : siblings) {
            if (node->name() == unique) {
                ++seq;
                unique = QString("%1 (%2)").arg(name).arg(seq);
                found = false;
                break;
            }
        }
    }
    return unique;
}

bool Node::nameAllowed(QString name)
{
    if (name.length() == 0)
        return false;
    if (parent()) {
        QList <Node*> siblings = parent()->subNodes();
        for (Node* node : siblings) {
            if (node == this) // except ourselves
                continue;
            if (node->name() == name)
                return false;
        }
    }
    return true;
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
    emit doneChanged();
    if (parent())
        root()->emitDoneChanged();
}

Weights * Node::weights()
{
    if (_type == NodeType::Group && subFactors().size() > 1) {
        if (!_weightsEdit) {
            if (!_weights)
                _weightsEdit = Weights::create(this, Weights::Direct);
            else
                _weightsEdit = _weights->clone();
        }
        return _weightsEdit;
    } else
        return 0;
}

void Node::setWeights(Weights * weights)
{
    _weights = weights;
    emit doneChanged();
    if (parent())
        root()->emitDoneChanged();
}

void Node::resetWeightEdits()
{
    _weightsEdit = 0;
    emit weightsChanged();
}

Standardization * Node::standardization()
{
    if (_type == NodeType::Factor || _type == NodeType::Constraint) {
        if (!_standardizationEdit) {
            if (!_standardization)
                _standardizationEdit = Standardization::create(this);
            else
                _standardizationEdit = _standardization->clone();
        }
        return _standardizationEdit;
    } else
        return 0;
}

void Node::setStandardization(Standardization * standardization)
{
    _standardization = standardization;
    emit doneChanged();
    if (parent())
        root()->emitDoneChanged();
}

void Node::resetStandardizationEdits()
{
    _standardizationEdit = 0;
    emit standardizationChanged();
}

const Node * Node::parent() const
{
    return _parent;
}

QList<Node*> Node::subNodes() const
{
    return _subNodes;
}

QList <Node*> Node::subFactors() const
{
    QList<Node*> result;
    for (Node * node : _subNodes) {
        if (node->type() == NodeType::Factor)
            result.append(node);
        else if (node->type() == NodeType::Group && node->subFactors().length() > 0)
            result.append(node);
    }
    return result;
}

QList <Node*> Node::getConstraints() const
{
    QList<Node*> result;
    for (Node * node : _subNodes) {
        if (node->type() == NodeType::Group) {
            QList<Node*> subResult = node->getConstraints();
            for (Node * subNode : subResult)
                result.append(subNode);
        } else if (node->type() == NodeType::Constraint) {
            result.append(node);
        }
    }
    return result;
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
    emit doneChanged();
    if (parent())
        root()->emitDoneChanged();
}

const QString Node::fileName() const
{
    return _fileName;
}

void Node::setFileName(QString fileName)
{
    if (fileName.indexOf("://") < 0) {
        _fileName = fileName;
        if (_fileName.length() > 0 && (!_fileName.endsWith(".mpr") && !_fileName.endsWith(".tif") && !_fileName.endsWith(".img")))
            _fileName += ".mpr";
    } else {
        QUrl url (fileName);
        _fileName = url.fileName();
    }

    emit fileNameChanged();
    emit doneChanged();
    if (parent())
        root()->emitDoneChanged();
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
    node->setWeight(-1); // a new empty group has no weight, until factors are added to it
    addNode(node);
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
    if (_weights) {
        _weights->Recalculate();
        _weights->apply();
    } else {
        QList<Node*> sub = subFactors();
        if (sub.size() > 0) {
            if (sub.size() > 1) {
                for (Node* node : sub) {
                    node->setWeight(0);
                }
            } else
                sub[0]->setWeight(1);
            if (_parent != 0 && weight() == -1)
                setWeight(0); // first-time set weight for group with factor-children
        } else if (_parent != 0)
            setWeight(-1); // no weight if group has no factor-children
    }
    if (_parent != 0)
        _parent->recalcWeights(); // propagate all the way to the root; adding the first or deleting the last factor in a (sub)group may influence all its parents
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
    emit doneChanged();
    if (parent())
        root()->emitDoneChanged();
    node->deleteLater();
    recalcWeights();
}

QString Node::getPython(QString outputName, bool first) const
{
    QString result;
    if (_weight == 0)
        return "";
    switch(_type) {
    case NodeType::Group:
        {
            // Cases:
            // 1: there are no children
            // 2: there are only factors
            // 3: there are only constraints
            // 4: there are both factors and constraints
            QList<Node*> children = subFactors();
            QString nodeOutputName = QString("%1_%2").arg(outputName).arg(level());
            if (children.length() == 0) {
                if (first) {
                    QList<Node*> constraints = getConstraints();
                    QString constraintScript;
                    for (Node * node : constraints) {
                        if (constraintScript.length() > 0) {
                            QString term = node->getPython(nodeOutputName);
                            if (term.length() > 0) {
                                constraintScript += term;
                                constraintScript += QString("%1=%1 & %2\n").arg(outputName + "_constraint").arg(nodeOutputName);
                                constraintScript += QString("del %1\n").arg(nodeOutputName);
                            }
                        } else {
                            constraintScript = node->getPython(outputName + "_constraint");
                        }
                    }
                    if (constraintScript.length() > 0) {
                        result += constraintScript;
                        result += QString("%1=ilwis.Engine.do('iffraster',%2,1,0)\n").arg(outputName).arg(outputName + "_constraint");
                        result += QString("del %1\n").arg(outputName + "_constraint");
                    }
                } else
                    return "";
            } else {
                for (Node * node : children) {
                    if (result.length() > 0) {
                        QString term = node->getPython(nodeOutputName);
                        if (term.length() > 0) {
                            result += term;
                            result += QString("%1=%1+%2\n").arg(outputName).arg(nodeOutputName);
                            result += QString("del %1\n").arg(nodeOutputName);
                        }
                    } else {
                        result = node->getPython(outputName);
                    }
                }
                if (_weight > 0 && _weight < 1) {
                    result += QString("%1=%2*%1\n").arg(outputName).arg(_weight);
                }
                if (first) {
                    QList<Node*> constraints = getConstraints();
                    QString constraintScript;
                    for (Node * node : constraints) {
                        if (constraintScript.length() > 0) {
                            QString term = node->getPython(nodeOutputName);
                            if (term.length() > 0) {
                                constraintScript += term;
                                constraintScript += QString("%1=%1 & %2\n").arg(outputName + "_constraint").arg(nodeOutputName);
                                constraintScript += QString("del %1\n").arg(nodeOutputName);
                            }
                        } else {
                            constraintScript = node->getPython(outputName + "_constraint");
                        }
                    }
                    if (constraintScript.length() > 0) {
                        result += constraintScript;
                        result += QString("%1=ilwis.Engine.do('iffraster',%2,%1,0)\n").arg(outputName).arg(outputName + "_constraint");
                        result += QString("del %1\n").arg(outputName + "_constraint");
                    }
                }
            }
        }
        break;
    case NodeType::Factor:
        {
            QString coverageName = outputName + "_input";
            result = coverageName + "=ilwis.RasterCoverage('" + _fileName + "')\n";
            result += _standardization->getPython(coverageName, outputName);
            if (_weight > 0 && _weight < 1)
                result += QString("%1=%2*%1\n").arg(outputName).arg(_weight);
            result += "del " + coverageName + "\n";
            break;
        }
    case NodeType::Constraint:
        {
            QString coverageName = outputName + "_input";
            result = coverageName + "=ilwis.RasterCoverage('" + _fileName + "')\n";
            result += _standardization->getPython(coverageName, outputName);
            result += "del " + coverageName + "\n";
            break;
        }
    default:
        return "";
    }
    return result;
}

QString Node::getMapcalc() const
{
    QString result;
    if (_weight == 0)
        return "";
    switch(_type) {
    case NodeType::Group:
        {
            QList<Node*> children = subFactors();
            if (children.length() == 0)
                return "";
            else {
                for (Node * node : children) {
                    QString term = node->getMapcalc();
                    if (term.length() > 0)
                        result += ((result.length() > 0) ? "+" : "") + term;
                }
                if (_weight > 0 && _weight < 1) {
                    if (children.length() > 1)
                        result = "(" + result + ")";
                    result = QString("%1*%2").arg(_weight).arg(result);
                }
            }
        }
        break;
    case NodeType::Factor:
        {
            QString rasterCoverage = "ilwis.RasterCoverage('" + _fileName + "')";
            result = _standardization->getMapcalc(rasterCoverage);
            if (_weight > 0 && _weight < 1)
                result = QString("%1*%2").arg(_weight).arg(result);
            break;
        }
    default:
        return "";
    }
    return result;
}

bool Node::done(int mode, int col, bool recursive) const
{
    if (_type == NodeType::Group) {
        // two cases: 1) we're editing the tree, 2) we're std/weighing
        // when we're editing, check the following:
        // check if the group has children
        // for col>0 check if the output map is !="" for the root
        // when we're std/weighing, check the following:
        // for col == 0 check if there are any children and all factors are weighed
        bool ok;
        if (mode == Mode::EditTree) {
            if (col > 0)
                ok = (parent() != 0) || (_fileName != "");
            else
                ok = (_subNodes.size()>0);

            if (recursive) { // means we're doing a final check to see if we can change the mode
                if (parent() == 0) {// we're the root
                    ok = ok && (_fileName != "");
                }
            }
        }
        else
        {
            if (col > 0) {
                ok = true;
                if (parent() == 0) // we're the root and the output is maps
                    ok = _fileName != "";
            } else {
                ok = (_subNodes.size() > 0);
                int factors = subFactors().size();
                if (factors > 0)
                    ok = ok && ((_weights != 0) || (factors == 1));
            }
        }

        if (recursive)
            for (Node * node : _subNodes)
                ok = ok && node->done(mode, col, true);

        return ok;
    } else {
        // for col>0 check if the selected input map exists, independent on the mode
        // if mode is std/weigh, check  for col==0 if the thing is standardized

        // NOTE: Decide on the fRecursive flag if we should check on the availability of all input data
        bool ok = true;
        if (col > 0) {
            ok = _fileName != ""; // now check
            if (ok) {
                try {
                    Ilwis::IRasterCoverage rc(_fileName, itRASTER); // "probe"
                } catch (const ErrorObject& err){
                    ok = false; // _fileName does not exist or is not a valid RasterCoverage
                }
            }
        } else { // col == 0
            if (mode == Mode::EditTree) {
                if (recursive) { // means we're doing a final check to see if we can change the mode
                    unsigned int i=1;
                    while (ok && (i<=1)) {
                        ok = ok && done(mode, i, false); // not recursive!!
                        ++i;
                    }
                }
            }
            else // mode == Mode::StdWeigh
                ok = (_type == NodeType::MaskArea || _standardization);// && _standardization->fStandardized());
        }
        return ok;
    }
}

Node * Node::root()
{
    Node * treeRoot = this;
    while (treeRoot->_parent != 0)
        treeRoot = treeRoot->_parent;
    return treeRoot;
}

void Node::setSmceMode(int mode)
{
    Node * treeRoot = root();
    treeRoot->_smceMode = (Mode)mode;
    treeRoot->recursivelyEmitDoneChanged();
}

int Node::smceMode()
{
    Node * treeRoot = root();
    return treeRoot->_smceMode;
}

void Node::recursivelyEmitDoneChanged() const
{
    emit doneChanged();
    for (const Node * child : _subNodes)
        child->recursivelyEmitDoneChanged();
}

bool Node::nodeDone()
{
    return done(root()->_smceMode, 0, false);
}

bool Node::col1Done()
{
    return done(root()->_smceMode, 1, false);
}

bool Node::treeEditDone()
{
    return root()->done(Mode::EditTree, 0, true);
}

bool Node::stdWeighDone()
{
    return root()->done(Mode::StdWeigh, 0, true);
}

void Node::emitDoneChanged() const
{
    emit doneChanged();
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

Weights * Weights::create(Node * node, WeightMethod method)
{
    switch (method) {
    case Weights::Direct:
        return new DirectWeights(node);
        break;
    default:
        return 0;
    }
}

void Weights::store(QDataStream &stream, Weights * weights)
{
    if (weights != 0) {
        stream << (quint8)weights->method();
        weights->store(stream);
    } else
        stream << (quint8)WeightMethod::None;
}

void Weights::load(QDataStream &stream, Node * node)
{
    quint8 method;
    stream >> method;
    if (method != 0) {
        Weights * weights = create(node, static_cast<Weights::WeightMethod>(method));
        weights->load(stream);
        weights->apply();
    }
}

void Weights::store(QDataStream &stream)
{

}

void Weights::load(QDataStream &stream)
{

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
    _node->setWeights(this);
}

Weights * Weights::clone() const
{
    return new Weights(_node);
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
    for (Node * child : _node->subFactors()) {
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

        for (Node * child : _node->subFactors())
            rSum += getItem(child)->directWeight();

        if (rSum != 0) {
            for (Node * child : _node->subFactors())
                getItem(child)->setNormalizedWeight(getItem(child)->directWeight() / rSum);
        } else {
            // sum = 0 .. make sure it becomes 1 by setting the first item to 1!!
            if (_node->subFactors().size() > 0)
                getItem(_node->subFactors().at(0))->setNormalizedWeight(1);
        }
    }
    else if (MaxIsOne == _weightType)
    {
        double rMax = 0;

        for (Node * child : _node->subFactors())
            rMax = fmax(rMax, getItem(child)->directWeight());

        if (rMax != 0) {
            for (Node * child : _node->subFactors())
                getItem(child)->setNormalizedWeight(getItem(child)->directWeight() / rMax);
        } else {
            // max = 0 .. make sure it becomes 1 by setting the first item to 1!!
            if (_node->subFactors().size() > 0)
                getItem(_node->subFactors().at(0))->setNormalizedWeight(1);
        }
    }
}

void DirectWeights::Refresh()
{
    QList <DirectWeightItem*> listTemp;
    for (Node * child : _node->subFactors()) {
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
    Weights::apply();
    for (Node * child : _node->subFactors())
        if (getItem(child))
            child->setWeight(getItem(child)->normalizedWeight());
}

Weights * DirectWeights::clone() const
{
    DirectWeights * dwClone = new DirectWeights(_node);
    for (int i = 0; i < _directWeights.length(); ++i) {
        dwClone->_directWeights.at(i)->setDirectWeight(_directWeights.at(i)->directWeight());
        dwClone->_directWeights.at(i)->setNormalizedWeight(_directWeights.at(i)->normalizedWeight());
    }
    dwClone->_weightType = _weightType;
    return dwClone;
}

void DirectWeights::store(QDataStream &stream)
{
    Weights::store(stream);
    stream << (quint8)_weightType;
    for (DirectWeightItem * dwi : _directWeights) {
        stream << dwi->directWeight() << dwi->normalizedWeight();
    }
}

void DirectWeights::load(QDataStream &stream)
{
    Weights::load(stream);
    quint8 weightType;
    stream >> weightType;
    _weightType = static_cast<DirectWeights::WeightType>(weightType);
    for (int i = 0; i < _directWeights.length(); ++i) {
        DirectWeightItem * dwi = _directWeights.at(i);
        double directWeight;
        double normalizedWeight;
        stream >> directWeight >> normalizedWeight;
        dwi->setDirectWeight(directWeight);
        dwi->setNormalizedWeight(normalizedWeight);
    }
}

/* ******************************************************* */

SmceFunction::SmceFunction(Node * node, QList<Anchor*> & anchors, int nrAnchors, double minX, double maxX, double minY, double maxY, bool benefit)
: _anchors(anchors)
, _minX(minX)
, _maxX(maxX)
, _minY(minY)
, _maxY(maxY)
, _benefit(benefit)
{
    _anchors.clear();
    for (int i = 0; i < nrAnchors; ++i)
        _anchors.append(new Anchor(node, 0, 0));
}

SmceFunction::~SmceFunction()
{

}

double SmceFunction::rUnitDist2(double x, double y, Anchor * anchor)
{
    double dX = x - anchor->x();
    double dY = y - anchor->y();
    double rNormX = dX / (_maxX - _minX);
    double rNormY = dY / (_maxY - _minY);
    return rNormX * rNormX + rNormY * rNormY;
}

void SmceFunction::SeparateAnchorPoints()
{
    double rMinSeparation = (_maxX - _minX) / 1000.0;
    for (int i = 1; i < _anchors.size(); ++i) {
        if (_anchors[i]->x() < (_anchors[i-1]->x() + rMinSeparation))
            _anchors[i]->setX(_anchors[i-1]->x() + rMinSeparation);
    }
}

void SmceFunction::setAnchor(double x, double y)
{
    if (_anchors.size() > 0) {
        int iPoint = 0;
        for (int i = 1; i < _anchors.size(); ++i) {
            if (rUnitDist2(x, y, _anchors[i]) < rUnitDist2(x, y, _anchors[iPoint]))
                iPoint = i;
        }
        _anchors[iPoint]->setX(x);
        _anchors[iPoint]->setY(y);
        SeparateAnchorPoints();
    }
    solveParams();
}

/* ******************************************************* */

MaximumFunction::MaximumFunction(Node *node, QList<Anchor*> & anchors, double minX, double maxX, double minY, double maxY, bool benefit)
: SmceFunction(node, anchors, 0, minX, maxX, minY, maxY, benefit)
{

}

void MaximumFunction::solveParams()
{
}

void MaximumFunction::setDefaultAnchors()
{
}

double MaximumFunction::getFx(double x) const
{
    double a = _benefit ? ((_maxY -_minY) / _maxX) : ((_minY - _maxY) / _maxX);
    double b = _benefit ? _minY : (_maxY - a * _minX);
    return a * x + b;
}

QString MaximumFunction::getPython(QString rasterCoverage, QString outputName)
{
    QString result = QString("%1=%2\n").arg(outputName).arg(getMapcalc(rasterCoverage));
    return result;
}

QString MaximumFunction::getMapcalc(QString rasterCoverage)
{
    double a = _benefit ? ((_maxY -_minY) / _maxX) : ((_minY - _maxY) / _maxX);
    double b = _benefit ? _minY : (_maxY - a * _minX);

    //return a * x + b;
    if (a != 0) {
        if (b != 0)
            return QString ("%1*%2+%3").arg(a).arg(rasterCoverage).arg(b);
        else
            return QString("%1*%2").arg(a).arg(rasterCoverage);
    } else {
        return QString("%1").arg(b);
    }
}

/* ******************************************************* */

IntervalFunction::IntervalFunction(Node *node, QList<Anchor*> & anchors, double minX, double maxX, double minY, double maxY, bool benefit)
: SmceFunction(node, anchors, 0, minX, maxX, minY, maxY, benefit)
{

}

void IntervalFunction::solveParams()
{
}

void IntervalFunction::setDefaultAnchors()
{
}

double IntervalFunction::getFx(double x) const
{
    double a = ((_benefit ? _maxY : _minY) - (_benefit ? _minY : _maxY)) / (_maxX - _minX);
    double b = (_benefit ? _minY : _maxY) - a * _minX;
    return a * x + b;
}

QString IntervalFunction::getPython(QString rasterCoverage, QString outputName)
{
    QString result = QString("%1=%2\n").arg(outputName).arg(getMapcalc(rasterCoverage));
    return result;
}

QString IntervalFunction::getMapcalc(QString rasterCoverage)
{
    double a = ((_benefit ? _maxY : _minY) - (_benefit ? _minY : _maxY)) / (_maxX - _minX);
    double b = (_benefit ? _minY : _maxY) - a * _minX;

    //return a * x + b;
    if (a != 0) {
        if (b != 0)
            return QString ("%1*%2+%3").arg(a).arg(rasterCoverage).arg(b);
        else
            return QString("%1*%2").arg(a).arg(rasterCoverage);
    } else {
        return QString("%1").arg(b);
    }
}

/* ******************************************************* */

GoalFunction::GoalFunction(Node *node, QList<Anchor*> & anchors, double minX, double maxX, double minY, double maxY, bool benefit)
: SmceFunction(node, anchors, 2, minX, maxX, minY, maxY, benefit)
{

}

void GoalFunction::solveParams()
{
    _anchors[0]->setY(_benefit ? _minY : _maxY);
    _anchors[1]->setY(_benefit ? _maxY : _minY);

    a = (_anchors[1]->y() - _anchors[0]->y()) / (_anchors[1]->x() - _anchors[0]->x());
    b = _anchors[0]->y() - a * _anchors[0]->x();
}

void GoalFunction::setDefaultAnchors()
{
    _anchors[0]->setX(_minX);
    _anchors[1]->setX(_maxX);
    solveParams();
}

double GoalFunction::getFx(double x) const
{
    if (x < _anchors[0]->x())
        return _benefit ? _minY : _maxY;
    else if (x > _anchors[1]->x())
        return _benefit ? _maxY : _minY;
    else
        return a * x + b;
}

QString GoalFunction::getPython(QString rasterCoverage, QString outputName)
{
    QString result = QString("%1=ilwis.RasterCoverage.min(1,ilwis.RasterCoverage.max(0,%2))\n").arg(outputName).arg(getMapcalc(rasterCoverage));
    return result;
}

QString GoalFunction::getMapcalc(QString rasterCoverage)
{
    solveParams();
    //return a * x + b;
    if (a != 0) {
        if (b != 0)
            return QString ("%1*%2+%3").arg(a).arg(rasterCoverage).arg(b);
        else
            return QString("%1*%2").arg(a).arg(rasterCoverage);
    } else {
        return QString("%1").arg(b);
    }
}

/* ******************************************************* */

ConvexFunction::ConvexFunction(Node *node, QList<Anchor*> & anchors, double minX, double maxX, double minY, double maxY, bool benefit)
: SmceFunction(node, anchors, 3, minX, maxX, minY, maxY, benefit)
{

}

void ConvexFunction::solveParams()
{
    if (_anchors[1]->y() > _maxY - (_maxY - _minY) / 1000.0)
        _anchors[1]->setY(_maxY - (_maxY - _minY) / 1000.0);
    else if (_anchors[1]->y() < _minY + (_maxY - _minY) / 1000.0)
        _anchors[1]->setY(_minY + (_maxY - _minY) / 1000.0);

    if (_benefit)
    {
        // constraints
        _anchors[0]->setY(_minY);
        _anchors[2]->setY(_maxY);

        a = _minY;
        c = log((_anchors[1]->y() - a) / (_maxY - a)) / (_anchors[1]->x() - _anchors[2]->x());
        b = (_anchors[1]->y() - a) / exp(c * _anchors[1]->x());

        double rIntermediateSolution = a + b * exp(c * _anchors[0]->x());
        double rStep = 10;
        int iIterations = 100;
        while (fabs(rIntermediateSolution - _minY) > 0.01 && (--iIterations > 0))
        {
            if (((rIntermediateSolution > _minY) && (rStep > 0)) || ((rIntermediateSolution < _minY) && (rStep < 0)))
                rStep /= -2.0;
            a += rStep;
            c = log((_anchors[1]->y() - a) / (_maxY - a)) / (_anchors[1]->x() - _anchors[2]->x());
            b = (_anchors[1]->y() - a) / exp(c * _anchors[1]->x());

            rIntermediateSolution = a + b * exp(c * _anchors[0]->x());
        }
    } else {
        // constraints
        _anchors[0]->setY(_maxY);
        _anchors[2]->setY(_minY);

        a = -_maxY;
        double rExp = (-a) / (_maxY - a - _anchors[1]->y());
        if (rExp > 0)
            c = -fabs(log((-a) / (_maxY - a - _anchors[1]->y())) / (_anchors[0]->x() - _anchors[1]->x()));
        else
            c = -fabs(log((_maxY - a - _minY) / (_maxY - a - _anchors[1]->y())) / (_anchors[2]->x() - _anchors[1]->x()));

        b = (_maxY - a - _anchors[1]->y()) / exp(c * _anchors[1]->x());

        double rIntermediateSolution = _maxY - (a + b * exp(c * _anchors[2]->x()));

        double rStep = 10;
        int iIterations = 100;
        while ((fabs(rIntermediateSolution - _minY) > 0.01) && (--iIterations > 0))
        {
            if (((rIntermediateSolution < _minY) && (rStep > 0)) || ((rIntermediateSolution > _minY) && (rStep < 0)))
                rStep /= -2.0;

            a += rStep;
            double rExp = (-a) / (_maxY - a - _anchors[1]->y());
            if (rExp > 0)
                c = -fabs(log((-a) / (_maxY - a - _anchors[1]->y())) / (_anchors[0]->x() - _anchors[1]->x()));
            else
                c = -fabs(log((_maxY - a - _minY) / (_maxY - a - _anchors[1]->y())) / (_anchors[2]->x() - _anchors[1]->x()));

            b = (_maxY - a - _anchors[1]->y()) / exp(c * _anchors[1]->x());

            rIntermediateSolution = _maxY - (a + b * exp(c * _anchors[2]->x()));
        }
    }
}

void ConvexFunction::setDefaultAnchors()
{
    if (_benefit)
    {
        _anchors[0]->setX(_minX);
        _anchors[0]->setY(_minY);
        _anchors[1]->setX(_minX + (_maxX - _minX) / 2.0);
        _anchors[1]->setY(_minY + (_maxY - _minY) / 4.0);
        _anchors[2]->setX(_maxX);
        _anchors[2]->setY(_maxY);
    } else {
        _anchors[0]->setX(_minX);
        _anchors[0]->setY(_maxY);
        _anchors[1]->setX(_minX + (_maxX - _minX) / 2.0);
        _anchors[1]->setY(_minY + (_maxY - _minY) / 4.0);
        _anchors[2]->setX(_maxX);
        _anchors[2]->setY(_minY);
    }
    solveParams();
}

double ConvexFunction::getFx(double x) const
{
    double rRet;

    if (_benefit)
        rRet = a + b * exp(c * x);
    else
        rRet = _maxY - (a + b * exp(c * x));

    if (rRet < _minY)
        rRet = _minY;
    else if (rRet > _maxY)
        rRet = _maxY;

    return rRet;
}

QString ConvexFunction::getPython(QString rasterCoverage, QString outputName)
{
    solveParams();
    //return a + b * exp(c * x) OR _maxY - (a + b * exp(c * x))
    QString sRet;
    if (_benefit)
        sRet = QString("%1+%2*ilwis.Engine.do('exp',%3*%4)").arg(a).arg(b).arg(c).arg(rasterCoverage);
    else
        sRet = QString("%1-(%2+%3*ilwis.Engine.do('exp',%4*%5))").arg(_maxY).arg(a).arg(b).arg(c).arg(rasterCoverage);
    QString result = QString("%1=ilwis.RasterCoverage.min(1,ilwis.RasterCoverage.max(0,%2))\n").arg(outputName).arg(sRet);
    return result;
}

QString ConvexFunction::getMapcalc(QString rasterCoverage)
{
    solveParams();
    //return a + b * exp(c * x) OR _maxY - (a + b * exp(c * x))
    QString sRet;
    if (_benefit)
        sRet = QString("%1+%2*exp(%3*%4)").arg(a).arg(b).arg(c).arg(rasterCoverage);
    else
        sRet = QString("%1-(%2+%3*exp(%4*%5))").arg(_maxY).arg(a).arg(b).arg(c).arg(rasterCoverage);

    return sRet;
}

/* ******************************************************* */

ConcaveFunction::ConcaveFunction(Node *node, QList<Anchor*> & anchors, double minX, double maxX, double minY, double maxY, bool benefit)
: SmceFunction(node, anchors, 3, minX, maxX, minY, maxY, benefit)
{

}

void ConcaveFunction::solveParams()
{
    if (_anchors[1]->y() > _maxY - (_maxY - _minY) / 1000.0)
        _anchors[1]->setY(_maxY - (_maxY - _minY) / 1000.0);
    else if (_anchors[1]->y() < _minY + (_maxY - _minY) / 1000.0)
        _anchors[1]->setY(_minY + (_maxY - _minY) / 1000.0);

    if (_benefit)
    {
        // constraints
        _anchors[0]->setY(_minY);
        _anchors[2]->setY(_maxY);

        a = _maxY;
        c = -fabs(log((_anchors[1]->y() - a) / (_minY - a)) / (_anchors[1]->x() - _anchors[0]->x()));
        b = (_anchors[1]->y() - a) / exp(c * _anchors[1]->x());

        double rIntermediateSolution = a + b * exp(c * _anchors[2]->x());
        double rStep = 10;
        int iIterations = 100;
        while (fabs(rIntermediateSolution - _maxY) > 0.01 && (--iIterations > 0))
        {
            if (((rIntermediateSolution > _maxY) && (rStep > 0)) || ((rIntermediateSolution < _maxY) && (rStep < 0)))
                rStep /= -2.0;
            a += rStep;
            c = -fabs(log((_anchors[1]->y() - a) / (_minY - a)) / (_anchors[1]->x() - _anchors[0]->x()));
            b = (_anchors[1]->y() - a) / exp(c * _anchors[1]->x());

            rIntermediateSolution = a + b * exp(c * _anchors[2]->x());
        }
    } else {
        // constraints
        _anchors[0]->setY(_maxY);
        _anchors[2]->setY(_minY);

        a = _minY;
        c = fabs(log((_maxY - a - _minY) / (_maxY - a - _anchors[1]->y())) / (_anchors[2]->x() - _anchors[1]->x()));
        b = (_maxY - a - _anchors[1]->y()) / exp(c * _anchors[1]->x());

        double rIntermediateSolution = _maxY - (a + b * exp(c * _anchors[0]->x()));

        double rStep = 10;
        int iIterations = 100;
        while (fabs(rIntermediateSolution - _maxY) > 0.01 && (--iIterations > 0))
        {
            if (((rIntermediateSolution < _maxY) && (rStep > 0)) || ((rIntermediateSolution > _maxY) && (rStep < 0)))
                rStep /= -2.0;

            a += rStep;
            c = fabs(log((_maxY - a - _minY) / (_maxY - a - _anchors[1]->y())) / (_anchors[2]->x() - _anchors[1]->x()));
            b = (_maxY - a - _anchors[1]->y()) / exp(c * _anchors[1]->x());

            rIntermediateSolution = _maxY - (a + b * exp(c * _anchors[0]->x()));
        }
    }
}

void ConcaveFunction::setDefaultAnchors()
{
    if (_benefit)
    {
        _anchors[0]->setX(_minX);
        _anchors[0]->setY(_minY);
        _anchors[1]->setX(_minX + (_maxX - _minX) / 2.0);
        _anchors[1]->setY(_minY + (_maxY - _minY) / 1.5);
        _anchors[2]->setX(_maxX);
        _anchors[2]->setY(_maxY);
    } else {
        _anchors[0]->setX(_minX);
        _anchors[0]->setY(_maxY);
        _anchors[1]->setX(_minX + (_maxX - _minX) / 2.0);
        _anchors[1]->setY(_minY + (_maxY - _minY) / 1.5);
        _anchors[2]->setX(_maxX);
        _anchors[2]->setY(_minY);
    }
    solveParams();
}

double ConcaveFunction::getFx(double x) const
{
    double rRet;

    if (_benefit)
        rRet = a + b * exp(c * x);
    else
        rRet = _maxY - (a + b * exp(c * x));

    if (rRet < _minY)
        rRet = _minY;
    else if (rRet > _maxY)
        rRet = _maxY;

    return rRet;
}

QString ConcaveFunction::getPython(QString rasterCoverage, QString outputName)
{
    solveParams();
    //return a + b * exp(c * x) OR _maxY - (a + b * exp(c * x))
    QString sRet;
    if (_benefit)
        sRet = QString("%1+%2*ilwis.Engine.do('exp',%3*%4)").arg(a).arg(b).arg(c).arg(rasterCoverage);
    else
        sRet = QString("%1-(%2+%3*ilwis.Engine.do('exp',%4*%5))").arg(_maxY).arg(a).arg(b).arg(c).arg(rasterCoverage);
    QString result = QString("%1=ilwis.RasterCoverage.min(1,ilwis.RasterCoverage.max(0,%2))\n").arg(outputName).arg(sRet);
    return result;
}

QString ConcaveFunction::getMapcalc(QString rasterCoverage)
{
    solveParams();
    //return a + b * exp(c * x) OR _maxY - (a + b * exp(c * x))
    QString sRet;
    if (_benefit)
        sRet = QString("%1+%2*exp(%3*%4)").arg(a).arg(b).arg(c).arg(rasterCoverage);
    else
        sRet = QString("%1-(%2+%3*exp(%4*%5))").arg(_maxY).arg(a).arg(b).arg(c).arg(rasterCoverage);

    return sRet;
}

/* ******************************************************* */

QuadraticFunction::QuadraticFunction(Node *node, QList<Anchor*> & anchors, double minX, double maxX, double minY, double maxY, bool benefit)
: SmceFunction(node, anchors, 3, minX, maxX, minY, maxY, benefit)
{

}

void QuadraticFunction::solveParams()
{
    // constraints
    if (_benefit) {
        _anchors[0]->setY(_minY);
        _anchors[2]->setY(_minY);
    } else {
        // constraints
        _anchors[0]->setY(_maxY);
        _anchors[2]->setY(_maxY);
    }
    double x0 = _anchors[0]->x();
    double y0 = _anchors[0]->y();
    double x1 = _anchors[1]->x();
    double y1 = _anchors[1]->y();
    double x2 = _anchors[2]->x();
    double y2 = _anchors[2]->y();
    double teller = (y2-y0)*(x1-x0)-(y1-y0)*(x2-x0);
    double noemer = (x2*x2-x0*x0)*(x1-x0)-(x1*x1-x0*x0)*(x2-x0);
    a = teller / noemer;
    b = (y1-y0-a*(x1*x1-x0*x0))/(x1-x0);
    c = y0-a*x0*x0-b*x0;
}

void QuadraticFunction::setDefaultAnchors()
{
    if (_benefit)
    {
        _anchors[0]->setX(_minX);
        _anchors[0]->setY(_minY);
        _anchors[1]->setX(_minX + (_maxX - _minX) / 2.0);
        _anchors[1]->setY(_maxY);
        _anchors[2]->setX(_maxX);
        _anchors[2]->setY(_minY);
    } else {
        _anchors[0]->setX(_minX);
        _anchors[0]->setY(_maxY);
        _anchors[1]->setX(_minX + (_maxX - _minX) / 2.0);
        _anchors[1]->setY(_minY);
        _anchors[2]->setX(_maxX);
        _anchors[2]->setY(_maxY);
    }
    solveParams();
}

double QuadraticFunction::getFx(double x) const
{
    double rRet;

    rRet = a * x * x + b * x + c;

    if (rRet < _minY)
        rRet = _minY;
    else if (rRet > _maxY)
        rRet = _maxY;

    return rRet;
}

QString QuadraticFunction::getPython(QString rasterCoverage, QString outputName)
{
    QString result = QString("%1=ilwis.RasterCoverage.min(1,ilwis.RasterCoverage.max(0,%2))\n").arg(outputName).arg(getMapcalc(rasterCoverage));
    return result;
}

QString QuadraticFunction::getMapcalc(QString rasterCoverage)
{
    solveParams();
    // return a * x * x + b * x + c
    // String sRet ("%lg*%S*%S+%lg*%S+%lg", a, sData, sData, b, sData, c);
    // return (a * x + b) * x + c (this is optimized so that sData is used twice instead of 3x)
    QString sRet  = QString("(%1*%2+%3)*%4+%5").arg(a).arg(rasterCoverage).arg(b).arg(rasterCoverage).arg(c);

    return sRet;
}

/* ******************************************************* */

GaussianFunction::GaussianFunction(Node *node, QList<Anchor*> & anchors, double minX, double maxX, double minY, double maxY, bool benefit)
: SmceFunction(node, anchors, 3, minX, maxX, minY, maxY, benefit)
{

}

void GaussianFunction::solveParams()
{
    if (_benefit) {
        _anchors[1]->setX((_anchors[0]->x() + _anchors[2]->x()) / 2.0);
        _anchors[0]->setY(_minY + (_maxY - _minY) / 1000.0);
        _anchors[2]->setY(_minY + (_maxY - _minY) / 1000.0);
        if (_anchors[1]->y() < _minY + (_maxY - _minY) / 500.0)
            _anchors[1]->setY(_minY + (_maxY - _minY) / 500.0);
    } else {
        _anchors[1]->setX((_anchors[0]->x() + _anchors[2]->x()) / 2.0);
        _anchors[0]->setY(_maxY - (_maxY - _minY) / 1000.0);
        _anchors[2]->setY(_maxY - (_maxY - _minY) / 1000.0);
        if (_anchors[1]->y() > _maxY - (_maxY - _minY) / 500.0)
            _anchors[1]->setY(_maxY - (_maxY - _minY) / 500.0);
    }
    double x1 = _anchors[1]->x();
    double y1 = _anchors[1]->y();
    double x2 = _anchors[2]->x();
    double y2 = _anchors[2]->y();
    if (_benefit) {
        a = y1;
        c = x1;
        b = log(y2 / y1) / pow(x2 - c, 2);
    } else {
        a = _maxY - y1;
        c = x1;
        b = log((_maxY - y2) / (_maxY - y1)) / pow(x2 - c, 2);
    }
}

void GaussianFunction::setDefaultAnchors()
{
    if (_benefit)
    {
        _anchors[0]->setX(_minX);
        _anchors[1]->setX(_minX + (_maxX - _minX) / 2.0);
        _anchors[1]->setY(_maxY);
        _anchors[2]->setX(_maxX);
    } else {
        _anchors[0]->setX(_minX);
        _anchors[1]->setX(_minX + (_maxX - _minX) / 2.0);
        _anchors[1]->setY(_minY);
        _anchors[2]->setX(_maxX);
    }
    solveParams();
}

double GaussianFunction::getFx(double x) const
{
    double rRet;

    if (_benefit)
        rRet = a * exp(b * pow(x - c, 2));
    else
        rRet = _maxY - a * exp(b * pow(x - c, 2));

    if (rRet < _minY)
        rRet = _minY;
    else if (rRet > _maxY)
        rRet = _maxY;

    return rRet;
}

QString GaussianFunction::getPython(QString rasterCoverage, QString outputName)
{
    solveParams();
    // return a * exp(b * sq(x - c)) OR 1 - a * exp(b * sq(x - c))
    QString sRet;
    if (_benefit)
        sRet = QString("%1*ilwis.Engine.do('exp',%2*ilwis.Engine.do('binarymathraster',%3-%4,2,'power'))").arg(a).arg(b).arg(rasterCoverage).arg(c);
    else
        sRet = QString("%1-%2*ilwis.Engine.do('exp',%3*ilwis.Engine.do('binarymathraster',%4-%5,2,'power'))").arg(_maxY).arg(a).arg(b).arg(rasterCoverage).arg(c);
    QString result = QString("%1=ilwis.RasterCoverage.min(1,ilwis.RasterCoverage.max(0,%2))\n").arg(outputName).arg(sRet);
    return result;
}

QString GaussianFunction::getMapcalc(QString rasterCoverage)
{
    solveParams();
    // return a * exp(b * sq(x - c)) OR 1 - a * exp(b * sq(x - c))
    QString sRet;
    if (_benefit)
        sRet = QString("%1*exp(%2*sq(%3-%4))").arg(a).arg(b).arg(rasterCoverage).arg(c);
    else
        sRet = QString("%1-%2*exp(%3*sq(%4-%5))").arg(_maxY).arg(a).arg(b).arg(rasterCoverage).arg(c);

    return sRet;
}

/* ******************************************************* */

PiecewiseLinear8Function::PiecewiseLinear8Function(Node *node, QList<Anchor*> & anchors, double minX, double maxX, double minY, double maxY, bool benefit)
: SmceFunction(node, anchors, 9, minX, maxX, minY, maxY, benefit)
{

}

void PiecewiseLinear8Function::solveParams()
{
    _anchors[0]->setX(_minX);
    _anchors[8]->setX(_maxX);
    for (int i = 0; i < 9; ++i) {
        if (_anchors[i]->y() < _minY)
            _anchors[i]->setY(_minY);
        else if (_anchors[i]->y() > _maxY)
            _anchors[i]->setY(_maxY);
    }

    a1 = (_anchors[1]->y() - _anchors[0]->y()) / (_anchors[1]->x() - _anchors[0]->x());
    b1 = _anchors[0]->y() - a1 * _anchors[0]->x();

    a2 = (_anchors[2]->y() - _anchors[1]->y()) / (_anchors[2]->x() - _anchors[1]->x());
    b2 = _anchors[1]->y() - a2 * _anchors[1]->x();

    a3 = (_anchors[3]->y() - _anchors[2]->y()) / (_anchors[3]->x() - _anchors[2]->x());
    b3 = _anchors[2]->y() - a3 * _anchors[2]->x();

    a4 = (_anchors[4]->y() - _anchors[3]->y()) / (_anchors[4]->x() - _anchors[3]->x());
    b4 = _anchors[3]->y() - a4 * _anchors[3]->x();

    a5 = (_anchors[5]->y() - _anchors[4]->y()) / (_anchors[5]->x() - _anchors[4]->x());
    b5 = _anchors[4]->y() - a5 * _anchors[4]->x();

    a6 = (_anchors[6]->y() - _anchors[5]->y()) / (_anchors[6]->x() - _anchors[5]->x());
    b6 = _anchors[5]->y() - a6 * _anchors[5]->x();

    a7 = (_anchors[7]->y() - _anchors[6]->y()) / (_anchors[7]->x() - _anchors[6]->x());
    b7 = _anchors[6]->y() - a7 * _anchors[6]->x();

    a8 = (_anchors[8]->y() - _anchors[7]->y()) / (_anchors[8]->x() - _anchors[7]->x());
    b8 = _anchors[7]->y() - a8 * _anchors[7]->x();
}

void PiecewiseLinear8Function::setDefaultAnchors()
{
    _anchors[1]->setX(_minX + (_maxX - _minX) / 8.0);
    _anchors[2]->setX(_minX + (_maxX - _minX) * 2.0 / 8.0);
    _anchors[3]->setX(_minX + (_maxX - _minX) * 3.0 / 8.0);
    _anchors[4]->setX(_minX + (_maxX - _minX) * 4.0 / 8.0);
    _anchors[5]->setX(_minX + (_maxX - _minX) * 5.0 / 8.0);
    _anchors[6]->setX(_minX + (_maxX - _minX) * 6.0 / 8.0);
    _anchors[7]->setX(_minX + (_maxX - _minX) * 7.0 / 8.0);
    if (_benefit)
    {
        _anchors[0]->setY(_minY);
        _anchors[1]->setY(_minY + (_maxY - _minY) / 8.0);
        _anchors[2]->setY(_minY + (_maxY - _minY) * 2.0 / 8.0);
        _anchors[3]->setY(_minY + (_maxY - _minY) * 3.0 / 8.0);
        _anchors[4]->setY(_minY + (_maxY - _minY) * 4.0 / 8.0);
        _anchors[5]->setY(_minY + (_maxY - _minY) * 5.0 / 8.0);
        _anchors[6]->setY(_minY + (_maxY - _minY) * 6.0 / 8.0);
        _anchors[7]->setY(_minY + (_maxY - _minY) * 7.0 / 8.0);
        _anchors[8]->setY(_maxY);
    }
    else
    {
        _anchors[0]->setY(_maxY);
        _anchors[1]->setY(_minY + (_maxY - _minY) * 7.0 / 8.0);
        _anchors[2]->setY(_minY + (_maxY - _minY) * 6.0 / 8.0);
        _anchors[3]->setY(_minY + (_maxY - _minY) * 5.0 / 8.0);
        _anchors[4]->setY(_minY + (_maxY - _minY) * 4.0 / 8.0);
        _anchors[5]->setY(_minY + (_maxY - _minY) * 3.0 / 8.0);
        _anchors[6]->setY(_minY + (_maxY - _minY) * 2.0 / 8.0);
        _anchors[7]->setY(_minY + (_maxY - _minY) / 8.0);
        _anchors[8]->setY(_minY);
    }
    solveParams();
}

double PiecewiseLinear8Function::getFx(double x) const
{
    if (x < _anchors[1]->x())
        return a1 * x + b1;
    else if (x < _anchors[2]->x())
        return a2 * x + b2;
    else if (x < _anchors[3]->x())
        return a3 * x + b3;
    else if (x < _anchors[4]->x())
        return a4 * x + b4;
    else if (x < _anchors[5]->x())
        return a5 * x + b5;
    else if (x < _anchors[6]->x())
        return a6 * x + b6;
    else if (x < _anchors[7]->x())
        return a7 * x + b7;
    else
        return a8 * x + b8;
}

QString PiecewiseLinear8Function::getLine(double a, QString x, double b)
{
    //return a * x + b;
    QString result;
    if (a != 0)
        result = QString("%1*%2").arg(a).arg(x);
    if (b != 0)
        result += ((b > 0) ? "+":"") + QString("%1").arg(b);
    return result;
}

QString PiecewiseLinear8Function::getPython(QString rasterCoverage, QString outputName)
{
    solveParams();
    QString result = QString("%1=%2\n").arg(outputName).arg(getLine(a8,rasterCoverage,b8));
    result += QString("%1_term2=%2\n").arg(outputName).arg(getLine(a7,rasterCoverage,b7));
    result += QString("%1=ilwis.Engine.do('iffraster',%2<%3,%1_term2,%1)\n").arg(outputName).arg(rasterCoverage).arg(_anchors[7]->x());
    result += QString("%1_term2=%2\n").arg(outputName).arg(getLine(a6,rasterCoverage,b6));
    result += QString("%1=ilwis.Engine.do('iffraster',%2<%3,%1_term2,%1)\n").arg(outputName).arg(rasterCoverage).arg(_anchors[6]->x());
    result += QString("%1_term2=%2\n").arg(outputName).arg(getLine(a5,rasterCoverage,b5));
    result += QString("%1=ilwis.Engine.do('iffraster',%2<%3,%1_term2,%1)\n").arg(outputName).arg(rasterCoverage).arg(_anchors[5]->x());
    result += QString("%1_term2=%2\n").arg(outputName).arg(getLine(a4,rasterCoverage,b4));
    result += QString("%1=ilwis.Engine.do('iffraster',%2<%3,%1_term2,%1)\n").arg(outputName).arg(rasterCoverage).arg(_anchors[4]->x());
    result += QString("%1_term2=%2\n").arg(outputName).arg(getLine(a3,rasterCoverage,b3));
    result += QString("%1=ilwis.Engine.do('iffraster',%2<%3,%1_term2,%1)\n").arg(outputName).arg(rasterCoverage).arg(_anchors[3]->x());
    result += QString("%1_term2=%2\n").arg(outputName).arg(getLine(a2,rasterCoverage,b2));
    result += QString("%1=ilwis.Engine.do('iffraster',%2<%3,%1_term2,%1)\n").arg(outputName).arg(rasterCoverage).arg(_anchors[2]->x());
    result += QString("%1_term2=%2\n").arg(outputName).arg(getLine(a1,rasterCoverage,b1));
    result += QString("%1=ilwis.Engine.do('iffraster',%2<%3,%1_term2,%1)\n").arg(outputName).arg(rasterCoverage).arg(_anchors[1]->x());
    result += QString("del %1_term2\n").arg(outputName);
    return result;
}

QString PiecewiseLinear8Function::getMapcalc(QString rasterCoverage)
{
    solveParams();
    QString cond1 = QString("ilwis.Engine.do('binarylogicalraster',%1,%2,'less')").arg(rasterCoverage).arg(_anchors[1]->x());
    QString cond2 = QString("ilwis.Engine.do('binarylogicalraster',%1,%2,'less')").arg(rasterCoverage).arg(_anchors[2]->x());
    QString cond3 = QString("ilwis.Engine.do('binarylogicalraster',%1,%2,'less')").arg(rasterCoverage).arg(_anchors[3]->x());
    QString cond4 = QString("ilwis.Engine.do('binarylogicalraster',%1,%2,'less')").arg(rasterCoverage).arg(_anchors[4]->x());
    QString cond5 = QString("ilwis.Engine.do('binarylogicalraster',%1,%2,'less')").arg(rasterCoverage).arg(_anchors[5]->x());
    QString cond6 = QString("ilwis.Engine.do('binarylogicalraster',%1,%2,'less')").arg(rasterCoverage).arg(_anchors[6]->x());
    QString cond7 = QString("ilwis.Engine.do('binarylogicalraster',%1,%2,'less')").arg(rasterCoverage).arg(_anchors[7]->x());
    QString iff7 = QString("ilwis.Engine.do('iffraster',%1,%2,%3)").arg(cond7).arg(getLine(a7,rasterCoverage,b7)).arg(getLine(a8,rasterCoverage,b8));
    QString iff6 = QString("ilwis.Engine.do('iffraster',%1,%2,%3)").arg(cond6).arg(getLine(a6,rasterCoverage,b6)).arg(iff7);
    QString iff5 = QString("ilwis.Engine.do('iffraster',%1,%2,%3)").arg(cond5).arg(getLine(a5,rasterCoverage,b5)).arg(iff6);
    QString iff4 = QString("ilwis.Engine.do('iffraster',%1,%2,%3)").arg(cond4).arg(getLine(a4,rasterCoverage,b4)).arg(iff5);
    QString iff3 = QString("ilwis.Engine.do('iffraster',%1,%2,%3)").arg(cond3).arg(getLine(a3,rasterCoverage,b3)).arg(iff4);
    QString iff2 = QString("ilwis.Engine.do('iffraster',%1,%2,%3)").arg(cond2).arg(getLine(a2,rasterCoverage,b2)).arg(iff3);
    QString iff1 = QString("ilwis.Engine.do('iffraster',%1,%2,%3)").arg(cond1).arg(getLine(a1,rasterCoverage,b1)).arg(iff2);
    return iff1;
}

/* ******************************************************* */

Standardization::Standardization()
{

}

Standardization::Standardization(Node *node)
: QObject(node)
, _node(node)
{

}

Standardization * Standardization::create(Node * node)
{
    if(node) {
        QString fileName = node->fileName();
        if (fileName != "") {
            try {
                Ilwis::IRasterCoverage rc(fileName, itRASTER);
                Ilwis::IDomain dom = rc->datadefRef().domain<>();
                if (hasType(dom->valueType(), itNUMBER)) {
                    Ilwis::NumericRange * range = rc->datadefRef().range()->as<Ilwis::NumericRange>();
                    double min = range->min();
                    double max = range->max();
                    if (node->type() == Node::NodeType::Factor) {
                        StandardizationValue * stdValue = new StandardizationValue(node, min, max);
                        stdValue->setMethod(StandardizationValue::Maximum, true);
                        return stdValue;
                    }
                    else if (node->type() == Node::NodeType::Constraint)
                        return new StandardizationValueConstraint(node, min, max);
                    else
                        return 0;
                } else if (hasType(dom->valueType(), itTHEMATICITEM | itNAMEDITEM | itINDEXEDITEM | itNUMERICITEM)) {
                    if (node->type() == Node::NodeType::Factor)
                        return new StandardizationClass(node, false);
                    else if (node->type() == Node::NodeType::Constraint)
                        return new StandardizationClass(node, true);
                    else
                        return 0;
                } else if (hasType(dom->valueType(), itBOOL)) {
                    if (node->type() == Node::NodeType::Factor)
                        return new StandardizationBool(node);
                    else if (node->type() == Node::NodeType::Constraint)
                        return new StandardizationBoolConstraint(node);
                    else
                        return 0;
                } else
                    return 0;
            } catch (const ErrorObject& err){
                kernel()->issues()->log(QString(TR("SMCE: Error opening file '%1'. Cause: '%2'")).arg(fileName).arg(err.message()), IssueObject::itError);
            }
        }
    }
    return 0;
}

QString Standardization::getPython(QString rasterCoverage, QString outputName) const
{
    return outputName + "=" + rasterCoverage + "\n";
}

QString Standardization::getMapcalc(QString rasterCoverage) const
{
    return rasterCoverage;
}

int Standardization::type() const
{
    return Standardization::StandardizationType::None;
}

StandardizationValue * Standardization::pStandardizationValue()
{
    return 0;
}

StandardizationValueConstraint * Standardization::pStandardizationValueConstraint()
{
    return 0;
}

StandardizationClass * Standardization::pStandardizationClass()
{
    return 0;
}

StandardizationBool * Standardization::pStandardizationBool()
{
    return 0;
}

StandardizationBoolConstraint * Standardization::pStandardizationBoolConstraint()
{
    return 0;
}

void Standardization::apply()
{
    _node->setStandardization(this);
}

Standardization * Standardization::clone() const
{
    return new Standardization(_node);
}

void Standardization::store(QDataStream &stream, Standardization * standardization)
{
    if (standardization != 0) {
        stream << (quint8)standardization->type();
        standardization->store(stream);
    } else
        stream << (quint8)StandardizationType::None;
}

void Standardization::load(QDataStream &stream, Node * node)
{
    quint8 type;
    stream >> type;
    if (type != 0) {
        Standardization * standardization = create(node);
        if (standardization) { // corresponding rastermap exists and is valid
            standardization->load(stream);
            standardization->apply();
        } else { // rastermap does not exist, or became invalid since last time; "consume" the standardization data ("load") but discard it (skip "apply")
            StandardizationType stdType = static_cast<StandardizationType>(type);
            switch(stdType) {
            case Value:
                {
                    StandardizationValue * stdValue = new StandardizationValue(node, 0, 1);
                    stdValue->setMethod(StandardizationValue::Maximum, true);
                    standardization = stdValue;
                }
                break;
            case ValueConstraint:
                standardization = new StandardizationValueConstraint(node, 0, 1);
                break;
            case Class:
                standardization = new StandardizationClass(node, false);
                break;
            case ClassConstraint:
                standardization = new StandardizationClass(node, true);
                break;
            case Bool:
                standardization = new StandardizationBool(node);
                break;
            case BoolConstraint:
                standardization = new StandardizationBoolConstraint(node);
                break;
            default:
                break;
            }
            if (standardization) {
                standardization->load(stream);
                delete standardization;
            }
        }
    }
}

void Standardization::store(QDataStream &stream)
{

}

void Standardization::load(QDataStream &stream)
{

}

/* ******************************************************* */

Anchor::Anchor()
{

}

Anchor::Anchor(Node *node, double x, double y)
: QObject(node)
, _x(x)
, _y(y)
{

}

double Anchor::x() const
{
    return _x;
}

double Anchor::y() const
{
    return _y;
}
void Anchor::setX(double x)
{
    _x = x;
    emit xChanged();
}

void Anchor::setY(double y)
{
    _y = y;
    emit yChanged();
}

/* ******************************************************* */

StandardizationValue::StandardizationValue()
: Standardization()
, _stdValueMethod(0)
, _min(0)
, _max(-1)
, _method(StandardizationValueMethodType::None)
, _benefit(true)
{
}

StandardizationValue::StandardizationValue(Node *node, double min, double max)
: Standardization(node)
, _stdValueMethod(0)
, _min(min)
, _max(max)
, _method(StandardizationValueMethodType::None)
, _benefit(true)
{
}

StandardizationValue::~StandardizationValue()
{
    delete _stdValueMethod;
}

double StandardizationValue::min() const
{
    return _min;
}

double StandardizationValue::max() const
{
    return _max;
}

int StandardizationValue::method() const
{
    return _method;
}

bool StandardizationValue::benefit() const
{
    return _benefit;
}

void StandardizationValue::setMethod(StandardizationValueMethodType method, bool benefit)
{
    if (_stdValueMethod)
        delete _stdValueMethod;
    _method = method;
    _benefit = benefit;
    _stdValueMethod = StdValueMethod::create(_node, _anchors, _min, _max, _method, _benefit);
    emit methodChanged();
    emit benefitChanged();
    emit anchorsChanged();
}

QQmlListProperty<Anchor> StandardizationValue::anchors()
{
    return QQmlListProperty<Anchor>(this, _anchors);
}

void StandardizationValue::solveParams()
{
    _stdValueMethod->solveParams();
}

void StandardizationValue::setAnchor(double x, double y)
{
    _stdValueMethod->setAnchor(x, y);
}

double StandardizationValue::getFx(double x) const
{
    if (_stdValueMethod)
        return _stdValueMethod->getFx(x);
    else
        return 0;
}

QString StandardizationValue::getPython(QString rasterCoverage, QString outputName) const
{
    return _stdValueMethod->getPython(rasterCoverage, outputName);
}

QString StandardizationValue::getMapcalc(QString rasterCoverage) const
{
    return _stdValueMethod->getMapcalc(rasterCoverage);
}

int StandardizationValue::type() const
{
    return Standardization::StandardizationType::Value;
}

StandardizationValue * StandardizationValue::pStandardizationValue()
{
    return this;
}

Standardization * StandardizationValue::clone() const
{
    StandardizationValue * stdClone = new StandardizationValue(_node, _min, _max);
    stdClone->setMethod(_method, _benefit);
    for (int i = 0; i < _anchors.length(); ++i) {
        stdClone->_anchors.at(i)->setX(_anchors.at(i)->x());
        stdClone->_anchors.at(i)->setY(_anchors.at(i)->y());
    }
    stdClone->solveParams();
    return stdClone;
}

void StandardizationValue::store(QDataStream &stream)
{
    stream << (quint8)_method;
    stream << _benefit;
    for (Anchor * anchor : _anchors) {
        stream << anchor->x();
        stream << anchor->y();
    }
}

void StandardizationValue::load(QDataStream &stream)
{
    quint8 method;
    bool benefit = true;
    stream >> method >> benefit;
    setMethod(static_cast<StandardizationValue::StandardizationValueMethodType>(method), benefit);
    for (Anchor * anchor : _anchors) {
        double x;
        double y;
        stream >> x;
        stream >> y;
        anchor->setX(x);
        anchor->setY(y);
    }
    solveParams();
}

/* ******************************************************* */

StdValueMethod::StdValueMethod()
: _min(0)
, _max(0)
{

}

StdValueMethod::StdValueMethod(Node *node, double min, double max)
: _node(node)
, _min(min)
, _max(max)
{

}

StdValueMethod::~StdValueMethod()
{

}

StdValueMethod * StdValueMethod::create(Node * node, QList<Anchor *> & anchors, double min, double max, StandardizationValue::StandardizationValueMethodType method, bool benefit)
{
    switch (method) {
    case StandardizationValue::Maximum:
    case StandardizationValue::Interval:
    case StandardizationValue::Goal:
    case StandardizationValue::Convex:
    case StandardizationValue::Concave:
    case StandardizationValue::Ushape:
    case StandardizationValue::Gaussian:
    case StandardizationValue::PiecewiseLinear8:
        return new StdValueGeneral(node, anchors, min, max, method, benefit);
        break;
    default:
        return 0;
    }
}

/* ******************************************************* */

StdValueGeneral::StdValueGeneral(Node *node, QList<Anchor*> & anchors, double min, double max, StandardizationValue::StandardizationValueMethodType method, bool benefit)
: StdValueMethod(node, min, max)
{
    switch (method) {
    case StandardizationValue::None:
        _function = 0;
        break;
    case StandardizationValue::Maximum:
        _function = new MaximumFunction(node, anchors, min, max, 0, 1, benefit);
        break;
    case StandardizationValue::Interval:
        _function = new IntervalFunction(node, anchors, min, max, 0, 1, benefit);
        break;
    case StandardizationValue::Goal:
        _function = new GoalFunction(node, anchors, min, max, 0, 1, benefit);
        break;
    case StandardizationValue::Convex:
        _function = new ConvexFunction(node, anchors, min, max, 0, 1, benefit);
        break;
    case StandardizationValue::Concave:
        _function = new ConcaveFunction(node, anchors, min, max, 0, 1, benefit);
        break;
    case StandardizationValue::Ushape:
        _function = new QuadraticFunction(node, anchors, min, max, 0, 1, benefit);
        break;
    case StandardizationValue::Gaussian:
        _function = new GaussianFunction(node, anchors, min, max, 0, 1, benefit);
        break;
    case StandardizationValue::PiecewiseLinear8:
        _function = new PiecewiseLinear8Function(node, anchors, min, max, 0, 1, benefit);
        break;
    }
    if (_function != 0)
        _function->setDefaultAnchors();
}

StdValueGeneral::~StdValueGeneral()
{
    delete _function;
}

void StdValueGeneral::solveParams()
{
    _function->solveParams();
}

void StdValueGeneral::setAnchor(double x, double y)
{
    _function->setAnchor(x, y);
}

double StdValueGeneral::getFx(double x) const
{
    return _function->getFx(x);
}

QString StdValueGeneral::getPython(QString rasterCoverage, QString outputName) const
{
    return _function->getPython(rasterCoverage, outputName);
}

QString StdValueGeneral::getMapcalc(QString rasterCoverage) const
{
    return _function->getMapcalc(rasterCoverage);
}

/* ******************************************************* */

StandardizationValueConstraint::StandardizationValueConstraint()
: Standardization()
, _min(0)
, _max(-1)
, _minVal(0)
, _maxVal(-1)
, _useMin(false)
, _useMax(false)
{
}

StandardizationValueConstraint::StandardizationValueConstraint(Node *node, double min, double max)
: Standardization(node)
, _min(min)
, _max(max)
{
    _useMin = true;
    _useMax = false;
    _minVal = (_min + _max) / 2.0;
    _maxVal = _minVal;
}

double StandardizationValueConstraint::min() const
{
    return _min;
}

double StandardizationValueConstraint::max() const
{
    return _max;
}

double StandardizationValueConstraint::minVal() const
{
    return _minVal;
}

double StandardizationValueConstraint::maxVal() const
{
    return _maxVal;
}

void StandardizationValueConstraint::setMinVal(double minVal)
{
    _minVal = minVal;
    emit minValChanged();
}

void StandardizationValueConstraint::setMaxVal(double maxVal)
{
    _maxVal = maxVal;
    emit maxValChanged();
}

bool StandardizationValueConstraint::useMin() const
{
    return _useMin;
}

bool StandardizationValueConstraint::useMax() const
{
    return _useMax;
}

void StandardizationValueConstraint::setUseMin(bool useMin)
{
    _useMin = useMin;
    emit useMinChanged();
}

void StandardizationValueConstraint::setUseMax(bool useMax)
{
    _useMax = useMax;
    emit useMaxChanged();
}

QString StandardizationValueConstraint::getPython(QString rasterCoverage, QString outputName) const
{
    return outputName + "=" + getMapcalc(rasterCoverage) + "\n";
}

QString StandardizationValueConstraint::getMapcalc(QString rasterCoverage) const
{
    // cases:
    // !_useMin && !_useMax (no constraint?)
    // _useMin && !_useMax
    // !_useMin && _useMax
    // _useMin && _useMax && rMin <= rMax
    // _useMin && _useMax && rMin > rMax
    if (!(_useMin || _useMax))
        return rasterCoverage;
    else if (_useMin && !_useMax)
        return QString("%1>=%2").arg(rasterCoverage).arg(_minVal);
    else if (!_useMin && _useMax)
        return QString("%1<=%2").arg(rasterCoverage).arg(_maxVal);
    else // (_useMin && _useMax)
    {
        if (_minVal <= _maxVal)
            return QString("(%1>=%2) & (%1<=%3)").arg(rasterCoverage).arg(_minVal).arg(_maxVal);
        else // _maxVal < _minVal
            return QString("(%1<=%2) | (%1>=%3)").arg(rasterCoverage).arg(_maxVal).arg(_minVal);
    }
}

int StandardizationValueConstraint::type() const
{
    return Standardization::StandardizationType::ValueConstraint;
}

StandardizationValueConstraint * StandardizationValueConstraint::pStandardizationValueConstraint()
{
    return this;
}

Standardization * StandardizationValueConstraint::clone() const
{
    StandardizationValueConstraint * stdClone = new StandardizationValueConstraint(_node, _min, _max);
    stdClone->_minVal = _minVal;
    stdClone->_maxVal = _maxVal;
    stdClone->_useMin = _useMin;
    stdClone->_useMax = _useMax;
    return stdClone;
}

void StandardizationValueConstraint::store(QDataStream &stream)
{
    stream << _minVal;
    stream << _maxVal;
    stream << _useMin;
    stream << _useMax;
}

void StandardizationValueConstraint::load(QDataStream &stream)
{
    stream >> _minVal;
    stream >> _maxVal;
    stream >> _useMin;
    stream >> _useMax;
}

/* ******************************************************* */

StandardizationClass::StandardizationClass()
: Standardization()
, _constraint(false)
{
}

StandardizationClass::StandardizationClass(Node *node, bool constraint)
: Standardization(node)
, _constraint(constraint)
{
}

QString StandardizationClass::getPython(QString rasterCoverage, QString outputName) const
{
    return outputName + "=" + rasterCoverage + "\n";
}

QString StandardizationClass::getMapcalc(QString rasterCoverage) const
{
    return rasterCoverage;
}

int StandardizationClass::type() const
{
    return _constraint ? Standardization::StandardizationType::ClassConstraint : Standardization::StandardizationType::Class;
}

StandardizationClass * StandardizationClass::pStandardizationClass()
{
    return this;
}

Standardization * StandardizationClass::clone() const
{
    return new StandardizationClass(_node, _constraint);
}

void StandardizationClass::store(QDataStream &stream)
{

}

void StandardizationClass::load(QDataStream &stream)
{

}

/* ******************************************************* */

StandardizationBool::StandardizationBool()
: Standardization()
{
}

StandardizationBool::StandardizationBool(Node *node)
: Standardization(node)
{
}

QString StandardizationBool::getPython(QString rasterCoverage, QString outputName) const
{
    return outputName + "=" + rasterCoverage + "\n";
}

QString StandardizationBool::getMapcalc(QString rasterCoverage) const
{
    return rasterCoverage;
}

int StandardizationBool::type() const
{
    return Standardization::StandardizationType::Bool;
}

StandardizationBool * StandardizationBool::pStandardizationBool()
{
    return this;
}

Standardization * StandardizationBool::clone() const
{
    return new StandardizationBool(_node);
}

void StandardizationBool::store(QDataStream &stream)
{

}

void StandardizationBool::load(QDataStream &stream)
{

}

/* ******************************************************* */

StandardizationBoolConstraint::StandardizationBoolConstraint()
: Standardization()
{
}

StandardizationBoolConstraint::StandardizationBoolConstraint(Node *node)
: Standardization(node)
{
}

QString StandardizationBoolConstraint::getPython(QString rasterCoverage, QString outputName) const
{
    return outputName + "=" + rasterCoverage + "\n";
}

QString StandardizationBoolConstraint::getMapcalc(QString rasterCoverage) const
{
    return rasterCoverage;
}

int StandardizationBoolConstraint::type() const
{
    return Standardization::StandardizationType::BoolConstraint;
}

StandardizationBoolConstraint * StandardizationBoolConstraint::pStandardizationBoolConstraint()
{
    return this;
}

Standardization * StandardizationBoolConstraint::clone() const
{
    return new StandardizationBoolConstraint(_node);
}

void StandardizationBoolConstraint::store(QDataStream &stream)
{

}

void StandardizationBoolConstraint::load(QDataStream &stream)
{

}
