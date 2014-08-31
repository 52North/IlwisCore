#ifndef OPERATIONSPEC_H
#define OPERATIONSPEC_H

#include "operation.h"

namespace Ilwis {

template<> QString Operation::stringRepresentation<QString>(const QString& var){
    return var;
}

template<> QString Operation::stringRepresentation<IIlwisObject>(const IIlwisObject& var){
    return var->source().url().toString();
}

template<> QString Operation::stringRepresentation<Ilwis::IRasterCoverage>(const Ilwis::IRasterCoverage& var){
    return var->source().url().toString();
}

template<> QString Operation::stringRepresentation<Ilwis::IFeatureCoverage>(const Ilwis::IFeatureCoverage& var){
    return var->source().url().toString();
}

template<> QString Operation::stringRepresentation<Ilwis::ITable>(const Ilwis::ITable& var){
    return var->source().url().toString();
}

template<> QString Operation::stringRepresentation<Ilwis::IDomain>(const Ilwis::IDomain& var){
    return var->source().url().toString();
}

template<> QString Operation::stringRepresentation<Ilwis::ICoordinateSystem>(const Ilwis::ICoordinateSystem& var){
    return var->source().url().toString();
}

template<> QString Operation::stringRepresentation<Ilwis::IGeoReference>(const Ilwis::IGeoReference& var){
    return var->source().url().toString();
}

template<> QString Operation::stringRepresentation<Ilwis::BoundingBox>(const Ilwis::BoundingBox& var){
    return var.toString();
}

template<> QString Operation::stringRepresentation<Ilwis::Envelope>(const Ilwis::Envelope& var){
    return var.toString();
}

template<> QString Operation::stringRepresentation<int>(const int& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<unsigned int>(const unsigned int& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<unsigned short>(const unsigned short& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<short>(const short& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<unsigned char>(const unsigned char& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<char>(const char& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<unsigned long>(const unsigned long& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<long>(const long& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<quint64>(const quint64& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<qint64>(const qint64& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<float>(const float& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<double>(const double& var){
    return QString::number(var);
}

template<> QString Operation::stringRepresentation<std::string>(const std::string& var){
    return QString::fromStdString(var);
}


}

#endif // OPERATIONSPEC_H
