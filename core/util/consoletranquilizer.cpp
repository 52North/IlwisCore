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
    stop();
}

// This tranquilizer will generate a progress indicator in a terminal window
// When fully processed it will write:
// 0..10..20..30..40..50..60..70..80..90..100
// The output is streamed to the stdout channel
// Note that when the total is less then 30, the indicator will not reach the 100 mark
ConsoleTranquilizer::ConsoleTranquilizer(const IOOptions &opt, QObject *parent) : BaseTranquilizer(opt, parent)
{
    _inc = 0.0;
    _next = 0.0;
    _count = 0;
}

void ConsoleTranquilizer::prepare(const QString &title, const QString &description, double end, double start)
{
    BaseTranquilizer::prepare(title, description, end, start);

    // handle case where existing tranquilizer is re-used
    if (_count > 0)
        std::cout << std::endl;

     _inc = (end - start) / 30;
    _next = 0.0;
    _count = 0;

    std::cout << title.toStdString() << ": ";
}

bool ConsoleTranquilizer::update(double howfar)
{
    if (howfar < 1) {
        stop();
        return false;
    }

    _current += howfar;

    if (_current >= _next) {
         if (_count % 3 == 0)
             std::cout << _count * 10 / 3;   // round to integer between 0 and 100 in steps of 10
         else
             std::cout << ".";
         _count++;
         _next += _inc;
     }

    return true;
}

bool ConsoleTranquilizer::inform(QString msg)
{
    std::cout << msg.toStdString();
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




