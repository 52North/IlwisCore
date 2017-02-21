#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QQmlListProperty>

class Weights;

class Node : public QObject
{
    Q_OBJECT
    Q_ENUMS(NodeType)
    Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged )
    Q_PROPERTY( QString unit READ unit WRITE setUnit NOTIFY unitChanged )
    Q_PROPERTY( int type READ type NOTIFY typeChanged )
    Q_PROPERTY( double weight READ weight NOTIFY weightChanged )
    Q_PROPERTY( QQmlListProperty<Node> subNodes READ subNodesQml NOTIFY subNodesChanged )
    Q_PROPERTY( QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged )
    Q_PROPERTY( int level READ level NOTIFY levelChanged )
    Q_PROPERTY( Weights * weights READ weights NOTIFY weightsChanged )

signals:
   void nameChanged();
   void unitChanged();
   void typeChanged();
   void weightChanged();
   void subNodesChanged();
   void fileNameChanged();
   void levelChanged();
   void nodeDeleted();
   void weightsChanged();

public:
    enum NodeType { Group=0, MaskArea=1, Constraint=2, Factor=3 };
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
    const Node * parent() const;
    QList <Node*> subNodes();
    QQmlListProperty<Node> subNodesQml();
    void addNode(Node *node);
    const QString fileName() const;
    void setFileName(QString fileName);
    int level() const;
    void deleteChild(Node * node);
    void recalcWeights();
    Q_INVOKABLE void setGoal(QString name);
    Q_INVOKABLE void addMask(QString name);
    Q_INVOKABLE Node * addGroup(QString name);
    Q_INVOKABLE void addFactor(QString name);
    Q_INVOKABLE void addConstraint(QString name);
    Q_INVOKABLE void deleteNode();

protected:
    NodeType _type;
    QString _name;
    QString _unit;
    Node * _parent;
    double _weight;
    QList <Node*> _subNodes;
    QString _fileName;
    Weights * _weights;
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

#endif // NODE_H
