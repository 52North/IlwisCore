#ifndef USER_H
#define USER_H

namespace Ilwis {

class Kernel;
class MasterCatalog;
class IlwisContext;

class User : public Identity
{
public:
    User();


private:
    std::unique_ptr<MasterCatalog> _masterCatalog;
    std::unique_ptr<IlwisContext> _context;

};
}

#endif // USER_H
