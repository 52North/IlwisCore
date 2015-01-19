#ifndef ILWISCONFIGURATION_H
#define ILWISCONFIGURATION_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace Ilwis {
class KERNELSHARED_EXPORT IlwisConfiguration
{
public:
    IlwisConfiguration();
    bool prepare(const QString& configlocation);

    template<typename ValueType> ValueType operator ()(const QString& key, const ValueType& defaultValue){
        QString tempkey = key;
        tempkey.replace("/",".");
        ValueType v = _configuration.get(tempkey.toStdString(),defaultValue);
        return v;
    }
    void eraseChildren(const QString& key);
    void store(const QString &location=sUNDEF);
    void addValue(const QString &key, const QString &value);
    void putValue(const QString &key, const QString &value);
private:
   boost::property_tree::ptree _configuration;
   QString _configLocation;
   bool _modified = false;
};

template<> inline QString IlwisConfiguration::operator ()(const QString& key, const QString& defaultValue){
    QString tempkey = key;
    tempkey.replace("/",".");
    std::string v = _configuration.get(tempkey.toStdString(),defaultValue.toStdString());
    return QString::fromStdString(v);
}
}

#endif // ILWISCONFIGURATION_H
