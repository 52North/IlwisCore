#ifndef MODELLERFACTORY_H
#define MODELLERFACTORY_H

#include <functional>
#include "factory.h"
#include "abstractfactory.h"
#include "kernel_global.h"

namespace Ilwis {

class AnalysisPattern;
class ApplicationModel;

typedef std::function<AnalysisPattern *(const QString& name, const QString& description, const IOOptions &options)> CreateAnalysisPattern;
typedef std::function<ApplicationModel *()> CreateModelApplication;

class KERNELSHARED_EXPORT ModellerFactory : public AbstractFactory
{
public:
    ModellerFactory();

    AnalysisPattern *createAnalysisPattern(const QString type,const QString& name, const QString& description, const IOOptions &options);
    ApplicationModel *createApplication(const QString type);

    QStringList analysisTypes() const;
    QStringList applications(const QString& analysisType);

    static AnalysisPattern * registerAnalysisPattern(const QString& classname, CreateAnalysisPattern createFunc);
    static ApplicationModel * registerModelApplication(const QString& classname, CreateModelApplication createFunc);


private:
    void registerAnalysisPatternInternal(const QString& classname, CreateAnalysisPattern createFunc);
    void registerModelApplicationInternal(const QString& classname, CreateModelApplication createFunc);

    std::map<QString, CreateAnalysisPattern>  _analysisCreators;
    std::map<QString, CreateModelApplication>  _applicationCreators;
};
}

#endif // MODELLERFACTORY_H
