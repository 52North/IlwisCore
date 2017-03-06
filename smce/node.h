#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QQmlListProperty>

class Weights;
class Standardization;

class Node : public QObject
{
    Q_OBJECT
    Q_ENUMS(NodeType)
    Q_ENUMS(Mode)
    Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged )
    Q_PROPERTY( QString unit READ unit WRITE setUnit NOTIFY unitChanged )
    Q_PROPERTY( int type READ type NOTIFY typeChanged )
    Q_PROPERTY( double weight READ weight NOTIFY weightChanged )
    Q_PROPERTY( QQmlListProperty<Node> subNodes READ subNodesQml NOTIFY subNodesChanged )
    Q_PROPERTY( QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged )
    Q_PROPERTY( int level READ level NOTIFY levelChanged )
    Q_PROPERTY( Weights * weights READ weights NOTIFY weightsChanged )
    Q_PROPERTY( Standardization * standardization READ standardization NOTIFY standardizationChanged )
    Q_PROPERTY( bool nodeDone READ nodeDone NOTIFY doneChanged )
    Q_PROPERTY( bool col1Done READ col1Done NOTIFY doneChanged )
    Q_PROPERTY( bool treeEditDone READ treeEditDone NOTIFY doneChanged )

signals:
   void nameChanged() const;
   void unitChanged() const;
   void typeChanged() const;
   void weightChanged() const;
   void subNodesChanged() const;
   void fileNameChanged() const;
   void levelChanged() const;
   void nodeDeleted() const;
   void weightsChanged() const;
   void standardizationChanged() const;
   void doneChanged() const;

public:
    enum NodeType { Group=0, MaskArea=1, Constraint=2, Factor=3 };
    enum Mode { EditTree=0, StdWeigh=1};
    Node();
    Node(QObject *qparent);
    Node(Node * parent, QObject *qparent);
    int type() const;
    void setType(NodeType nt);
    const QString name() const;
    void setName(QString name);
    const QString unit() const;
    void setUnit(QString unit);
    double weight() const;
    void setWeight(double weight);
    Weights * weights();
    Standardization * standardization();
    const Node * parent() const;
    QList <Node*> subNodes();
    QList <Node*> subFactors() const;
    QQmlListProperty<Node> subNodesQml();
    void addNode(Node *node);
    const QString fileName() const;
    void setFileName(QString fileName);
    int level() const;
    void deleteChild(Node * node);
    void recalcWeights();
    QString getPython() const;
    QString getMapcalc() const;
    bool nodeDone();
    bool col1Done();
    void recursivelyEmitDoneChanged() const;
    bool treeEditDone();
    Q_INVOKABLE void setGoal(QString name);
    Q_INVOKABLE void addMask(QString name);
    Q_INVOKABLE Node * addGroup(QString name);
    Q_INVOKABLE void addFactor(QString name);
    Q_INVOKABLE void addConstraint(QString name);
    Q_INVOKABLE void deleteNode();
    Q_INVOKABLE bool done(int mode, int col, bool recursive) const;
    Q_INVOKABLE void setSmceMode(int mode);

protected:
    void emitDoneChanged() const;
    Node * root();
    NodeType _type;
    QString _name;
    QString _unit;
    Node * _parent;
    double _weight;
    QList <Node*> _subNodes;
    QString _fileName;
    Weights * _weights;
    Standardization * _standardization;
    Mode _smceMode;
};

class DirectWeights;

class Weights : public QObject
{
    Q_OBJECT
    Q_ENUMS(WeightMethod)
    Q_PROPERTY( int method READ method )
    Q_PROPERTY( DirectWeights * directWeights READ pDirectWeights )

signals:
   void weightsChanged();

public:
    enum WeightMethod{ None=0, Direct=1, Pairwise=2, Rankorder=3 };
    Weights();
    Weights(Node *node);
    static Weights * create(Node *node);
    virtual int method();
    virtual DirectWeights * pDirectWeights();
    virtual void Recalculate();
    Q_INVOKABLE virtual void apply();

protected:
    Node * _node;
};

class DirectWeightItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString name READ name NOTIFY nameChanged )
    Q_PROPERTY( double directWeight READ directWeight WRITE setDirectWeight NOTIFY directWeightChanged )
    Q_PROPERTY( double normalizedWeight READ normalizedWeight NOTIFY normalizedWeightChanged )

