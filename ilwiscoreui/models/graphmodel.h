#ifndef GRAPHMODEL_H
#define GRAPHMODEL_H

#include <QObject>
#include <QColor>
#include <QVariant>
#include "ilwiscoreui_global.h"

class ILWISCOREUISHARED_EXPORT GraphModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor fillColor READ fillColor WRITE fillColor NOTIFY fillColorChanged)
    Q_PROPERTY(QColor strokeColor READ strokeColor WRITE strokeColor NOTIFY strokeColorChanged)
    Q_PROPERTY(QColor pointColor READ pointColor WRITE pointColor NOTIFY pointColorChanged)
    Q_PROPERTY(QColor pointStrokeColor READ pointStrokeColor WRITE pointStrokeColor NOTIFY pointStrokeColorChanged)
    Q_PROPERTY(QString yAxis READ yAxis NOTIFY yAxisChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE enabled NOTIFY enabledChanged)

public:
    GraphModel();
    GraphModel(const QString& yAxis, QObject *parent);

    QColor fillColor() const;
    void fillColor(const QColor &fillcolor);

    QColor strokeColor() const;
    void strokeColor(const QColor &strokeColor);

    QColor pointColor() const;
    void pointColor(const QColor &pointColor);

    QColor pointStrokeColor() const;
    void pointStrokeColor(const QColor &pointStrokeColor);

    QList<QVariant> yvalues() const;
    void yvalues(const QList<QVariant> &yvalues);
    void yvalues(const std::vector<QVariant> &yvalues);
    QList<QVariant> yfraction() const;
    QList<QVariant> ycolors() const;
    Q_INVOKABLE QColor ycolor(int index) const;

    bool enabled() const;
    void enabled(bool yesno);

    Q_INVOKABLE void replaceUndefs(const QString& currentUndef, const QString& newUndef);

    QString yAxis() const;

signals:
    void fillColorChanged();
    void strokeColorChanged();
    void pointColorChanged();
    void pointStrokeColorChanged();
    void yAxisChanged();
    void enabledChanged();


private:
    QColor _fillcolor;
    QColor _strokeColor;
    QColor _pointColor;
    QColor _pointStrokeColor;
    QList<QVariant> _yvalues;
    QList<QVariant> _yfraction; // for piecharts,
    QList<QVariant> _ycolors; // where needed e.g. bar or pie
    QString _yAxis;
    bool _enabled = false;

};

#endif // GRAPHMODEL_H
