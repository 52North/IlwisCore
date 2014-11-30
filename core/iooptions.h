#ifndef IOOptions_H
#define IOOptions_H

#include "kernel_global.h"

namespace Ilwis {

// optional parameter for the creation process
class KERNELSHARED_EXPORT IOOptions {
public:
    class Option{
        friend class IOOptions;
    public:
        Option(const QString& key, const QVariant& value) : _key(key), _value(value) {}

    private:
        QString _key;
        QVariant _value;

    };

    IOOptions()  {}
    IOOptions(const QString& key, const QVariant& value) { _values[key] = value; }

    bool contains(const QString& option) const;
    quint32 size() const;
    bool isEmpty() const;
    QVariant operator[](const QString& option) const;

    IOOptions &operator<<(const Option& option);
    IOOptions &addOption(const QPair<QString, QVariant> &item);
    IOOptions &addOption(const QString& key, const QVariant& value);

private:
    std::map<QString, QVariant> _values;
};



const QString IMPLICITPARMATER="_implicitparm_";
const QString IMPLICITPARMATER0="_implicitparm_0";
const QString IMPLICITPARMATER1="_implicitparm_1";
const QString IMPLICITPARMATER2="_implicitparm_2";
const QString IMPLICITPARMATER3="_implicitparm_3";
}

#endif // IOOptions_H
