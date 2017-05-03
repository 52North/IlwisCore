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
    Q_PROPERTY( bool stdWeighDone READ stdWeighDone NOTIFY doneChanged )

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
    void store(QDataStream &stream);
    static Node * load(QDataStream &stream, QObject *qparent);
    int type() const;
    void setType(NodeType nt);
    const QString name() const;
    void setName(QString name);
    const QString unit() const;
    void setUnit(QString unit);
    double weight() const;
    void setWeight(double weight);
    Weights * weights();
    void setWeights(Weights * weights);
    Q_INVOKABLE void resetWeightEdits();
    Standardization * standardization();
    void setStandardization(Standardization * standardization);
    Q_INVOKABLE void resetStandardizationEdits();
    const Node * parent() const;
    QList <Node*> subNodes() const;
    QList <Node*> subFactors() const;
    QList <Node*> getConstraints() const;
    QQmlListProperty<Node> subNodesQml();
    void addNode(Node *node);
    const QString fileName() const;
    void setFileName(QString fileName);
    int level() const;
    void deleteChild(Node * node);
    void recalcWeights();
    QString getPython(QString outputName, bool first=false) const;
    QString getMapcalc() const;
    bool nodeDone();
    bool col1Done();
    void recursivelyEmitDoneChanged() const;
    bool treeEditDone();
    bool stdWeighDone();
    Q_INVOKABLE void setGoal(QString name);
    Q_INVOKABLE void addMask(QString name);
    Q_INVOKABLE Node * addGroup(QString name);
    Q_INVOKABLE void addFactor(QString name);
    Q_INVOKABLE void addConstraint(QString name);
    Q_INVOKABLE void deleteNode();
    Q_INVOKABLE bool done(int mode, int col, bool recursive) const;
    Q_INVOKABLE void setSmceMode(int mode);
    Q_INVOKABLE int smceMode();
    Q_INVOKABLE QString uniqueName(QString name);
    Q_INVOKABLE bool nameAllowed(QString name);

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
    Weights * _weightsEdit;
    Standardization * _standardization;
    Standardization * _standardizationEdit;
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
    static Weights * create(Node *node, WeightMethod method);
    virtual int method();
    virtual DirectWeights * pDirectWeights();
    virtual void Recalculate();
    Q_INVOKABLE virtual void apply();
    virtual Weights * clone() const;
    static void store(QDataStream &stream, Weights * weights);
    static void load(QDataStream &stream, Node *node);

protected:
    virtual void store(QDataStream &stream);
    virtual void load(QDataStream &stream);
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
    virtual Weights * clone() const;

protected:
    virtual void store(QDataStream &stream);
    virtual void load(QDataStream &stream);

private:
    void Refresh(); // to refresh the internal map in case of resizing
    DirectWeightItem * getItem(Node * node);
    QList <DirectWeightItem*> _directWeights; // these are the internal (not normalized) values
    WeightType _weightType;
};

class StandardizationValue;
class StandardizationValueConstraint;
class StandardizationClass;
class StandardizationBool;
class StandardizationBoolConstraint;

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
    void setAnchor(double x, double y);
    virtual QString getPython(QString rasterCoverage, QString outputName) = 0;
    virtual QString getMapcalc(QString rasterCoverage) = 0;
    virtual void setDefaultAnchors() = 0;
    virtual void solveParams() = 0;
    virtual double getFx(double x) const = 0;
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

class MaximumFunction : public SmceFunction
{
public:
    MaximumFunction(Node *node, QList<Anchor *> & anchors, double minX, double maxX, double minY, double maxY, bool benefit);
    virtual QString getPython(QString rasterCoverage, QString outputName);
    virtual QString getMapcalc(QString rasterCoverage);
    virtual void solveParams();
    virtual void setDefaultAnchors();
    virtual double getFx(double x) const;
};

