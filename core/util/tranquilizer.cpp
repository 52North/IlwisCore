#include "kernel.h"
#include "tranquilizer.h"

using namespace Ilwis;

quint64 Tranquilizer::_trqId = 0;

Tranquilizer::Tranquilizer(QObject *parent) :
    QObject(parent)
{

}

Tranquilizer::Tranquilizer(const QString& title, const QString& description, double end) :  QObject(0),
    _title(title), _desc(description), _end(end)
{
    _id = _trqId++;
}

void Tranquilizer::prepare(const QString &title, const QString &description, double end)
{
    _title = title;
    _desc = description;
    _end = end;
    kernel()->connect(this, &Tranquilizer::updateTranquilizer, kernel(), &Kernel::changeTranquilizer,Qt::DirectConnection);
    kernel()->newTranquilizer(_id, title, description, _end);

}

void Tranquilizer::end(double number)
{
    _end = number;
}

double Tranquilizer::end() const
{
    return _end;
}

double Tranquilizer::current() const
{
    return _current;
}

void Tranquilizer::current(double cur)
{
    _current = cur;
}
