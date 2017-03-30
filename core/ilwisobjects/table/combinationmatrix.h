#ifndef COMBINATIONMATRIX_H
#define COMBINATIONMATRIX_H

#include "kernel_global.h"

namespace Ilwis {
class KERNELSHARED_EXPORT CombinationMatrix : public IlwisObject
{
public:
    enum Axis {aXAXIS, aYAXIS};
    CombinationMatrix();

    CombinationMatrix(const Resource &resource);
    IlwisTypes ilwisType() const;

    DataDefinition axisDefinition( Axis axis) const;
    void axisDefinition(Axis axis,const DataDefinition& coldef);
    DataDefinition combinationDef() const;
    void combinationDef(const DataDefinition& def);
    QString comboAsString(const QString& xValue, const QString& yValue) const;
    QString comboAsString(int x, int y) const;
    double combo(const QString& xValue, const QString& yValue);
    double combo(int x, int y) const;
    void combo(int x, int y, double v);
    QString axisValue(Axis axis,int index);
    void axisValues(Axis axis, const std::vector<QString>& values);
    double axisValueCount(Axis axis) const;

    IlwisObject *clone();
private:
    std::vector<DataDefinition> _axisDef;
    DataDefinition _comboDef;
    std::vector<std::vector<QString>> _axisValues;

    std::vector<std::vector<double>> _combos;
    void copyTo(IlwisObject *obj);
};

typedef Ilwis::IlwisData<Ilwis::CombinationMatrix> ICombinationMatrix;

}
Q_DECLARE_METATYPE(Ilwis::ICombinationMatrix);


#endif // COMBINATIONMATRIX_H