class IntervalFunction : public SmceFunction
{
public:
    IntervalFunction(Node *node, QList<Anchor *> & anchors, double minX, double maxX, double minY, double maxY, bool benefit);
    virtual QString getPython(QString rasterCoverage, QString outputName);
    virtual QString getMapcalc(QString rasterCoverage);
    virtual void solveParams();
    virtual void setDefaultAnchors();
    virtual double getFx(double x) const;
};

class GoalFunction : public SmceFunction
{
public:
    GoalFunction(Node *node, QList<Anchor *> & anchors, double minX, double maxX, double minY, double maxY, bool benefit);
    virtual QString getPython(QString rasterCoverage, QString outputName);
    virtual QString getMapcalc(QString rasterCoverage);
    virtual void solveParams();
    virtual void setDefaultAnchors();
    virtual double getFx(double x) const;
private:
    double a;
    double b; // y = ax + b
};

class ConvexFunction : public SmceFunction
{
public:
    ConvexFunction(Node *node, QList<Anchor *> & anchors, double minX, double maxX, double minY, double maxY, bool benefit);
    virtual QString getPython(QString rasterCoverage, QString outputName);
    virtual QString getMapcalc(QString rasterCoverage);
    virtual void solveParams();
    virtual void setDefaultAnchors();
    virtual double getFx(double x) const;
private:
    double a;
    double b;
    double c; // y = a + be^cx
};

class ConcaveFunction : public SmceFunction
{
public:
    ConcaveFunction(Node *node, QList<Anchor *> & anchors, double minX, double maxX, double minY, double maxY, bool benefit);
    virtual QString getPython(QString rasterCoverage, QString outputName);
    virtual QString getMapcalc(QString rasterCoverage);
    virtual void solveParams();
    virtual void setDefaultAnchors();
    virtual double getFx(double x) const;
private:
    double a;
    double b;
    double c; // y = 1 - (a + be^cx)
};

class QuadraticFunction : public SmceFunction
{
public:
    QuadraticFunction(Node *node, QList<Anchor *> & anchors, double minX, double maxX, double minY, double maxY, bool benefit);
    virtual QString getPython(QString rasterCoverage, QString outputName);
    virtual QString getMapcalc(QString rasterCoverage);
    virtual void solveParams();
    virtual void setDefaultAnchors();
    virtual double getFx(double x) const;
private:
    double a;
    double b;
    double c; // y = ax^2 + bx + c
};

class GaussianFunction : public SmceFunction
{
public:
    GaussianFunction(Node *node, QList<Anchor *> & anchors, double minX, double maxX, double minY, double maxY, bool benefit);
    virtual QString getPython(QString rasterCoverage, QString outputName);
    virtual QString getMapcalc(QString rasterCoverage);
    virtual void solveParams();
    virtual void setDefaultAnchors();
    virtual double getFx(double x) const;
private:
    double a;
    double b;
    double c; // y = a * e^(b(x-c))
};

