#ifndef NODE_H
#define NODE_H

#include <QObject>


class Node
{

public:

    enum NodeType { Goal=0, AnalysisArea=1, Objective=2, Constraint=3, Factor=4};

    // for now, try these here....
    quint16 _id;
    quint16 _parentId;
    NodeType _type;
    QString _text;
    float _weight;
    QString _input;
    //QString _standardizedInput; // ???
    QList <Node> _subNodes;

    Node();
};




#endif // NODE_H
