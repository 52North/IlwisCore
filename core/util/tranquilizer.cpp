#include <QThread>
#include <QCoreApplication>
#include "kernel.h"
#include "tranquilizer.h"

using namespace Ilwis;

quint64 Tranquilizer::_trqId = 0;

Tranquilizer::Tranquilizer(QObject *parent) :
    QObject(parent), _id(i64UNDEF),_start(0),_end(0), _current(0)
{

}

Tranquilizer::Tranquilizer(const QString& title, const QString& description, double end, double start) :  QObject(0)
{
    prepare(title, description,end,start);
}

void Tranquilizer::prepare(const QString &title, const QString &description, double end, double start)
{
    _title = title;
    _desc = description;
    _end = end;
    _start = start;
    _current = _start;
    _id = _trqId++;
    kernel()->connect(this, &Tranquilizer::updateTranquilizer, kernel(), &Kernel::changeTranquilizer);
    kernel()->connect(this, &Tranquilizer::removeTranquilizer, kernel(), &Kernel::removeTranquilizer);
    kernel()->newTranquilizer(_id, title, description, _start, _end);

}

void Tranquilizer::end(double number)
{
    _end = number;
}

double Tranquilizer::end() const
{
    return _end;
}

void Tranquilizer::stop()
{
    Locker<std::mutex> lock(_mutex);
    emit(removeTranquilizer(_id));
}

double Tranquilizer::current() const
{
    return _current;
}

void Tranquilizer::current(double cur)
{
    _current = cur;
}

void Tranquilizer::update(double step) {
    Locker<std::mutex> lock(_mutex);
    _current += step;
    if ( _current >= _end || _current < _start){
        emit(updateTranquilizer(_id, _current));
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        emit(removeTranquilizer(_id));
    }else
        emit(updateTranquilizer(_id, _current));

}

//-------------------------------------------------------------
TranquilizerWorker::TranquilizerWorker()
{

}

void TranquilizerWorker::updateTranquilizer(quint64 id, double amount)
{
    emit sendUpdateTranquilizer(id, amount);
}

void TranquilizerWorker::createTranquilizer(quint64 id, const QString &title, const QString &description, double start, double end)
{
    emit sendCreateTranquilizer(id, title, description, start, end);
}

void TranquilizerWorker::removeTranquilizer(quint64 id)
{
    emit sendRemoveTranquilizer(id);
}
