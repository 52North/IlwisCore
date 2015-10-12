#ifndef LINEPROPERTYSETTER_H
#define LINEPROPERTYSETTER_H


#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"

class LinePropertySetter : public VisualAttributeEditor
{
 Q_OBJECT

    Q_PROPERTY(bool showBoundaries READ showBoundaries WRITE setShowBoundaries NOTIFY showBoundariesChanged)
    Q_PROPERTY(QColor boundaryColor READ boundaryColor WRITE setBoundaryColor NOTIFY boundaryColorChanged)
    Q_PROPERTY(int boundarywidth READ boundarywidth WRITE setBoundarywidth NOTIFY boundarywidthChanged)
public:
    ~LinePropertySetter();
    LinePropertySetter(QObject *parent=0);
    bool canUse(const IIlwisObject &obj, const QString& name) const;

    bool showBoundaries() const;
    void setShowBoundaries(bool yesno);
    QColor boundaryColor() const;
    void setBoundaryColor(const QColor& clr);
    float boundarywidth() const;
    void setBoundarywidth(float w);

    static VisualAttributeEditor *create();

 NEW_PROPERTYEDITOR(LinePropertySetter)

 signals:
     void showBoundariesChanged();
     void boundaryColorChanged();
     void boundarywidthChanged();
};

#endif // LINEPROPERTYSETTER_H
