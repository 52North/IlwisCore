#include <QDesktopServices>
#include <iostream>
#include "kernel.h"
#include "errmessages.h"
#include "ilwisconfiguration.h"

using namespace Ilwis;

IlwisConfiguration::IlwisConfiguration()
{
}

bool IlwisConfiguration::prepare(const QString &configlocation)
{
    try{
    std::ifstream inputfile;
    inputfile.open(configlocation.toStdString().c_str(),std::ifstream::in);
    if (!inputfile.is_open()) {
        std::cerr << TR("warning : Could not load configuration file").toStdString(); // dont follow logger here as this might be before the logger is initialized
        return false;
    }
    _configLocation = configlocation;
    boost::property_tree::json_parser::read_json(inputfile,_configuration);

    return true;
    } catch(const boost::property_tree::json_parser::json_parser_error& err){
        //QString message = QString::fromStdString(err.message());
        std::cerr << TR("warning : invalid json file for configuration").toStdString();
    }
    return false;
}

void IlwisConfiguration::eraseChildren(const QString &key)
{
    QString tempkey = key;
    tempkey.replace("/",".");
    auto ptree = _configuration.get_child(tempkey.toStdString());
    for(auto it = ptree.begin();it != ptree.end();){
        it = ptree.erase(it);
    }

    _modified = true;
}

void IlwisConfiguration::addValue(const QString &key, const QString& value){
    QString tempkey = key;
    tempkey.replace("/",".");

    _configuration.put(tempkey.toStdString(), value.toStdString());
    _modified = true;
}

void IlwisConfiguration::putValue(const QString &key, const QString &value)
{
    QString tempkey = key;
    tempkey.replace("/",".");

    //TODO bit unclear if put also replaces values
    _configuration.put(tempkey.toStdString(), value.toStdString());

    _modified = true;
}



void IlwisConfiguration::store(const QString &location)
{
    try {
    if ( _modified) {
        std::string loc = location.toStdString() ; //== sUNDEF ? _configLocation.toStdString() : location.toStdString();
        if ( loc == sUNDEF){
            loc = QString(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/ilwis.config").toStdString();

        }
        boost::property_tree::json_parser::write_json(loc,_configuration);
    }
    }catch(const std::exception& ex){
        kernel()->issues()->log(ex.what());
    }
}



