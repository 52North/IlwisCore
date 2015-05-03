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

public:
    GraphModel();
    GraphModel(const QString& yAxis, const std::vector<QVariant>& yvalues, QObject *parent);

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

    QString yAxis() const;

signals:
    void fillColorChanged();
    void strokeColorChanged();
    void pointColorChanged();
    void pointStrokeColorChanged();
    void yAxisChanged();


private:
    QColor _fillcolor;
    QColor _strokeColor;
    QColor _pointColor;
    QColor _pointStrokeColor;
    QList<QVariant> _yvalues;
    QString _yAxis;

};

#endif // GRAPHMODEL_H
