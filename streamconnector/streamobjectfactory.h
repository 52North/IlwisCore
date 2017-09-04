#ifndef StreamOBJECTFACTORY_H
#define StreamOBJECTFACTORY_H

namespace Ilwis {
namespace Stream {

class StreamObjectFactory : public IlwisObjectFactory
{
public:
    StreamObjectFactory();

    bool canUse(const Resource &resource) const;
    IlwisObject *create(const Resource &resource,const IOOptions& options=IOOptions()) const;
};
}
}

#endif // StreamOBJECTFACTORY_H