class PiecewiseLinear8Function : public SmceFunction
{
public:
    PiecewiseLinear8Function(Node *node, QList<Anchor *> & anchors, double minX, double maxX, double minY, double maxY, bool benefit);
    virtual QString getPython(QString rasterCoverage, QString outputName);
    virtual QString getMapcalc(QString rasterCoverage);
    virtual void solveParams();
    virtual void setDefaultAnchors();
    virtual double getFx(double x) const;
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
    Q_PROPERTY( StandardizationValueConstraint * standardizationValueConstraint READ pStandardizationValueConstraint NOTIFY standardizationValueConstraintChanged )
    Q_PROPERTY( StandardizationClass * standardizationClass READ pStandardizationClass NOTIFY standardizationClassChanged )
    Q_PROPERTY( StandardizationBool * standardizationBool READ pStandardizationBool NOTIFY standardizationBoolChanged )
    Q_PROPERTY( StandardizationBoolConstraint * standardizationBoolConstraint READ pStandardizationBoolConstraint NOTIFY standardizationBoolConstraintChanged )

signals:
   void typeChanged();
   void standardizationValueChanged();
   void standardizationValueConstraintChanged();
   void standardizationClassChanged();
   void standardizationBoolChanged();
   void standardizationBoolConstraintChanged();

public:
    enum StandardizationType{ None=0, Value=1, ValueConstraint=2, Class=3, ClassConstraint=4, Bool=5, BoolConstraint=6 };
    Standardization();
    Standardization(Node *node);
    virtual QString getPython(QString rasterCoverage, QString outputName) const;
    virtual QString getMapcalc(QString rasterCoverage) const;
    virtual int type() const;
    virtual StandardizationValue * pStandardizationValue();
    virtual StandardizationValueConstraint * pStandardizationValueConstraint();
    virtual StandardizationClass * pStandardizationClass();
    virtual StandardizationBool * pStandardizationBool();
    virtual StandardizationBoolConstraint * pStandardizationBoolConstraint();
    static Standardization * create(Node *node);
    Q_INVOKABLE virtual void apply();
    virtual Standardization * clone() const;
    static void store(QDataStream &stream, Standardization * standardization);
    static void load(QDataStream &stream, Node *node);

protected:
    virtual void store(QDataStream &stream);
    virtual void load(QDataStream &stream);

protected:
    Node * _node;
};

class StandardizationValue : public Standardization
{
    Q_OBJECT
    Q_ENUMS(StandardizationValueMethodType)
    Q_PROPERTY( double min READ min NOTIFY minChanged )
    Q_PROPERTY( double max READ max NOTIFY maxChanged )
    Q_PROPERTY( int method READ method NOTIFY methodChanged )
    Q_PROPERTY( int benefit READ benefit NOTIFY benefitChanged )
    Q_PROPERTY( QQmlListProperty<Anchor> anchors READ anchors NOTIFY anchorsChanged )

signals:
    void minChanged();
    void maxChanged();
    void methodChanged();
    void benefitChanged();
    void anchorsChanged();

public:
    enum StandardizationValueMethodType{ None=0, Maximum=1, Interval=2, Goal=3, Convex=4, Concave=5, Ushape=6, Gaussian=7, PiecewiseLinear8=8 };
    StandardizationValue();
    StandardizationValue(Node *node, double min, double max);
    double min() const;
    double max() const;
    int method() const;
    bool benefit() const;
    Q_INVOKABLE void setMethod(StandardizationValueMethodType method, bool benefit);
    virtual QString getPython(QString rasterCoverage, QString outputName) const;
    virtual QString getMapcalc(QString rasterCoverage) const;
    QQmlListProperty<Anchor> anchors();
    Q_INVOKABLE void solveParams();
    Q_INVOKABLE void setAnchor(double x, double y);
    Q_INVOKABLE double getFx(double x) const;
    virtual int type() const;
    virtual StandardizationValue * pStandardizationValue();
    virtual ~StandardizationValue();
    virtual Standardization * clone() const;

protected:
    virtual void store(QDataStream &stream);
    virtual void load(QDataStream &stream);

private:
    StdValueMethod * _stdValueMethod;
    const double _min;
    const double _max;
    StandardizationValueMethodType _method;
    bool _benefit;
    QList <Anchor*> _anchors;
};

class StdValueMethod
{
public:
    StdValueMethod();
    StdValueMethod(Node *node, double min, double max);
    virtual ~StdValueMethod();
    virtual void solveParams() = 0;
    virtual void setAnchor(double x, double y) = 0;
    virtual double getFx(double x) const = 0;
    virtual QString getPython(QString rasterCoverage, QString outputName) const = 0;
    virtual QString getMapcalc(QString rasterCoverage) const = 0;
    static StdValueMethod * create(Node *node, QList<Anchor *> &anchors, double min, double max, StandardizationValue::StandardizationValueMethodType method, bool benefit);

protected:
    Node * _node;
    const double _min; // the min/max of the rastercoverage
    const double _max;
};

