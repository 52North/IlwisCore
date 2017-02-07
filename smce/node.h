#ifndef NODE_H
#define NODE_H

#include <QObject>
#include <QQmlListProperty>

class Node : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString name READ name NOTIFY nameChanged )
    Q_PROPERTY( int type READ type NOTIFY typeChanged )
    Q_PROPERTY( double weight READ weight NOTIFY weightChanged )
    Q_PROPERTY( QQmlListProperty<Node> subNodes READ subNodesQml NOTIFY subNodesChanged)
    Q_PROPERTY( QString fileName READ fileName NOTIFY fileNameChanged )
    Q_PROPERTY( int level READ level NOTIFY levelChanged )

signals:
   void nameChanged();
   void typeChanged();
   void weightChanged();
   void subNodesChanged();
   void fileNameChanged();
   void levelChanged();

public:
    enum NodeType { Group=0, MaskArea=1, Constraint=2, Factor=3 };
    Node(QObject *qparent=0);
    Node(Node * parent, QObject *qparent=0);
    int type() const;
    void setType(NodeType nt);
    const QString name() const;
    void setName(QString name);
    double weight() const;
    void setWeight(double weight);
    const Node * parent() const;
    QList <Node*> subNodes();
    QQmlListProperty<Node> subNodesQml();
    void addNode(Node *node);
    const QString fileName() const;
    void setFileName(QString fileName);
    int level() const;
    Q_INVOKABLE void setGoal(QString name, QString fileName);
    Q_INVOKABLE void addMask(QString name, QString fileName);
    Q_INVOKABLE Node * addGroup(QString name, double weight, QString fileName);
    Q_INVOKABLE void addFactor(QString name, double weight, QString fileName);
    Q_INVOKABLE void addConstraint(QString name, QString fileName);

protected:
    NodeType _type;
    QString _name;
    Node * _parent;
    double _weight;
    QList <Node*> _subNodes;
    QString _fileName;
};




#endif // NODE_H
