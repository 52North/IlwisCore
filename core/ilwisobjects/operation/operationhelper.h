#ifndef OPERATIONHELPER_H
#define OPERATIONHELPER_H

#include <functional>

namespace Ilwis {

typedef std::function<bool()> CheckCondtion;

class KERNELSHARED_EXPORT OperationHelper
{
public:
    OperationHelper();
    static void initialize(const IIlwisObject &inputObject, Ilwis::IIlwisObject &outputObject, IlwisTypes tp, quint64 what);
    static void check(CheckCondtion func, const std::vector<QString>& errStrings){
        if ( ! func()){
            QString errMessage = errStrings[0];
            for (int i = 1; i < errStrings.size(); ++i){
                errMessage = errMessage.arg(errStrings[i]);
            }
            throw CheckExpressionError(errMessage);
        }

    }
};
}

#endif // OPERATIONHELPER_H
