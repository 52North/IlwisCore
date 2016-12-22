#ifndef SMCEMODEL_H
#define SMCEMODEL_H

#include <QObject>
#include "node.h"
#include "workflow/analysismodel.h"

class SMCEModel : public AnalysisModel
{
    Q_OBJECT


    Q_PROPERTY(Node node READ getNode WRITE setNode)
    Q_PROPERTY(Node tree READ tree)

public:
    SMCEModel();
    SMCEModel(Ilwis::AnalysisPattern *p);
    static AnalysisModel *create(Ilwis::AnalysisPattern *pattern);
    Q_INVOKABLE bool execute(const QVariantMap& inputParameters, QVariantMap& outputParameters) ;
    Q_INVOKABLE Node * getNode(const quint16 id) ;

    Node getNode() const;

    void setNode(Node node);

    Node tree() const;


private:

    NEW_ANALYSISMODEL

    Node _tree;

    //Node *getNode(quint16 id);
    Node *getNode (Node *node, quint16 id);

};

#endif // SMCEMODEL_H
