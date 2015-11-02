#include <QThread>
#include <QCoreApplication>
#include "kernel.h"
#include "errorobject.h"
#include "factory.h"
#include "abstractfactory.h"
#include "tranquilizerfactory.h"
#include "tranquilizer.h"

using namespace Ilwis;

quint64 BaseTranquilizer::_trqId = 0;

BaseTranquilizer::BaseTranquilizer(const IOOptions &,QObject *parent) : Tranquilizer(parent),
 _id(i64UNDEF),_start(rUNDEF),_end(rUNDEF), _current(rUNDEF)
{

}

BaseTranquilizer::~BaseTranquilizer()
{
}

void BaseTranquilizer::prepare(const QString &title, const QString &description, double end, double start)
{
    _title = title;
    _desc = description;
    _end = end;
    _start = start;
    _current = _start;
    if ( _id == i64UNDEF)
        _id = _trqId++;
}

void BaseTranquilizer::stopTranquilizer()
{
    if ( QThread::currentThread() != QCoreApplication::instance()->thread()){

    }
}

void BaseTranquilizer::end(double number)
{
    _end = number;
}

double BaseTranquilizer::end() const
{
    return _end;
}

double BaseTranquilizer::current() const
{
    return _current;
}

void BaseTranquilizer::current(double cur)
{
    _current = cur;
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

//-------------------------------------
Tranquilizer::Tranquilizer(QObject *parent) : QObject(parent)
{

}

Tranquilizer::~Tranquilizer()
{

}

Tranquilizer *Tranquilizer::create(int mode, const IOOptions& options)
{
    TranquilizerFactory *factory = kernel()->factory<TranquilizerFactory>("ilwis::tranquilizerfactory");
    if (!factory){
        return 0;
    }
    return factory->create<>(mode, options);
}

//------------------------------------------------------
EmptyTranquilizer::EmptyTranquilizer(const IOOptions &opt, QObject *parent) : BaseTranquilizer(opt, parent)
{

}

void EmptyTranquilizer::prepare(const QString &title, const QString &description, double end, double start)
{
    BaseTranquilizer::prepare(title, description, end, start);
}

bool EmptyTranquilizer::update(double )
{
    return true;
}

void EmptyTranquilizer::stop()
{

}

Tranquilizer *EmptyTranquilizer::create(const IOOptions &opt)
{
    return new EmptyTranquilizer(opt,0);
}
