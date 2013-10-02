#include "kernel.h"
#include "tranquilizer.h"

using namespace Ilwis;

quint64 Tranquilizer::_trqId = 0;

Tranquilizer::Tranquilizer(QObject *parent) :
    QObject(parent)
{

}

Tranquilizer::Tranquilizer(const QString& title, const QString& description, qint64 end, qint32 step) :  QObject(0),
    _title(title), _desc(description), _end(end), _step(step)
{
    _id = _trqId++;
}

void Tranquilizer::end(qint64 number)
{
    _end = number;
}
