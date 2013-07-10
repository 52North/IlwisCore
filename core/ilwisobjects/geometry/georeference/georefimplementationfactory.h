#ifndef GEOREFIMPLEMENTATIONFACTORY_H
#define GEOREFIMPLEMENTATIONFACTORY_H

namespace Ilwis {

class GeoRefImplementation;

typedef std::function<Ilwis::GeoRefImplementation *()> CreateGeoRefImplementation;

class KERNELSHARED_EXPORT GeoRefImplementationFactory : public AbstractFactory
{
public:
    GeoRefImplementationFactory();
    void addCreator(const QString& name, CreateGeoRefImplementation func);

    GeoRefImplementation *create(const QString &name);
    bool prepare();
private:
    std::map<QString,CreateGeoRefImplementation> _implCreators;
};
}

#endif // GEOREFIMPLEMENTATIONFACTORY_H
