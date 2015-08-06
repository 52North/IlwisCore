#ifndef GRIDPROPERTYEDITOR_H
#define GRIDPROPERTYEDITOR_H

#include <QObject>
#include <QQmlListProperty>
#include "propertyeditors/attributeeditor.h"

class SubGridPropertyEditor : public VisualAttributeEditor
{
    Q_OBJECT


    Q_PROPERTY(double opacity READ opacity WRITE opacity NOTIFY opacityChanged)
    Q_PROPERTY(QColor lineColor READ lineColor WRITE lineColor NOTIFY lineColorChanged)

public:
    SubGridPropertyEditor(const QString &editname, const QString &label, const QUrl &qml, QObject *parent = 0);

    bool canUse(const IIlwisObject &obj, const QString &name) const;
    //void prepare(const QString &editorName, VisualAttributeModel *vattrib, const Ilwis::IIlwisObject& bj, const Ilwis::ColumnDefinition &datadef = Ilwis::ColumnDefinition());

    double opacity() const;
    void opacity( double v);
    QColor lineColor() const;
    void lineColor(const QColor& clr);
    void isActive(bool yesno);

signals:
    void distanceChanged();
    void opacityChanged();
    void lineColorChanged();


protected:
    virtual QString gridDrawerName() const = 0;
    Ilwis::Geodrawer::UPDrawer& subgriddrawer(const QString& gridname);
    const Ilwis::Geodrawer::UPDrawer& subgriddrawer(const QString& gridname) const;

};

class PrimaryGridEditor : public SubGridPropertyEditor{
    Q_OBJECT
public:
    PrimaryGridEditor(QObject *parent = 0);

    NEW_PROPERTYEDITOR(PrimaryGridEditor)

    Q_PROPERTY(double distance READ distance WRITE distance NOTIFY distanceChanged)
    Q_PROPERTY(bool isActive READ isActive WRITE isActive NOTIFY activeChanged)

    double distance() const;
    void distance(double v);
    bool isActive() const;
    void isActive(bool yesno);

    QString gridDrawerName() const;

    static VisualAttributeEditor *create();
    void prepare(VisualAttributeModel *vattrib, const Ilwis::IIlwisObject &bj, const Ilwis::ColumnDefinition &datadef);
signals:
    void distanceChanged();
    void activeChanged();

};

class SecondaryGridEditor : public SubGridPropertyEditor{
    Q_OBJECT
public:
    SecondaryGridEditor(QObject *parent = 0);

    NEW_PROPERTYEDITOR(SecondaryGridEditor)

    Q_PROPERTY(int distance READ noOfCells WRITE noOfCells NOTIFY noOfCellsChanged)
    Q_PROPERTY(bool isActive READ isActive WRITE isActive NOTIFY activeChanged)

    int noOfCells() const;
    void noOfCells(int v);
    bool isActive() const;
    void isActive(bool yesno);

    QString gridDrawerName() const;

    static VisualAttributeEditor *create();
    void prepare(VisualAttributeModel *vattrib, const Ilwis::IIlwisObject &bj, const Ilwis::ColumnDefinition &datadef);

signals:
    void noOfCellsChanged();
    void activeChanged();

};

#endif // GRIDPROPERTYEDITOR_H
