#ifndef PREPARATIONPARAMETERS_H
#define PREPARATIONPARAMETERS_H

namespace Ilwis {

// optional parameter for the creation process
struct PrepareOptions {
    PrepareOptions()  {}
    PrepareOptions(const QString& filter) { _values["filter"] = filter; }
    PrepareOptions(double undef) { _values["undefined"] = undef; }
    PrepareOptions(const QString& key, const QVariant& value) { _values[key] = value; }

    bool contains(const QString& option) const{
        return _values.find(option) != _values.end();
    }

    std::map<QString, QVariant> _values;
};

const QString IMPLICITPARMATER="_implicitparm_";
const QString IMPLICITPARMATER0="_implicitparm_0";
const QString IMPLICITPARMATER1="_implicitparm_1";
const QString IMPLICITPARMATER2="_implicitparm_2";
const QString IMPLICITPARMATER3="_implicitparm_3";
}

#endif // PREPARATIONPARAMETERS_H
