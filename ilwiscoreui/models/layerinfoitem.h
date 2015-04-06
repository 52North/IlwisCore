#ifndef LAYERINFOITEM_H
#define LAYERINFOITEM_H

#include "ilwiscoreui_global.h"
#include <QObject>

class ILWISCOREUISHARED_EXPORT LayerInfoItem : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString value READ value CONSTANT)
    Q_PROPERTY(QString icon READ icon CONSTANT)
public:
    LayerInfoItem();
    explicit LayerInfoItem(const QString &name, const QString &value, QObject *parent = 0);
    ~LayerInfoItem();

    void icon(const QString& ic);

signals:

public slots:

private:
    QString name() const;
    QString value() const;
    QString icon() const;

    QString _name;
    QString _value;
    QString _icon;
};

#endif // LAYERINFOITEM_H
