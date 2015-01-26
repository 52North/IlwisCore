#ifndef LINESTYLESETTER_H
#define LINESTYLESETTER_H

#include <QObject>
#include <QQmlListProperty>
#include <QColor>
#include "ilwiscoreui_global.h"
#include "kernel.h"
#include "ilwisdata.h"
#include "representation.h"
#include "resource.h"
#include "propertyeditor.h"

class ILWISCOREUISHARED_EXPORT LineStyleSetter : public PropertyEditor
{
    Q_OBJECT
public:
    LineStyleSetter(QObject * parent=0);

    int defaultHeight() const;
    static PropertyEditor *create();

     NEW_PROPERTYEDITOR(LineStyleSetter)
};

#endif // LINESTYLESETTER_H
