#ifndef PREPARATIONPARAMETERS_H
#define PREPARATIONPARAMETERS_H

namespace Ilwis {

// optional parameter for the creation process
struct PrepareOptions {
    PrepareOptions()  {}
    PrepareOptions(const QString filter) { _values["filter"] = filter; }
    PrepareOptions(double undef) { _values["undefined"] = undef; }
    PrepareOptions(const QString& key, const QString& value) { _values[key] = value; }

    bool contains(const QString& option) const{
        return _values.find(option) != _values.end();
    }

    std::map<QString, QVariant> _values;
};
}

#endif // PREPARATIONPARAMETERS_H