class StdValueGeneral : public StdValueMethod
{
public:
    StdValueGeneral(Node *node, QList<Anchor *> &anchors, double min, double max, StandardizationValue::StandardizationValueMethodType method, bool benefit);
    virtual void solveParams();
    virtual void setAnchor(double x, double y);
    virtual double getFx(double x) const;
    virtual QString getPython(QString rasterCoverage, QString outputName) const;
    virtual QString getMapcalc(QString rasterCoverage) const;
    virtual ~StdValueGeneral();

private:
    StandardizationValue::StandardizationValueMethodType _method;
    SmceFunction * _function;
};

class StandardizationValueConstraint : public Standardization
{
    Q_OBJECT
    Q_PROPERTY( double min READ min NOTIFY minChanged )
    Q_PROPERTY( double max READ max NOTIFY maxChanged )
    Q_PROPERTY( double minVal READ minVal WRITE setMinVal NOTIFY minValChanged )
    Q_PROPERTY( double maxVal READ maxVal WRITE setMaxVal NOTIFY maxValChanged )
    Q_PROPERTY( bool useMin READ useMin WRITE setUseMin NOTIFY useMinChanged )
    Q_PROPERTY( bool useMax READ useMax WRITE setUseMax NOTIFY useMaxChanged )

signals:
    void minChanged();
    void maxChanged();
    void minValChanged();
    void maxValChanged();
    void useMinChanged();
    void useMaxChanged();

public:
    StandardizationValueConstraint();
    StandardizationValueConstraint(Node *node, double min, double max);
    double min() const;
    double max() const;
    double minVal() const;
    double maxVal() const;
    void setMinVal(double minVal);
    void setMaxVal(double maxVal);
    bool useMin() const;
    bool useMax() const;
    void setUseMin(bool useMin);
    void setUseMax(bool useMax);
    virtual QString getPython(QString rasterCoverage, QString outputName) const;
    virtual QString getMapcalc(QString rasterCoverage) const;
    virtual int type() const;
    virtual StandardizationValueConstraint * pStandardizationValueConstraint();
    virtual Standardization * clone() const;

protected:
    virtual void store(QDataStream &stream);
    virtual void load(QDataStream &stream);

protected:
    const double _min; // the min/max of the rastercoverage
    const double _max;
    double _minVal; // the min/max thresholds that the user has chosen
    double _maxVal;
    bool _useMin; // whether _minVal and/or _maxVal must be used
    bool _useMax;
};

class StandardizationClass : public Standardization
{
    Q_OBJECT

public:
    StandardizationClass();
    StandardizationClass(Node *node, bool constraint);
    virtual QString getPython(QString rasterCoverage, QString outputName) const;
    virtual QString getMapcalc(QString rasterCoverage) const;
    virtual int type() const;
    virtual StandardizationClass * pStandardizationClass();
    virtual Standardization * clone() const;

protected:
    virtual void store(QDataStream &stream);
    virtual void load(QDataStream &stream);

private:
    const bool _constraint;
};

class StandardizationBool : public Standardization
{
    Q_OBJECT

public:
    StandardizationBool();
    StandardizationBool(Node *node);
    virtual QString getPython(QString rasterCoverage, QString outputName) const;
    virtual QString getMapcalc(QString rasterCoverage) const;
    virtual int type() const;
    virtual StandardizationBool * pStandardizationBool();
    virtual Standardization * clone() const;

protected:
    virtual void store(QDataStream &stream);
    virtual void load(QDataStream &stream);
};

class StandardizationBoolConstraint : public Standardization
{
    Q_OBJECT

public:
    StandardizationBoolConstraint();
    StandardizationBoolConstraint(Node *node);
    virtual QString getPython(QString rasterCoverage, QString outputName) const;
    virtual QString getMapcalc(QString rasterCoverage) const;
    virtual int type() const;
    virtual StandardizationBoolConstraint * pStandardizationBoolConstraint();
    virtual Standardization * clone() const;

protected:
    virtual void store(QDataStream &stream);
    virtual void load(QDataStream &stream);
};

#endif // NODE_H
