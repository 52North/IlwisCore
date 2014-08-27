#ifndef TRANQUILIZER_H
#define TRANQUILIZER_H

#include <QObject>
#include <memory>
#include "locker.h"

namespace Ilwis {
class Tranquilizer : public QObject
{
    Q_OBJECT
public:
    explicit Tranquilizer(QObject *parent = 0);
    Tranquilizer(const QString &title, const QString &description, qint64 end, qint32 step);
    void end(qint64 number);

    void move() {
        if ( _current % _step){
            Locker<std::mutex> lock(_mutex);
            emit(doMove(_id, _current));
        }
        _current += _step;

    }

private:
    static quint64 _trqId;
    quint64 _id;
    QString _title;
    QString _desc;
    qint64  _end;
    qint64 _current;
    qint32 _step;
    std::mutex _mutex;
signals:
    void doMove(quint64 id, qint32 amount);
    
public slots:
    
};

}

#endif // TRANQUILIZER_H
