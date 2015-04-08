#ifndef POLYGONPROPERTYSETTER_H
#define POLYGONPROPERTYSETTER_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"

class PolygonPropertySetter : public VisualAttributeEditor
{
    Q_OBJECT

    Q_PROPERTY(bool showBoundaries READ showBoundaries WRITE setShowBoundaries NOTIFY showBoundariesChanged)
    Q_PROPERTY(bool showAreas READ showAreas WRITE setShowAreas NOTIFY showAreasChanged)
    Q_PROPERTY(QColor boundaryColor READ boundaryColor WRITE setBoundaryColor NOTIFY boundaryColorChanged)
    Q_PROPERTY(int boundarywidth READ boundarywidth WRITE setBoundarywidth NOTIFY boundarywidthChanged)
public:
    ~PolygonPropertySetter();
    PolygonPropertySetter(QObject *parent=0);
    static VisualAttributeEditor *create();

    NEW_PROPERTYEDITOR(PolygonPropertySetter)
    bool canUse(const IIlwisObject &obj, const QString &name) const;
    bool showBoundaries() const;
    void setShowBoundaries(bool yesno);
    bool showAreas() const;
    void setShowAreas(bool yesno);
    QColor boundaryColor() const;
    void setBoundaryColor(const QColor& clr);
    float boundarywidth() const;
    void setBoundarywidth(float w);


signals:
    void showBoundariesChanged();
    void showAreasChanged();
    void boundaryColorChanged();
    void boundarywidthChanged();

private:

};

#endif // POLYGONPROPERTYSETTER_H