signals:
   void nameChanged();
   void directWeightChanged();
   void normalizedWeightChanged();

public:
    DirectWeightItem();
    DirectWeightItem(Node * node, DirectWeights * weights);
    QString name();
    double directWeight();
    void setDirectWeight(double directWeight);
    double normalizedWeight();
    void setNormalizedWeight(double normalizedWeight);

private:
    Node * _node;
    DirectWeights * _weights;
    double _directWeight;
    double _normalizedWeight;
};

class DirectWeights : public Weights
{
    Q_OBJECT
    Q_PROPERTY( QQmlListProperty<DirectWeightItem> directWeights READ directWeights NOTIFY weightsChanged )

signals:
   void weightsChanged();

public:
    enum WeightType{SumToOne, MaxIsOne};
    DirectWeights();
    DirectWeights(Node *node);
    virtual int method();
    virtual DirectWeights * pDirectWeights();
    QQmlListProperty<DirectWeightItem> directWeights();
    virtual void Recalculate();
    Q_INVOKABLE virtual void apply();

private:
    void Refresh(); // to refresh the internal map in case of resizing
    DirectWeightItem * getItem(Node * node);
    QList <DirectWeightItem*> _directWeights; // these are the internal (not normalized) values
    WeightType _weightType;
};

class StandardizationValue;

