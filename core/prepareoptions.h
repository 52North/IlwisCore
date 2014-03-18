#ifndef PREPARATIONPARAMETERS_H
#define PREPARATIONPARAMETERS_H

namespace Ilwis {

// optional parameter for the creation process
struct PrepareOptions {
    PrepareOptions()  {}
    PrepareOptions(const QString filter) { _values["filter"] = filter; }
    PrepareOptions(double undef) { _values["undefined"] = undef; }
    PrepareOptions(const QString& un, const QString& pw) { _values["username"] = un; _values["password"] = pw;}

    std::map<QString, QVariant> _values;
};
}

#endif // PREPARATIONPARAMETERS_H
