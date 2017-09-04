#ifndef WFSOBJECTFACTORY_H
#define WFSOBJECTFACTORY_H

namespace Ilwis {

namespace Wfs {

class WfsObjectFactory : public IlwisObjectFactory
{
public:
    WfsObjectFactory();
    IlwisObject *create(const Resource &resource,const IOOptions& options=IOOptions()) const;
    bool canUse(const Resource &resource) const;
};

}
}



#endif // WfsOBJECTFACTORY_H
