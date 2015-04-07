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
    Q_PROPERTY(bool layerHeader READ layerHeader CONSTANT)
public:
    LayerInfoItem();
    explicit LayerInfoItem(const QString &name, const QString &value, QObject *parent = 0);
    ~LayerInfoItem();

    void icon(const QString& ic);
    bool layerHeader() const;
    void layerHeader(bool yesno);

signals:

public slots:

private:
    QString name() const;
    QString value() const;
    QString icon() const;

    QString _name;
    QString _value;
    QString _icon;
    bool _layerHeader=false;
};

#endif // LAYERINFOITEM_H
