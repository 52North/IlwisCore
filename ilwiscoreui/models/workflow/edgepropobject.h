#ifndef EDGEPROPOBJECT_H
#define EDGEPROPOBJECT_H

#include <QObject>
#include "ilwiscoreui_global.h"

namespace Ilwis {
struct EdgeProperties;
}


class ILWISCOREUISHARED_EXPORT EdgePropObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(quint16 fromParameter MEMBER _outputParameterIndex)
    Q_PROPERTY(quint16 toParameter MEMBER _inputParameterIndex)
    Q_PROPERTY(quint32 fromRect MEMBER _outputRectangleIndex)
    Q_PROPERTY(quint64 toRect MEMBER _inputRectangleIndex)
    Q_PROPERTY(quint64 fromVertex MEMBER _outputVertexIndex)
    Q_PROPERTY(quint64 toVertex MEMBER _intputVertexIndex)
public:
    EdgePropObject(QObject *parent = 0);
    void setProps(const Ilwis::EdgeProperties& props, int v1, int v2);

private:
    int _outputParameterIndex = 0;
    int _inputParameterIndex = 0;
    int _outputRectangleIndex = 0;
    int _inputRectangleIndex = 0;
    int _outputVertexIndex = 0;
    int _intputVertexIndex = 0;
};

#endif // EDGEPROPOBJECT_H

