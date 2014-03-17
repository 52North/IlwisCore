#ifndef PREPARATIONPARAMETERS_H
#define PREPARATIONPARAMETERS_H

namespace Ilwis {

// optional parameter for the creation process
struct PrepareOptions {
    PrepareOptions()  {}
    PrepareOptions(const QString filter) : _filter(filter) {}
    PrepareOptions(double undef) : _undefinedValue(undef){}
    PrepareOptions(const QString& un, const QString& pw) : _userName(un), _password(pw){}

    QString _filter = sUNDEF;
    double _undefinedValue = rUNDEF;
    QString _userName = sUNDEF;
    QString _password = sUNDEF;
};
}

#endif // PREPARATIONPARAMETERS_H
