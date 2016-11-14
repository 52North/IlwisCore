#ifndef MODELLERFACTORY_H
#define MODELLERFACTORY_H

#include <functional>
#include "factory.h"
#include "abstractfactory.h"
#include "kernel_global.h"

namespace Ilwis {

class AnalysisPattern;
class ModelApplication;

typedef std::function<AnalysisPattern *(const QString& name, const QString& description, const IOOptions &options)> CreateAnalysisPattern;
typedef std::function<ModelApplication *(const QString& name, const QString& description, const IOOptions &options)> CreateModelApplication;

class KERNELSHARED_EXPORT ModellerFactory : public AbstractFactory
{
public:
    ModellerFactory();

    AnalysisPattern *createAnalysisPattern(const QString type,const QString& name, const QString& description, const IOOptions &options);
    ModelApplication *createApplication(const QString type, const QString &name, const QString &description, const IOOptions &options);

    QStringList analysisTypes() const;

    static AnalysisPattern * registerAnalysisPattern(const QString& classname, CreateAnalysisPattern createFunc);
    static ModelApplication * registerModelApplication(const QString& classname, CreateModelApplication createFunc);


private:
    void registerAnalysisPatternInternal(const QString& classname, CreateAnalysisPattern createFunc);
    void registerModelApplicationInternal(const QString& classname, CreateModelApplication createFunc);

    std::map<QString, CreateAnalysisPattern>  _analysisCreators;
    std::map<QString, CreateModelApplication>  _applicationCreators;
};
}

#endif // MODELLERFACTORY_H
