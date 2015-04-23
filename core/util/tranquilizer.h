#ifndef TRANQUILIZER_H
#define TRANQUILIZER_H

#include <QObject>
#include <memory>
#include "locker.h"

namespace Ilwis {

class KERNELSHARED_EXPORT TranquilizerWorker : public QObject {
    Q_OBJECT

public:
    TranquilizerWorker();
public slots:

    void updateTranquilizer(quint64 id, double amount);
    void createTranquilizer(quint64 id, const QString &title, const QString &description, double start, double end);
    void removeTranquilizer(quint64 id);
signals:
    void sendUpdateTranquilizer(quint64 id, double amount);
    void sendCreateTranquilizer(quint64 id,const QString &title, const QString &description, double start, double end);
    void sendRemoveTranquilizer(quint64 id);
    //void sendtranquilizersChanged();
};


class KERNELSHARED_EXPORT Tranquilizer : public QObject
{
    Q_OBJECT
public:
    explicit Tranquilizer(QObject *parent = 0);
    Tranquilizer(const QString &title, const QString &description, double end,double start=0);
    void prepare(const QString &title, const QString &description, double end, double start=0);


    void update(double step);
    double current() const;
    void current(double cur);
    void end(double number);
    double end() const;
    void stop();

private:
    static quint64 _trqId;
    quint64 _id;
    QString _title;
    QString _desc;
    double _start;
    double  _end;
    double _current;
    std::mutex _mutex;
signals:
    void updateTranquilizer(quint64 id, double current);
    void removeTranquilizer(quint64 id);
    void tranquilizerCreated(quint64 id, const QString &title, const QString &description, quint64 end);
    
public slots:
    
};

}

#endif // TRANQUILIZER_H
