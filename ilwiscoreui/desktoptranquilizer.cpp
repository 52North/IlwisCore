#include <QThread>
#include <QCoreApplication>
#include "kernel.h"
#include "tranquilizer.h"
#include "models/uicontextmodel.h"
#include "desktoptranquilizer.h"

using namespace Ilwis;
using namespace Geodrawer;

DesktopTranquilizer::DesktopTranquilizer(const IOOptions& opt, QObject *parent) : BaseTranquilizer(opt,parent)
{

}

DesktopTranquilizer::~DesktopTranquilizer()
{
    if ( _runsInMainThread) //  we do not update in main thread else everything is waiting
        return ;

    emit removeTranquilizer(_id);
}

bool DesktopTranquilizer::update(double step) {
    Locker<std::mutex> lock(_mutex);
    if ( uicontext()->abort() || _end == rUNDEF){
        return false;
    }
    if ( _runsInMainThread) //  we do not update in main thread else everything is waiting
        return true;
    _current += step;
    if ( _current >= _end  || _current < _start){
        emit(updateTranquilizer(_id, _current));
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        _end = rUNDEF;
        emit(removeTranquilizer(_id));
    }else
        emit(updateTranquilizer(_id, _current));

    return true;

}

void DesktopTranquilizer::stop()
{
    if ( _runsInMainThread) //  we do not update in main thread else everything is waiting
        return ;

    Locker<std::mutex> lock(_mutex);
    emit(removeTranquilizer(_id));
}

void DesktopTranquilizer::prepare(const QString &title, const QString &description, double end, double start)
{
    _runsInMainThread = QThread::currentThread() == QCoreApplication::instance()->thread();
    BaseTranquilizer::prepare(title, description,end, start);
    kernel()->connect(this, &BaseTranquilizer::updateTranquilizer, kernel(), &Kernel::changeTranquilizer);
    kernel()->connect(this, &BaseTranquilizer::removeTranquilizer, kernel(), &Kernel::removeTranquilizer);
    kernel()->newTranquilizer(_id, title, description, _start, _end);

}

Tranquilizer *DesktopTranquilizer::create(const IOOptions &opt)
{
    return new DesktopTranquilizer(opt,0);
}



