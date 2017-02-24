#include "../../IlwisCore/core/ilwisobjects/coverage/rastercoverage.h"

#include "node.h"
#include <QUrl>

Node::Node()
{

}

Node::Node(QObject *qparent)
: QObject(qparent)
, _parent(0)
, _weight(0)
, _weights(0)
, _standardization(0)
{

}

Node::Node(Node * parent, QObject *qparent)
: QObject(qparent)
, _parent(parent)
, _weight(0)
, _weights(0)
, _standardization(0)
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
    if (_type == NodeType::Group && subFactors().size() > 1) {
        if (!_weights)
            _weights = Weights::create(this);
        return _weights;
    } else
        return 0;
}

Standardization * Node::standardization()
{
    if (_type == NodeType::Factor || _type == NodeType::Constraint) {
        if (!_standardization)
            _standardization = Standardization::create(this);
        return _standardization;
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

QList <Node*> Node::subFactors()
{
    QList<Node*> result;
    for (Node * node : _subNodes)
        if (node->type() == NodeType::Group || node->type() == NodeType::Factor)
            result.append(node);
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
    if (subFactors().size() > 1) {
        for (Node* node : subFactors()) {
            node->setWeight(0);
        }
    } else if (subFactors().size() == 1) {
        subFactors()[0]->setWeight(1);
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
    for (Node * child : _node->subFactors())
        if (getItem(child))
            child->setWeight(getItem(child)->normalizedWeight());
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

void SmceFunction::SetAnchor(double x, double y)
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
    SolveParams();
}

/* ******************************************************* */

PiecewiseLinear8Function::PiecewiseLinear8Function(Node *node, QList<Anchor*> & anchors, double minX, double maxX, double minY, double maxY, bool benefit)
: SmceFunction(node, anchors, 9, minX, maxX, minY, maxY, benefit)
{

}

void PiecewiseLinear8Function::SolveParams()
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

void PiecewiseLinear8Function::SetDefaultAnchors()
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
    SolveParams();
}

QString PiecewiseLinear8Function::sGetFx(QString sData)
{
    SolveParams();
    //return a * x + b;
    return sData; // QString("iff(%S<%lg,%lg*%S+%lg,iff(%S<%lg,%lg*%S+%lg,iff(%S<%lg,%lg*%S+%lg,iff(%S<%lg,%lg*%S+%lg,%lg*%S+%lg))))", sData, _anchors[1].first, a1, sData, b1, sData, _anchors[2].first, a2, sData, b2, sData, _anchors[3].first, a3, sData, b3, sData, _anchors[4].first, a4, sData, b4, a5, sData, b5);
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
            Ilwis::IRasterCoverage rc(fileName, itRASTER);
            Ilwis::IDomain dom = rc->datadefRef().domain<>();
            if (hasType(dom->valueType(), itNUMBER)) {
                Ilwis::NumericRange * range = rc->datadefRef().range()->as<Ilwis::NumericRange>();
                double min = range->min();
                double max = range->max();
                if (node->type() == Node::NodeType::Factor)
                    return new StandardizationValue(node, min, max);
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
        }
    }
    return 0;
}

int Standardization::type() const
{
    return Standardization::StandardizationType::None;
}

StandardizationValue * Standardization::pStandardizationValue()
{
    return 0;
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
, _methodType(StandardizationValueMethodType::Maximum)
{
}

StandardizationValue::StandardizationValue(Node *node, double min, double max)
: Standardization(node)
, _min(min)
, _max(max)
, _methodType(StandardizationValueMethodType::PiecewiseLinear8)
{
    _stdValueMethod = StdValueMethod::create(node, _anchors, min, max, _methodType);
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

int StandardizationValue::methodType() const
{
    return _methodType;
}

QQmlListProperty<Anchor> StandardizationValue::anchors()
{
    return QQmlListProperty<Anchor>(this, _anchors);
}

void StandardizationValue::SolveParams()
{
    _stdValueMethod->SolveParams();
}

void StandardizationValue::SetAnchor(double x, double y)
{
    _stdValueMethod->SetAnchor(x, y);
}

int StandardizationValue::type() const
{
    return Standardization::StandardizationType::Value;
}

StandardizationValue * StandardizationValue::pStandardizationValue()
{
    return this;
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

StdValueMethod * StdValueMethod::create(Node * node, QList<Anchor *> & anchors, double min, double max, StandardizationValue::StandardizationValueMethodType method)
{
    switch (method) {
    case StandardizationValue::PiecewiseLinear8:
        return new StdValueGeneral(node, anchors, min, max, method);
        break;
    default:
        return 0;
    }
}

/* ******************************************************* */

StdValueGeneral::StdValueGeneral(Node *node, QList<Anchor*> & anchors, double min, double max, StandardizationValue::StandardizationValueMethodType method)
: StdValueMethod(node, min, max)
{
    switch (method) {
    case StandardizationValue::Maximum:
    case StandardizationValue::Interval:
    case StandardizationValue::Goal:
    case StandardizationValue::Convex:
    case StandardizationValue::Concave:
    case StandardizationValue::Ushape:
    case StandardizationValue::Gaussian:
        _function = 0;
        break;
    case StandardizationValue::PiecewiseLinear8:
        _function = new PiecewiseLinear8Function(node, anchors, min, max, 0, 1, true);
        break;
    }
    if (_function != 0)
        _function->SetDefaultAnchors();
}

StdValueGeneral::~StdValueGeneral()
{
    delete _function;
}

void StdValueGeneral::SolveParams()
{
    _function->SolveParams();
}

void StdValueGeneral::SetAnchor(double x, double y)
{
    _function->SetAnchor(x, y);
}

/* ******************************************************* */

StandardizationValueConstraint::StandardizationValueConstraint()
: Standardization()
{
}

StandardizationValueConstraint::StandardizationValueConstraint(Node *node, double min, double max)
: Standardization(node)
{
}

int StandardizationValueConstraint::type() const
{
    return Standardization::StandardizationType::ValueConstraint;
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

int StandardizationClass::type() const
{
    return _constraint ? Standardization::StandardizationType::ClassConstraint : Standardization::StandardizationType::Class;
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

int StandardizationBool::type() const
{
    return Standardization::StandardizationType::Bool;
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

int StandardizationBoolConstraint::type() const
{
    return Standardization::StandardizationType::BoolConstraint;
}
