#ifndef TRANQUILIZERHANDLER_H
#define TRANQUILIZERHANDLER_H

#include <QObject>
#include <QColor>
#include <QString>
#include <QQmlListProperty>
#include "kernel.h"
#include "issuelogger.h"
#include "juliantime.h"
#include "tranquilizer.h"

class TranquilizerModel : public QObject{
    Q_OBJECT

    Q_PROPERTY(quint64 id READ id CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(double endValue READ endValue CONSTANT)
    Q_PROPERTY(double currentValue READ currentValue CONSTANT)
    Q_PROPERTY(double currentValueP READ currentValueP CONSTANT)

public:
    TranquilizerModel();
    TranquilizerModel(quint64 id, const QString& title, const QString& description, double endval, QObject *obj);
    QString title() const;
    QString description() const;
    double endValue() const;
    double currentValue() const;
    void currentValue(double v);
    double currentValueP() const;

    quint64 id() const;
private:
    quint64 _id;
    QString _title;
    QString _description;
    double _beginValue = 0;
    double _endValue;
    double _currentValue;
};

typedef QQmlListProperty<TranquilizerModel> TranquilizerList;

class TranquilizerHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TranquilizerList tranquilizers READ tranquilizers NOTIFY tranquilizersChanged)
public:
    explicit TranquilizerHandler(QObject *parent = 0);


    TranquilizerList tranquilizers();

signals:
    void updateTranquilizerUI(quint64 id, double amount);
    void createTranquilizerUI(quint64 id,const QString &title, const QString &description, double end);
    void removeTranquilizerUI(quint64 id);
    void tranquilizersChanged();

public slots:
    void updateTranquilizer(quint64 id, double amount);
    void createTranquilizer(quint64 id, const QString &title, const QString &description, double end);
    void removeTranquilizer(quint64 id);

private:

    QList<TranquilizerModel *> _tranquilizers;

};

Q_DECLARE_METATYPE(TranquilizerList)

#endif // TRANQUILIZERHANDLER_H
