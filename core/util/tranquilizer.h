#ifndef TRANQUILIZER_H
#define TRANQUILIZER_H

#include <QObject>
#include <memory>
#include "locker.h"

namespace Ilwis {
class KERNELSHARED_EXPORT Tranquilizer : public QObject
{
    Q_OBJECT
public:
    explicit Tranquilizer(QObject *parent = 0);
    Tranquilizer(const QString &title, const QString &description, double end);
    void prepare(const QString &title, const QString &description, double end);


    void update(double step) {
        Locker<std::mutex> lock(_mutex);
        _current += step;
        emit(updateTranquilizer(_id, _current));

    }

    double current() const;
    void current(double cur);
    void end(double number);
    double end() const;

private:
    static quint64 _trqId;
    quint64 _id;
    QString _title;
    QString _desc;
    double  _end;
    double _current;
    std::mutex _mutex;
signals:
    void updateTranquilizer(quint64 id, double current);
    void tranquilizerCreated(quint64 id, const QString &title, const QString &description, quint64 end);
    
public slots:
    
};

}

#endif // TRANQUILIZER_H
