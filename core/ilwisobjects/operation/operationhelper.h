#ifndef OPERATIONHELPER_H
#define OPERATIONHELPER_H

#include <functional>
#include "containerstatistics.h"

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
    static double statisticalMarker(const std::vector<double>& values, NumericStatistics::PropertySets method){
        double result = rUNDEF;
        if ( method == NumericStatistics::pMEAN){
            double sum=0;
            double count = 0;
            for(auto v : values){
                if ( !isNumericalUndef(v)){
                    sum += v;
                    ++count;
                }
            }
            result = count == 0 ? rUNDEF : sum / count;
        }else if (method == NumericStatistics::pMIN){
            double minv = 1e308;
            for(auto v : values){
                minv = Ilwis::min(minv, v);
            }
            result = minv != 1e308 ? minv : rUNDEF;
        }else if (method == NumericStatistics::pMAX){
            double maxv = -1e308;
            for(auto v : values){
                maxv = Ilwis::max(maxv, v);
            }
            result = maxv != 1e308 ? maxv : rUNDEF;
        }
        else if (method == NumericStatistics::pMEDIAN){
            std::vector<double> orderedNumbers;
            orderedNumbers.reserve(values.size());
            for(auto v : values){
                if ( !isNumericalUndef(v)){
                    orderedNumbers.push_back(v);
                }
            }
            std::sort(orderedNumbers.begin(), orderedNumbers.end());
            result = orderedNumbers.size() > 0 ? orderedNumbers[(int)(orderedNumbers.size() / 2)] : rUNDEF;
        }else if (method == NumericStatistics::pPREDOMINANT){
            std::map<double, qint32> counts;
            for(auto v : values){
                counts[v]++;
            }
            double maxCount = -1e308;
            for(auto item : counts){
                double cmax = Ilwis::max(item.second, maxCount);
                if ( cmax > maxCount){
                    result = item.first;
                    maxCount = cmax;
                }
            }
        }
        return result;
    }


    static IlwisTypes determineType(const QString &value);
    static IIlwisObject variant2ilwisobject(const QVariant &variant, IlwisTypes tp);
    static QString variant2string(const QVariant &v, IlwisTypes tp);
    static QString unquote(const QString &name);
};
}

#endif // OPERATIONHELPER_H
