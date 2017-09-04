#ifndef SpreadSheetOBJECTFACTORY_H
#define SpreadSheetOBJECTFACTORY_H

namespace Ilwis {
namespace SpreadSheetConnectors {

class SpreadSheetConnectorsObjectFactory : public IlwisObjectFactory
{
public:
    SpreadSheetConnectorsObjectFactory();

    bool canUse(const Resource &resource) const;
    IlwisObject *create(const Resource &resource,const IOOptions& options=IOOptions()) const;
private:
};
}
}

#endif // SpreadSheetOBJECTFACTORY_H
