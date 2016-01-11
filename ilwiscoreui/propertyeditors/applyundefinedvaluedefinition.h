#ifndef APPLYUNDEFINEDVALUEDEFINITION_H
#define APPLYUNDEFINEDVALUEDEFINITION_H

#include <QObject>
#include "propertyeditors/attributeeditor.h"

class ApplyUndefinedValueDefinition : public VisualAttributeEditor
{
      Q_OBJECT

    Q_PROPERTY(double undefinedvalue READ undefinedvalue WRITE undefinedvalue NOTIFY undefinedChanged)

public:
    ApplyUndefinedValueDefinition(QObject *parent = 0);

    bool canUse(const IIlwisObject &obj, const QString &name) const;
    static VisualAttributeEditor *create();

    double undefinedvalue() const;
    void undefinedvalue(double tr);


     NEW_PROPERTYEDITOR(ApplyUndefinedValueDefinition)

signals:
         void undefinedChanged();
};

#endif // APPLYUNDEFINEDVALUEDEFINITION_H
