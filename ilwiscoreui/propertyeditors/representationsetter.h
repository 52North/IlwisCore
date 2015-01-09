#ifndef REPRESENTATIONSETTER_H
#define REPRESENTATIONSETTER_H

#include <QObject>
#include "ilwiscoreui_global.h"
#include "identity.h"
#include "resource.h"
//#include "propertyeditor.h"

class ILWISCOREUISHARED_EXPORT RepresentationSetter : public QObject /*: public PropertyEditor*/
{
    Q_OBJECT

    Q_PROPERTY(QStringList rprNames READ rprNames CONSTANT)

public:
    Q_INVOKABLE RepresentationSetter(QObject *parent = 0);
    Q_INVOKABLE void prepare();

private:
    QStringList rprNames() const;
    std::vector<Ilwis::Resource> _representations;
};

#endif // REPRESENTATIONSETTER_H
