#ifndef PYTHONOBJECTFACTORY_H
#define PYTHONOBJECTFACTORY_H

namespace Ilwis{
namespace PythonScript {
class PythonObjectFactory : public IlwisObjectFactory
{
public:
    PythonObjectFactory();
    IlwisObject *create(const Ilwis::Resource &resource,const IOOptions& options=IOOptions()) const;
    bool canUse(const Resource &resource) const;
    bool prepare();
};
}
}

#endif // PYTHONOBJECTFACTORY_H