class Anchor : public QObject
{
    Q_OBJECT
    Q_PROPERTY( double x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY( double y READ y WRITE setY NOTIFY yChanged)
signals:
    void xChanged();
    void yChanged();
public:
    Anchor();
    Anchor(Node * node, double x, double y);
    double x() const;
    double y() const;
    void setX(double x);
    void setY(double y);
private:
    double _x;
    double _y;
};

class SmceFunction
{
public:
    SmceFunction(Node *node, QList<Anchor*> & anchors, int nrAnchors, double minX, double maxX, double minY, double maxY, bool benefit);
    virtual ~SmceFunction();
    void SetAnchor(double x, double y);
    virtual QString getPython(QString rasterCoverage) = 0;
    virtual QString getMapcalc(QString rasterCoverage) = 0;
    virtual void SetDefaultAnchors() = 0;
    virtual void SolveParams() = 0;
protected:
    QList<Anchor*> & _anchors;
    const double _minX;
    const double _maxX;
    const double _minY;
    const double _maxY;
    const bool _benefit;
private:
    void SeparateAnchorPoints();
    double rUnitDist2(double x, double y, Anchor * anchor);
};

class PiecewiseLinear8Function : public SmceFunction
{
public:
    PiecewiseLinear8Function(Node *node, QList<Anchor *> & anchors, double minX, double maxX, double minY, double maxY, bool benefit);
    virtual QString getPython(QString rasterCoverage);
    virtual QString getMapcalc(QString rasterCoverage);
    virtual void SolveParams();
    virtual void SetDefaultAnchors();
private:
    QString getLine(double a, QString x, double b);
    double a1;
    double b1; // y = a1x + b1 in first interval
    double a2;
    double b2; // y = a2x + b2 in second interval
    double a3;
    double b3; // y = a3x + b3 in third interval
    double a4;
    double b4; // y = a4x + b4 in fourth interval
    double a5;
    double b5; // y = a5x + b5 in fifth interval
    double a6;
    double b6; // y = a6x + b6 in sixth interval
    double a7;
    double b7; // y = a7x + b7 in seventh interval
    double a8;
    double b8; // y = a8x + b8 in eighth interval
};

class StdValueMethod;

class Standardization : public QObject
{
    Q_OBJECT
    Q_ENUMS(StandardizationType)
    Q_PROPERTY( int type READ type NOTIFY typeChanged )
    Q_PROPERTY( StandardizationValue * standardizationValue READ pStandardizationValue NOTIFY standardizationValueChanged )

signals:
   void typeChanged();
   void standardizationValueChanged();

public:
    enum StandardizationType{ None=0, Value=1, ValueConstraint=2, Class=3, ClassConstraint=4, Bool=5, BoolConstraint=6 };
    Standardization();
    Standardization(Node *node);
    virtual QString getPython(QString rasterCoverage) const;
    virtual QString getMapcalc(QString rasterCoverage) const;
    virtual int type() const;
    virtual StandardizationValue * pStandardizationValue();
    static Standardization * create(Node *node);

protected:
    Node * _node;
};

class StandardizationValue : public Standardization
{
    Q_OBJECT
    Q_ENUMS(StandardizationValueMethodType)
    Q_PROPERTY( double min READ min NOTIFY minChanged )
    Q_PROPERTY( double max READ max NOTIFY maxChanged )
    Q_PROPERTY( int methodType READ methodType NOTIFY methodTypeChanged )
    Q_PROPERTY( QQmlListProperty<Anchor> anchors READ anchors /*WRITE setAnchors */ NOTIFY anchorsChanged )

signals:
    void minChanged();
    void maxChanged();
    void methodTypeChanged();
    void anchorsChanged();

public:
    enum StandardizationValueMethodType{ Maximum=0, Interval=1, Goal=2, Convex=3, Concave=4, Ushape=5, Gaussian=6, PiecewiseLinear8=7 };
    StandardizationValue();
    StandardizationValue(Node *node, double min, double max);
    double min() const;
    double max() const;
    int methodType() const;
    virtual QString getPython(QString rasterCoverage) const;
    virtual QString getMapcalc(QString rasterCoverage) const;
    QQmlListProperty<Anchor> anchors();
    //void setAnchors(QQmlListProperty<Anchor> anchors);
    Q_INVOKABLE void SolveParams();
    Q_INVOKABLE void SetAnchor(double x, double y);
    virtual int type() const;
    virtual StandardizationValue * pStandardizationValue();
    virtual ~StandardizationValue();

private:
    StdValueMethod * _stdValueMethod;
    const double _min;
    const double _max;
    const StandardizationValueMethodType _methodType;
    QList <Anchor*> _anchors;
};

class StdValueMethod
{
public:
    StdValueMethod();
    StdValueMethod(Node *node, double min, double max);
    virtual ~StdValueMethod();
    virtual void SolveParams() = 0;
    virtual void SetAnchor(double x, double y) = 0;
    virtual QString getPython(QString rasterCoverage) const = 0;
    virtual QString getMapcalc(QString rasterCoverage) const = 0;
    static StdValueMethod * create(Node *node, QList<Anchor *> &anchors, double min, double max, StandardizationValue::StandardizationValueMethodType method);

protected:
    Node * _node;
    const double _min;
    const double _max;
};

class StdValueGeneral : public StdValueMethod
{
public:
    StdValueGeneral(Node *node, QList<Anchor *> &anchors, double min, double max, StandardizationValue::StandardizationValueMethodType method);
    virtual void SolveParams();
    virtual void SetAnchor(double x, double y);
    virtual QString getPython(QString rasterCoverage) const;
    virtual QString getMapcalc(QString rasterCoverage) const;
    virtual ~StdValueGeneral();

private:
    StandardizationValue::StandardizationValueMethodType _method;
    SmceFunction * _function;
};

class StandardizationValueConstraint : public Standardization
{
    Q_OBJECT

public:
    StandardizationValueConstraint();
    StandardizationValueConstraint(Node *node, double min, double max);
    virtual QString getPython(QString rasterCoverage) const;
    virtual QString getMapcalc(QString rasterCoverage) const;
    virtual int type() const;
};

class StandardizationClass : public Standardization
{
    Q_OBJECT

public:
    StandardizationClass();
    StandardizationClass(Node *node, bool constraint);
    virtual QString getPython(QString rasterCoverage) const;
    virtual QString getMapcalc(QString rasterCoverage) const;
    virtual int type() const;

private:
    const bool _constraint;
};

class StandardizationBool : public Standardization
{
    Q_OBJECT

public:
    StandardizationBool();
    StandardizationBool(Node *node);
    virtual QString getPython(QString rasterCoverage) const;
    virtual QString getMapcalc(QString rasterCoverage) const;
    virtual int type() const;
};

class StandardizationBoolConstraint : public Standardization
{
    Q_OBJECT

public:
    StandardizationBoolConstraint();
    StandardizationBoolConstraint(Node *node);
    virtual QString getPython(QString rasterCoverage) const;
    virtual QString getMapcalc(QString rasterCoverage) const;
    virtual int type() const;
};

#endif // NODE_H
