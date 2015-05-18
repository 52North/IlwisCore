#include <QThread>
#include <QCoreApplication>
#include <iostream>
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

// This tranquilizer will generate a progress indicator in a terminal window
// When fully processed it will write:
// 0..10..20..30..40..50..60..70..80..90..100
// The output is streamed to the stderr channel
ConsoleTranquilizer::ConsoleTranquilizer(const IOOptions &opt, QObject *parent) : BaseTranquilizer(opt, parent)
{

}

void ConsoleTranquilizer::prepare(const QString &title, const QString &description, double end, double start)
{
    BaseTranquilizer::prepare(title, description, end, start);

    _inc = (end - start) / 30;
    _next = 0.0;
    _count = 0;
}

bool ConsoleTranquilizer::update(double howfar)
{
    _current = howfar;
    if (_current >= _next) {
        if (_count % 3 == 0)
            std::cerr << _count * 10 / 3;   // round to integer between 0 and 100 in steps of 10
        else
            std::cerr << ".";
        _count++;
        _next += _inc;
    }
    return true;
}

bool ConsoleTranquilizer::inform(QString msg)
{
    std::cerr << msg.toStdString();
    return true;
}

void ConsoleTranquilizer::stop()
{
    _inc = 0.0;
    _next = 0.0;
    _count = 0;
    _start = 0.0;
    _end = 0.0;
    std::cerr << std::endl;
}

Tranquilizer *ConsoleTranquilizer::create(const IOOptions &opt)
{
    return new ConsoleTranquilizer(opt, 0);
}




