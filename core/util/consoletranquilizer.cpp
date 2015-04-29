#include <QThread>
#include <QCoreApplication>
#include "kernel.h"
#include "errorobject.h"
#include "factory.h"
#include "abstractfactory.h"
#include "tranquilizer.h"
#include "consoletranquilizer.h"

using namespace Ilwis;

ConsoleTranquilizer::~ConsoleTranquilizer()
{

}

ConsoleTranquilizer::ConsoleTranquilizer(const IOOptions &opt, QObject *parent) : BaseTranquilizer(opt, parent)
{

}

void ConsoleTranquilizer::prepare(const QString &title, const QString &description, double end, double start)
{
    BaseTranquilizer::prepare(title, description, end, start);
}

bool ConsoleTranquilizer::update(double step)
{
    return true;
}

void ConsoleTranquilizer::stop()
{

}

Tranquilizer *ConsoleTranquilizer::create(const IOOptions &opt)
{
    return new ConsoleTranquilizer(opt,0);
}




