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
    }

    boost::property_tree::json_parser::read_json(inputfile,_configuration);
    int port = _configuration.get("server-settings.port",120);

    return true;
    } catch(const boost::property_tree::json_parser::json_parser_error& err){
        //QString message = QString::fromStdString(err.message());
        std::cerr << TR("warning : invalid json file for configuration").toStdString();
    }
    return false;
}


