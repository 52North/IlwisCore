#ifndef ILWISCONFIGURATION_H
#define ILWISCONFIGURATION_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace Ilwis {
class IlwisConfiguration
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

private:
   boost::property_tree::ptree _configuration;
};

template<> inline QString IlwisConfiguration::operator ()(const QString& key, const QString& defaultValue){
    QString tempkey = key;
    tempkey.replace("/",".");
    tempkey = "config." + tempkey;
    std::string v = _configuration.get(tempkey.toStdString(),defaultValue.toStdString());
    return QString::fromStdString(v);
}
}

#endif // ILWISCONFIGURATION_H
