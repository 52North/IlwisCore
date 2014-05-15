#include "kernel.h"
#include "mastercatalog.h"
#include "ilwiscontext.h"
#include "identity.h"
#include "user.h"

using namespace Ilwis;

User::User()
{
    _masterCatalog.reset(new MasterCatalog());
    _context.reset(new IlwisContext());
}
