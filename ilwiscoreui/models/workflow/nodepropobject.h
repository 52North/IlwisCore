#ifndef NODEPROPOBJECT_H
#define NODEPROPOBJECT_H

#include <QObject>
#include "ilwiscoreui_global.h"

namespace Ilwis {
struct NodeProperties;
}


class ILWISCOREUISHARED_EXPORT NodePropObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint16 x MEMBER _x)
    Q_PROPERTY(quint16 y MEMBER _y)
    Q_PROPERTY(quint32 vertex MEMBER _vertex)
    Q_PROPERTY(quint64 operationId MEMBER _operationId)
public:
    NodePropObject(QObject *parent = 0);
    void setProps(const Ilwis::NodeProperties& props, int v);

private:
    quint16 _x = 0;
    quint16 _y = 0;
    quint32 _vertex = 0;
    quint64 _operationId = 0;
};

#endif // NODEPROPOBJECT_H
