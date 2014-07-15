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
            return WARN2(ERR_COULD_NOT_LOAD_2,TR("configuration file"),configlocation);
        }

        boost::property_tree::json_parser::read_json(inputfile,_configuration);

        return true;
    } catch(const boost::property_tree::json_parser::json_parser_error& err){
        QString message = QString::fromStdString(err.message());
        return WARN2(ERR_NO_INITIALIZED_1,TR("configuration"),message);
    }
    return false;
}


